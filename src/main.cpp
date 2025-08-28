#include "heltec-eink-modules.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include "secrets.h"
#include <ArduinoOTA.h>

EInkDisplay_WirelessPaperV1_2 display;
WiFiMulti wifiMulti;

// Server will be dynamically allocated
WiFiServer* echoServer = nullptr;

// Forward declarations for our event handlers
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiLostIP(WiFiEvent_t event, WiFiEventInfo_t info);

// Helper function to update the display
void updateDisplay(const String& message) {
  display.clear();
  display.printCenter(message);
  display.update();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing display...");

  display.setRotation(-1);
  display.setTextSize(2);
  updateDisplay("Hello, eyeballs!");
  Serial.println("Display update complete!");

  // Register WiFi event handlers
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiLostIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  // Add APs to WiFiMulti
  for (int i = 0; i < NUM_WIFI_CREDENTIALS; i++) {
    wifiMulti.addAP(WIFI_CREDENTIALS[i].ssid, WIFI_CREDENTIALS[i].password);
  }

  Serial.println("Connecting to WiFi...");
  Serial.println("[TERMINAL] setup() complete, entering loop().");
}

// The loop() function is called repeatedly by the Arduino framework.
void loop() {
  wifiMulti.run();
  ArduinoOTA.handle();

  // Only handle clients if we are connected and the server object exists.
  if (WiFi.status() == WL_CONNECTED && echoServer != nullptr) {
    WiFiClient client = echoServer->available();
    if (client) {
      Serial.println("New client connected.");

      while (client.connected()) {
        ArduinoOTA.handle();
        if (client.available()) {
          String receivedData = client.readStringUntil('\n');
          receivedData.trim();
          Serial.print("Received from client: ");
          Serial.println(receivedData);
          client.println(receivedData);
          updateDisplay(receivedData);
        }
        delay(10);
      }

      client.stop();
      Serial.println("[TERMINAL] Client disconnected.");
    }
  }
}

// --- WiFi Event Handlers ---

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  updateDisplay("Hello, " + WiFi.SSID() + "\n" + WiFi.localIP().toString());

  // Delete the old server instance if it exists
  if (echoServer != nullptr) {
    Serial.println("Stopping old echo server.");
    echoServer->stop();
    delete echoServer;
    echoServer = nullptr;
  }

  // Create and start a new server instance
  Serial.println("Starting new echo server on port 7...");
  echoServer = new WiFiServer(7);
  echoServer->begin();
  Serial.println("Echo server started.");

  // OTA Setup
  ArduinoOTA.setHostname("WirelessPaper");
  ArduinoOTA.onStart([]() {
    Serial.println("OTA Update: Start");
    updateDisplay("OTA Update...");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA Update: End");
    updateDisplay("Update Complete!");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Update: Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Update: Error[%u]: ", error);
    String err;
    if (error == OTA_AUTH_ERROR) err = "Auth Failed";
    else if (error == OTA_BEGIN_ERROR) err = "Begin Failed";
    else if (error == OTA_CONNECT_ERROR) err = "Connect Failed";
    else if (error == OTA_RECEIVE_ERROR) err = "Receive Failed";
    else if (error == OTA_END_ERROR) err = "End Failed";
    updateDisplay("OTA Error!");
    Serial.println(err);
  });
  ArduinoOTA.begin();
  Serial.println("OTA service started.");
}

void WiFiLostIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("WiFi disconnected. Waiting for reconnection...");
  updateDisplay("WiFi Mislaid!");
  // The server object will be cleaned up on the next reconnect.
}