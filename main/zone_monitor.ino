//********************************************************* zone monitor ***********************
bool isKnownBSSID(const String& bssidToCheck, const std::vector<String>& knownList) {
  for (const auto& bssid : knownList) {
    if (bssid == bssidToCheck) return true;
  }
  return false;
}

std::vector<String> getKnownBSSIDList() {
  String list = prefs.getString("known_bssids", "");
  std::vector<String> bssids;
  int start = 0;
  while (start < list.length()) {
    int commaIndex = list.indexOf(',', start);
    if (commaIndex == -1) commaIndex = list.length();
    bssids.push_back(list.substring(start, commaIndex));
    start = commaIndex + 1;
  }
  return bssids;
}

void addBSSID(const String& newBssid, const std::vector<String>& knownBSSIDs) {
  String list = prefs.getString("known_bssids", "");
  if (knownBSSIDs.size() < 6 && !isKnownBSSID(newBssid,knownBSSIDs)) {
    list += (list.length() > 0 ? "," : "") + newBssid;
    prefs.putString("known_bssids", list);
    Serial.println("🧠 Learned and added: " + newBssid);
  }
}

void monitorZone() {
  if (WiFi.status() == WL_CONNECTED){
    Serial.printf("✅ SAFE: Currently connected to home wifi.\n");
    return;
  }
  if (WiFi.status() == WL_NO_SHIELD) {
    WiFi.begin();
  }
  Serial.println("WiFi mode bfore scanning: " + String(WiFi.getMode()));  // Should be 1 (WIFI_STA)
  Serial.println("Scanning WiFi...");
  int n = WiFi.scanNetworks();

  if (n < 0) {
    Serial.println("❌ Wi-Fi scan failed (returned -1).");
    return;
  } else if (n == 0) {
    Serial.println("No networks found.");
    return;
  }

  auto knownBSSIDs = getKnownBSSIDList();  // fetched once

  String last_connected_bssid = prefs.getString("bssid","-1");
  int knownCount = 0;
  String newBSSID = "";
  for (int i = 0; i < n; ++i) {
    String bssid = WiFi.BSSIDstr(i);
    if (bssid==last_connected_bssid){
      connectToSavedWiFi();
    } else if (isKnownBSSID(bssid, knownBSSIDs)) {
      knownCount++;
    } else {
      newBSSID = bssid;
    }
  }

  if (knownCount == 0) {
    Serial.println("🚨 OUT OF ZONE: No known BSSIDs.");
    potential_zone_left();
  } else { 
    Serial.printf("✅ SAFE: %d known BSSIDs found.\n", knownCount);
    if (knownCount >= 2 && newBSSID.length() > 0) {
      addBSSID(newBSSID, knownBSSIDs);
    }
  }

  Serial.println();
}
