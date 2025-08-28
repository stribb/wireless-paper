# Project: wireless-paper

This project, `wireless-paper`, is an embedded application designed for the Heltec WiFi LoRa 32 (V3) board. It utilizes the ESP32-S3 microcontroller and the Arduino framework.

## Key Features

*   **Wireless Communication:** The project incorporates LoRa communication using the `SX126x-Arduino` library, enabling long-range, low-power data transmission.
*   **E-Paper Display:** It drives an e-paper display with the `heltec-eink-modules` library, which is ideal for low-power applications requiring persistent information display.
*   **Connectivity:** The project leverages the `WiFi` library for standard Wi-Fi connectivity and `ArduinoOTA` for convenient over-the-air firmware updates.

## Development Environments

The `platformio.ini` file defines two primary environments:

*   `env:wireless-paper-usb`: Configured for development and debugging via a USB connection.
*   `env:wireless-paper-ota`: Configured for deploying updates over-the-air.

## Dependencies

*   **Hardware:** Heltec WiFi LoRa 32 (V3)
*   **Framework:** Arduino
*   **Libraries:**
    *   `beegee-tokyo/SX126x-Arduino`
    *   `https://github.com/todd-herbert/heltec-eink-modules.git`
    *   `ArduinoOTA`
    *   `WiFi`
