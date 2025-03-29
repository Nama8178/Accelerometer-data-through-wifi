# ADXL335 Accelerometer with ESP32 and Web Interface

### Overview

This project interfaces an ADXL335 accelerometer with an ESP32 and provides real-time acceleration data via a web-based interface. The data is visualized using Chart.js and updated dynamically through Server-Sent Events (SSE).

### Features

- Reads acceleration data from the ADXL335 using ADC pins.
- Calibrates and converts raw ADC values to acceleration values.
- Hosts a web page to display real-time data.
- Uses EventSource (SSE) for efficient live data updates.
- Auto-reconnects WiFi if disconnected.

### Hardware Requirements
- ESP32 development board
- ADXL335 Accelerometer
- Jumper wires

### Wiring Diagram

- VCC  --------> 3.3V
- GND  -------> GND
- X  ----------> GPIO 34
- Y  ----------> GPIO 35
- z  ----------> GPIO 32

### Installation
 1. Clone this repository or copy the code into your Arduino IDE.
 2. Install required libraries:
       ##### a) ESPAsyncWebServer
       ##### b) ArduinoJson

3. Modify WiFi Credentials in the code:

        const char* ssid = "your_wifi_ssid";
        
        const char* password = "your_wifi_password";

5. Upload the code to your ESP32.

6. Open Serial Monitor (baud rate: 115200) to check for connectivity.

7. Access the web interface by visiting http://<ESP32-IP> in your browser.

### Usage

- The web interface shows a real-time graph of X, Y, and Z acceleration data.

- The ESP32 continuously reads sensor data and sends it via SSE to the webpage.

- If WiFi disconnects, the ESP32 will automatically reconnect and refresh the webpage.

### Example Output (Serial Monitor)

    0.02  -0.01  1.00
    
    0.03  -0.02  0.99

### Future Improvements

- Add data logging to store acceleration readings.

- Implement a mobile-friendly UI.

- Support MQTT for IoT applications.

Feel free to cotribute !!
