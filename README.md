# Smart Gas Cylinder Monitoring and Alert System 🚒

**Author:** K. I. M. Perera  
**Course:** ICT 3052.0 - Embedded Systems

## 📖 Project Overview
This project is a prototype-based Smart Gas Cylinder Monitoring System. It is designed to automatically monitor gas levels and detect hazardous leaks in domestic environments.

## 🚀 Key Features
* **Real-time Gas Level Monitoring:** Uses a 1kg load cell to detect if the cylinder is Full, Medium, Low, or Empty.
* **Gas Leak Detection:** Uses an MQ-135 sensor to detect hazardous leaks.
* **SMS Alerts:** Sends SMS notifications via SIM800L GSM module for low gas levels and leak detection.
* **Visual & Audio Indicators:** 16x2 LCD display, LED status lights, and Buzzer alerts.

## 🛠️ Hardware Components
1.  Arduino Uno
2.  Load Cell (1 kg) with HX711 Amplifier
3.  MQ-135 Gas Sensor
4.  SIM800L GSM Module
5.  16x2 LCD (I2C)
6.  LEDs & Buzzer

## 📸 How it Works
The system continuously weighs the cylinder. If the weight drops below a threshold, it alerts the user. Simultaneously, if the MQ-135 detects gas, it triggers an immediate alarm and sends an SMS alert.
