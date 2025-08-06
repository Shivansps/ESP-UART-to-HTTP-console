#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "index_html.h"

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const bool verbose = false; // Send wifi IP and client connect/disconnects notifications to uart host


AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

int currentBaud = 115200;
int clients = 0;
String serialBuffer = "";

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    delay(100);
    clients++;
    if(verbose) {
      Serial.println("ESP UART to HTTP. Client Connected.");
    }
  } else if (type == WS_EVT_DISCONNECT) {
    clients--;
    if(verbose) {
      Serial.println("ESP UART to HTTP. Client Disconnected.");
    }
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      String msg = "";
      for (size_t i = 0; i < len; i++) msg += (char)data[i];
      msg.replace("\n", "\r\n");  // CRLF
      Serial.print(msg);
    }
  }
}

void loadBaudFromFS() {
  File file = LittleFS.open("/baud.txt", "r");
  if (!file) return;
  currentBaud = file.readString().toInt();
  if (currentBaud <= 0) currentBaud = 115200;
  file.close();
}

void saveBaudToFS(int baud) {
  File file = LittleFS.open("/baud.txt", "w");
  if (!file) return;
  file.print(baud);
  file.close();
}

void setup() {
  Serial.begin(115200); // default for debug
  LittleFS.begin();
  loadBaudFromFS();
  Serial.begin(currentBaud);  // Apply saved baud
  if(verbose){
    Serial.print("ESP UART to HTTP. Connecting to Wifi...\n\r");
  }
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  if(verbose){
    Serial.print("ESP UART to HTTP. WiFi IP: ");
    Serial.println(WiFi.localIP());
  }
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  // Serve HTML
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", indexHtml);
  });

  // Handle baudrate change
  server.on("/setBaud", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasArg("baud")) {
      int baud = request->arg("baud").toInt();
      if (baud > 0) {
        Serial.begin(baud);
        currentBaud = baud;
      }
    }
    request->send(200, "text/plain", "OK");
  });

  // Save baudrate persistently
  server.on("/saveBaud", HTTP_GET, [](AsyncWebServerRequest *request){
    saveBaudToFS(currentBaud);
    request->send(200, "text/plain", "Saved");
  });

  // Get current baudrate
  server.on("/getBaud", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(currentBaud));
  });

  // Get StoredData
  server.on("/getStored", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", serialBuffer);
    serialBuffer = "";
  });

  server.begin();
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    serialBuffer += c;
    if ((c == '\n' && clients > 0)) {
      ws.textAll(serialBuffer);
      serialBuffer = "";
    }
  }
}