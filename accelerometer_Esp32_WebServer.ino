#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// ADXL335 Pins
#define PIN_X 34
#define PIN_Y 35
#define PIN_Z 32

// ADC Calibration
#define ADC_RESOLUTION 4095.0
#define VREF 3.3
#define ZERO_G_VOLTAGE 1.65
#define SENSITIVITY 0.330

// WiFi Credentials
const char* ssid = "rover_wifi";
const char* password = "87654321";

// Web Server
AsyncWebServer server(80);
AsyncEventSource events("/events");

// HTML Web Page
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ADXL335 Accelerometer</title>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <script>
    let dataX = [], dataY = [], dataZ = [];

    const updateData = (event) => {
      let sensor = JSON.parse(event.data);
      if (dataX.length > 50) { 
        dataX.shift();
        dataY.shift();
        dataZ.shift();
      }
      dataX.push(sensor.x);
      dataY.push(sensor.y);
      dataZ.push(sensor.z);

      chart.data.labels = Array(dataX.length).fill('');
      chart.data.datasets[0].data = dataX;
      chart.data.datasets[1].data = dataY;
      chart.data.datasets[2].data = dataZ;

      chart.update();  // Update chart
    };

    document.addEventListener("DOMContentLoaded", function() {
  function startEventSource() {
    const evtSource = new EventSource("/events");
    
    evtSource.addEventListener("accelData", updateData);
    
    evtSource.addEventListener("reload", () => {
      console.log("WiFi Reconnected! Reloading...");
      location.reload();
    });

    evtSource.onerror = function() {
      console.error("EventSource disconnected. Reconnecting...");
      setTimeout(() => {
        startEventSource(); // Restart EventSource after a delay
      }, 2000);
    };
  }

  startEventSource(); // Call the function after defining it




      const ctx = document.getElementById('accelChart').getContext('2d');
      window.chart = new Chart(ctx, {
        type: 'line',
        data: {
          labels: [],
          datasets: [
            { label: 'X', borderColor: 'blue', data: dataX, fill: false },
            { label: 'Y', borderColor: 'red', data: dataY, fill: false },
            { label: 'Z', borderColor: 'green', data: dataZ, fill: false }
          ]
        },
        options: {
          animation: false, // Disable animations for smooth updates
          responsive: true,
          scales: { y: { suggestedMin: -1, suggestedMax: 1 } }
        }
      });
    });
  </script>
</head>
<body>
  <h2>ADXL335 Accelerometer Data (Live Graph)</h2>
  <canvas id="accelChart" width="400" height="200"></canvas>
</body>
</html>
)rawliteral";

// Reconnect WiFi if Disconnected
void checkWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Disconnected! Reconnecting...");
    WiFi.disconnect();
    WiFi.reconnect();
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi Reconnected! IP: " + WiFi.localIP().toString());
    // reload graph
    events.send("", "reload", millis());
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", webpage);
  });

  server.addHandler(&events);
  server.begin();
}

void loop() {
  checkWiFi();  // Ensure WiFi stays connected

  int raw_x = analogRead(PIN_X);
  int raw_y = analogRead(PIN_Y);
  int raw_z = analogRead(PIN_Z);

  float voltage_x = (raw_x / ADC_RESOLUTION) * VREF;
  float voltage_y = (raw_y / ADC_RESOLUTION) * VREF;
  float voltage_z = (raw_z / ADC_RESOLUTION) * VREF;

  float acc_x = (voltage_x - ZERO_G_VOLTAGE) / SENSITIVITY;
  float acc_y = (voltage_y - ZERO_G_VOLTAGE) / SENSITIVITY;
  float acc_z = (voltage_z - ZERO_G_VOLTAGE) / SENSITIVITY;

  Serial.print(acc_x);
  Serial.print(" ");
  Serial.print(acc_y);
  Serial.print(" ");
  Serial.println(acc_z);

  // Send data
  String json;
  StaticJsonDocument<200> doc;
  doc["x"] = acc_x;
  doc["y"] = acc_y;
  doc["z"] = acc_z;
  serializeJson(doc, json);

  events.send(json.c_str(), "accelData", millis());

  delay(100);  // Update every 100ms for continuous updates
}
