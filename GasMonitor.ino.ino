
//----K.I.M Perera----
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"
#include <SoftwareSerial.h>

//----- HX711 Setup
#define DOUT 2
#define CLK 3
HX711 scale;

//----- I2C LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

//----- Pins
#define GREEN_LED 8
#define YELLOW_LED 9
#define RED_LED 10
#define BLUE_LED 4
#define BUZZER 11
#define MQ135 A0

//----- SIM800L Setup
#define SIM800_TX 7
#define SIM800_RX 6
SoftwareSerial sim800(SIM800_TX, SIM800_RX);

//----- Thresholds
long fullLevel = 1259700;
long mediumLevel = 782100;
long lowLevel = 500000;
long noCylinderLevel = 412600;
int gasThreshold = 200;

//----- Timers
unsigned long lastLowBeepTime = 0;
unsigned long lastGasBeepTime = 0;
unsigned long lastSmsTime = 0;
const unsigned long gasBeepInterval = 200;
const unsigned long smsInterval = 60000;
const unsigned long lowBeepInterval = 2000;

//----- Phone number
String phoneNumber = "+94#######"; 

//----- Gas state tracking
bool smsSentLeak = false;
bool smsSentClear = false;

void setup() {
  Serial.begin(9600);
  sim800.begin(9600);
  scale.begin(DOUT, CLK);
  scale.tare();
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Starting");
  lcd.setCursor(0, 1);
  lcd.print("Please Wait...");
  delay(2000);
  
  lcd.clear();
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
  Serial.println("Gas Level & Leak Monitor started...");
  
  // Initialize SIM800L
  sendAT("AT");
  sendAT("AT+CMGF=1");
  sendAT("AT+CNMI=1,2,0,0,0");
}

void loop() {
  unsigned long currentMillis = millis();
  
  //----- Weight Monitoring ---
  long reading = scale.read_average(10);
  String weightStatus;
  
  if (reading >= mediumLevel) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
    weightStatus = "High";
  } else if (reading >= lowLevel && reading < mediumLevel) {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    weightStatus = "Medium";
  } else if (reading >= noCylinderLevel && reading < lowLevel) {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    weightStatus = "Low";
    
    // Low gas volume alert every 2 seconds
    if (currentMillis - lastLowBeepTime >= lowBeepInterval) {
      digitalWrite(BUZZER, HIGH);
      delay(200);
      digitalWrite(BUZZER, LOW);
      lastLowBeepTime = currentMillis;
    }
  } else {
    // No cylinder
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
    weightStatus = "No cyl";
  }
  
  // ----- Gas Sensor Monitoring -----
  int gasValue = analogRead(MQ135);
  bool gasLeak = false;
  
  if (gasValue > gasThreshold) {
    gasLeak = true;
    digitalWrite(BLUE_LED, HIGH);
    
    // Gas leak rapid beep
    if (currentMillis - lastGasBeepTime >= gasBeepInterval) {
      tone(BUZZER, 3000, 100);
      lastGasBeepTime = currentMillis;
    }
    
    // Send SMS if gas leak detected
    if (!smsSentLeak || (currentMillis - lastSmsTime >= smsInterval)) {
      sendSMS(phoneNumber, "ALERT: Gas leak detected! Check cylinder immediately.");
      smsSentLeak = true;
      smsSentClear = false;
      lastSmsTime = currentMillis;
    }
  } else {
    gasLeak = false;
    digitalWrite(BLUE_LED, LOW);
    noTone(BUZZER);
  }
  
  // Send SMS when gas leak clears
  if (!smsSentClear && smsSentLeak) {
    sendSMS(phoneNumber, "Gas levels back to normal. No leak detected.");
    smsSentClear = true;
    smsSentLeak = false;
    lastSmsTime = currentMillis;
  }
  
  //----- LCD Display -----
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gas level ");
  lcd.print(weightStatus);
  lcd.setCursor(0, 1);
  if (gasLeak) lcd.print("Gas leak detect");
  else lcd.print("No gas leak");
  
  //----- Serial Monitor -----
  Serial.print("Weight: "); Serial.print(reading);
  Serial.print(" ("); Serial.print(weightStatus); Serial.print(") ");
  Serial.print("Gas: "); Serial.println(gasValue);
  
  delay(500);
}

//----- SIM800L Functions
void sendAT(String cmd) {
  sim800.println(cmd);
  delay(500);
  while (sim800.available()) Serial.write(sim800.read());
}

void sendSMS(String number, String message) {
  Serial.println("Sending SMS...");
  while(sim800.available()) sim800.read();
  sim800.println("AT+CMGF=1");
  delay(300);
  while(sim800.available()) sim800.read();
  sim800.print("AT+CMGS=\"");
  sim800.print(number);
  sim800.println("\"");
  delay(300);
  sim800.print(message);
  delay(300);
  sim800.write(26); // Ctrl+Z ASCII code
  delay(3000);
  Serial.println("SMS sent successfully.");
}