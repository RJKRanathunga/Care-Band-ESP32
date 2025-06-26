#include "BluetoothSerial.h"
#include <WiFi.h>
#include <ArduinoJson.h>  // Needed to parse JSON-formatted Bluetooth commands
#include <Preferences.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>


//############### wifi bluetooth
BluetoothSerial SerialBT;
Preferences prefs;

SemaphoreHandle_t wifiMutex;
volatile bool pauseMonitor = false;
volatile bool isUser_atHome = true;

// ###############  GPS
#define RXD2 16
#define TXD2 17
#define GPS_BAUD 9600
#define GPS_TIMEOUT 300000

TinyGPSPlus gps;
HardwareSerial gpsSerial(2);


struct GPSLocation {
  double latitude;
  double longitude;
};

// GSM serial
HardwareSerial gsm(1); // Use UART1 (pins 16, 17)

// ********************************* setup ************
void setup() {
  Serial.begin(115200);
  prefs.begin("wifi_prefs", false);
  wifiMutex = xSemaphoreCreateMutex();
  xTaskCreate(Zone_monitor, "ZoneMonitor", 4096, NULL, 1, NULL);
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);
  SerialBT.begin("ESP32_BT_Device"); // Bluetooth device name
  Serial.println("Bluetooth started!");
  WiFi.setAutoReconnect(false);
}

void Zone_monitor(void *pvParameters) {
  while (1) {
    if (pauseMonitor) {
      // If paused, skip this cycle (but not in the middle of one)
      vTaskDelay(100 / portTICK_PERIOD_MS);
      continue;
    }

    if (xSemaphoreTake(wifiMutex, portMAX_DELAY)) {
      Serial.println("Zone monitoring...");
      if (isUser_atHome){
        monitorZone();
      } else {
        trackUser();
      }
      xSemaphoreGive(wifiMutex);
    }

    if (isUser_atHome){
      vTaskDelay(3000 / portTICK_PERIOD_MS);  // monitorZone() function delay
    } else {
      vTaskDelay(1000*20 / portTICK_PERIOD_MS);  // trackUser() function delay
    }
  }
}


void loop() {
  if (SerialBT.available()) {
    String incoming = SerialBT.readStringUntil('\n'); // Read till newline for safer JSON parsing
    Serial.println("Received: " + incoming);
    SerialBT.println("Echo: " + incoming);
    handle_bluetooth_command(incoming);
  }
}
