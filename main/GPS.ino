// ********************************* GPS functions ***********
// This function returns the location when found for initial setup
GPSLocation getGPSLocation() {
  unsigned long startTime = millis();

  Serial.println("Start finding GPS location...");

  while (millis() - startTime < GPS_TIMEOUT) {
    while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
      if (gps.location.isUpdated()) {
        GPSLocation loc;
        loc.latitude = gps.location.lat();
        loc.longitude = gps.location.lng();
        return loc;
      }
    }
  }

  return {};  // Never reached if handleNoGPSFix halts; added for completeness
}


// Function to get the saved GPS location using prefs
GPSLocation getHomeLocation() {
  GPSLocation loc = {0.0, 0.0};

  String locationStr = prefs.getString("Home_location", "");
  int commaIndex = locationStr.indexOf(',');
  if (commaIndex != -1) {
    String latStr = locationStr.substring(0, commaIndex);
    String lonStr = locationStr.substring(commaIndex + 1);
    loc.latitude = latStr.toDouble();
    loc.longitude = lonStr.toDouble();
  } else {
    Serial.println("Invalid saved location format");
  }

  return loc;
}