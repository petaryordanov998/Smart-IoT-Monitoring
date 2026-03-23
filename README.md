# Smart IoT Monitoring & Alert System

## Overview

This project is an ESP32-based IoT system that monitors environmental data and sends alerts in real time.

It is designed as a **smart home / security system prototype** that can be controlled remotely via Telegram and visualized through a web dashboard.

---

## Features

### Monitoring

* Temperature (DHT11)
* Humidity (DHT11)
* Light level (LDR)
* Motion detection (PIR sensor)

### Alerts

* Motion detected → Telegram notification
* High temperature → warning
* Motion in darkness → 🚨 ALERT

### Remote Control

* Telegram Bot integration
* Commands:

  * `/home` → normal mode
  * `/away` → security mode

### Web Dashboard

* Real-time data visualization
* Chart.js graph for temperature
* Accessible via ESP32 local IP

### Data Logging

* Logs stored in SPIFFS (`/log.txt`)
* Includes temperature, humidity, light, motion

---

## System Architecture

ESP32 reads sensor data → processes conditions →

* sends alerts via Telegram
* serves dashboard via WebServer
* logs data to SPIFFS

---

## Hardware

* ESP32 (ESP32-WROOM-32)
* DHT11 temperature & humidity sensor
* LDR (light sensor)
* PIR motion sensor
* Breadboard & jumper wires

---

## Pin Configuration

| Component | ESP32 Pin |
| --------- | --------- |
| DHT11     | GPIO 4    |
| LDR (A0)  | GPIO 34   |
| PIR       | GPIO 5    |

---

## Setup

### 1. Install Arduino Libraries

* WiFi
* WebServer
* SPIFFS
* DHT sensor library
* UniversalTelegramBot
* ArduinoJson

---

### 2. Configure Secrets

Create a file `secrets.h` (NOT uploaded to GitHub):

```cpp
#pragma once

const char* WIFI_SSID = "YOUR_WIFI";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";

#define BOT_TOKEN "YOUR_BOT_TOKEN"
#define CHAT_ID "YOUR_CHAT_ID"
```

---

### 3. Upload Code

* Select board: **ESP32 Dev Module**
* Upload via Arduino IDE

---

### 4. Open Dashboard

* Open Serial Monitor
* Copy ESP32 IP address
* Open in browser

---

## Project Structure

```
Smart_IOT_Monitoring/
│
├── Smart_IoT_Monitoring.ino
├── secrets.h        (ignored)
└── .gitignore
```

---

## Security

Sensitive data is stored in `secrets.h` and excluded via `.gitignore`.

---

## Future Improvements

* Mobile app (Flutter / React Native)
* Cloud integration (Firebase / MQTT)
* AI anomaly detection
* Multiple sensors support

---

## Author

Petar Yordanov

---

## Project Goal

This project demonstrates:

* Embedded systems (ESP32)
* IoT architecture
* Real-time monitoring
* Web + Telegram integration
* Clean code & security practices
