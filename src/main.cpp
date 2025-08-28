#include "heltec-eink-modules.h"
#include <Arduino.h>
#include <WiFi.h>
#include "secrets.h"

EInkDisplay_WirelessPaperV1_2 display;
WiFiServer echoServer(7);

// Helper function to update the display
void updateDisplay(String message) {
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

  // Wi-Fi connection
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Add Wi-Fi diagnostic information
  WiFi.printDiag(Serial);

  int connectionAttempts = 0;
  while (WiFi.status() != WL_CONNECTED && connectionAttempts < 20) {
    delay(500);
    Serial.print(".");
    connectionAttempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    updateDisplay("Hello, Internet!\n" + WiFi.localIP().toString());

    // Start the echo server
    echoServer.begin();
    Serial.println("Echo server started on port 7");
  } else {
    Serial.println("");
    Serial.println("Failed to connect to WiFi!");
    updateDisplay("WiFi Failed!");
  }
}

void loop() {
  // Check if a client has connected
  WiFiClient client = echoServer.available();
  if (client) {
    Serial.println("New client connected");

    // Read data from the client
    while (client.connected()) {
      if (client.available()) {
        String receivedData = client.readStringUntil('\n');
        receivedData.trim();

        Serial.print("Received: ");
        Serial.println(receivedData);

        // Echo back the received data
        client.println(receivedData);

        // Display on e-ink
        updateDisplay(receivedData);
      }
    }
    client.stop();
    Serial.println("Client disconnected");
  }
}
