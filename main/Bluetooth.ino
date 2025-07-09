// Handle Bluetooth command (expects JSON format like {"command":"Connect_to_wifi","ssid":"MySSID","password":"MyPass"})
void handle_bluetooth_command(String incoming) {
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, incoming);

  if (error) {
    Serial.println("Failed to parse command");
    return;
  }

  String command = doc["command"];

  // Step 1: Tell Zone_monitor to pause after finishing this loop
  pauseMonitor = true;

  // Step 2: Wait until the current Zone_monitor finishes and releases mutex
  if (xSemaphoreTake(wifiMutex, portMAX_DELAY)) {
    Serial.println("🔧 Running user command...");

    if (command == "connect_wifi") {
      const char* ssid = doc["ssid"];
      const char* password = doc["password"];
      connect_wifi(ssid, password);
    } else if (command == "confirm_home") {
      confirmHome();
    } else if (command == "clear_memory") {
      prefs.clear();
      Serial.println("🔄 Preferences cleared.");
    } else if (command == "shut_down") {
      shut_down();
    } else if (command == "restart") {
      restart();
    } else if (command == "connect_saved_wifi") {
      connectToSavedWiFi();
    } else if (command == "potential_zone_left"){
      potential_zone_left();
    }

    // Step 3: User task done → let Zone_monitor run again
    pauseMonitor = false;

    // Step 4: Release Wi-Fi access
    xSemaphoreGive(wifiMutex);
  }
}

void debugPrint(const String& msg) {
  Serial.println(msg);
  SerialBT.println(msg);
}

