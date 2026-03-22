#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <UniversalTelegramBot.h>
#include "DHT.h"
#include "secrets.h"



WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// -------- Web Server --------
WebServer server(80);

// -------- DHT --------
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// -------- Pins --------
#define LDR_PIN 34
#define PIR_PIN 5

#define DARK_THRESHOLD 500

// -------- Mode --------
enum Mode {HOME, AWAY};
Mode currentMode = HOME;

// -------- Data --------
float temp = 0;
float hum = 0;
int lightLevel = 0;
int motion = 0;

// -------- Timing --------
unsigned long lastTelegramCheck = 0;
unsigned long lastLog = 0;

// ========================
// SPIFFS LOG
// ========================
void logData() {
  File file = SPIFFS.open("/log.txt", FILE_APPEND);
  if (file) {
    file.print(temp);
    file.print(",");
    file.print(hum);
    file.print(",");
    file.print(lightLevel);
    file.print(",");
    file.println(motion);
    file.close();
  }
}

// ========================
// WEB DASHBOARD
// ========================
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>

<h2>ESP32 Dashboard</h2>

<canvas id="chart" width="400" height="200"></canvas>

<script>
let labels = [];
let tempData = [];
let humData = [];

const ctx = document.getElementById('chart').getContext('2d');

const chart = new Chart(ctx, {
  type: 'line',
  data: {
    labels: labels,
    datasets: [
      {
        label: 'Temperature (°C)',
        data: tempData,
        borderColor: 'red',
        borderWidth: 2
      },
      {
        label: 'Humidity (%)',
        data: humData,
        borderColor: 'blue',
        borderWidth: 2
      }
    ]
  },
  options: {
    responsive: true,
    animation: false,
    scales: {
      y: { beginAtZero: true }
    }
  }
});

async function fetchData() {
  try {
    const res = await fetch('/data');
    const data = await res.json();

    labels.push(new Date().toLocaleTimeString());
    tempData.push(data.temp);
    humData.push(data.hum);

    if (labels.length > 10) {
      labels.shift();
      tempData.shift();
      humData.shift();
    }

    chart.update();
  } catch (e) {
    console.log("Error:", e);
  }
}

setInterval(fetchData, 2000);
</script>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

// ========================
void handleData() {
  String json = "{";
  json += "\"temp\":" + String(temp) + ",";
  json += "\"hum\":" + String(hum) + ",";
  json += "\"light\":" + String(lightLevel) + ",";
  json += "\"motion\":" + String(motion);
  json += "}";

  server.send(200, "application/json", json);
}

// ========================
// TELEGRAM COMMANDS
// ========================
void handleTelegram() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages) {
    for (int i = 0; i < numNewMessages; i++) {
      String text = bot.messages[i].text;

      if (text == "/home") {
        currentMode = HOME;
        bot.sendMessage(CHAT_ID, "🏠 Mode: HOME", "");
      }

      if (text == "/away") {
        currentMode = AWAY;
        bot.sendMessage(CHAT_ID, "🚨 Mode: AWAY", "");
      }
    }
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}

// ========================
// SETUP
// ========================
void setup() {
  Serial.begin(115200);

  // WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected: " + WiFi.localIP());

  client.setInsecure(); // Telegram SSL

  // Initialize sensors
  dht.begin();
  pinMode(PIR_PIN, INPUT);

  // SPIFFS
  SPIFFS.begin(true);

  // Web server
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();

  Serial.println("System Ready!");
}

// ========================
// LOOP
// ========================
void loop() {

  // --- Read sensors ---
  temp = dht.readTemperature();
  hum = dht.readHumidity();

  int raw = analogRead(LDR_PIN);
  lightLevel = 4095 - raw;

  motion = digitalRead(PIR_PIN);

  // --- Alerts ---
  if (currentMode == AWAY && motion == HIGH && lightLevel < DARK_THRESHOLD) {
    bot.sendMessage(CHAT_ID, "🚨 ALERT: Движение в тъмно!", "");
    delay(5000); // анти-спам
  }

  // --- Telegram check ---
  if (millis() - lastTelegramCheck > 2000) {
    handleTelegram();
    lastTelegramCheck = millis();
  }

  // --- Logging ---
  if (millis() - lastLog > 5000) {
    logData();
    lastLog = millis();
  }

  // --- Web ---
  server.handleClient();
}