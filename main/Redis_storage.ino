// Upstash details

void send_data_Redis(const GPSLocation& location) {
  // Connect to Wi-Fi
  if (WiFi.status() != WL_CONNECTED) { // TODO: Check weather connected to Sim800L module
    Serial.println("Haven't connected to WiFi");
    return;
  }

  String coordinate = "(" + String(location.latitude,6) + "," + String(location.longitude,6) + ")";

  Serial.println("Sending GPS location data to Redis...");
  
  // Send data to Upstash
  HTTPClient http;
  http.begin(String(upstash_url) + "/lpush/location/"+coordinate);
  http.addHeader("Authorization", auth_token);
  // http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(""); // No body needed for simple commands
  String response = http.getString();

  Serial.println("Response Code: " + String(httpResponseCode));
  Serial.println("Response: " + response);

  http.end();

}

