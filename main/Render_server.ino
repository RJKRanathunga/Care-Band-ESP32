//********************************** Render server ***********

void send_message_Render(String message){
    // Make HTTPS GET request
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClientSecure client;

    // Disable SSL certificate verification (less secure, good for testing)
    client.setInsecure();

    http.begin(client, serverName); // Initialize HTTP with secure client

    Serial.println(String("Message: ") + message);
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.printf("Free heap before HTTPS request: %u bytes\n", ESP.getFreeHeap()); // Should return more than 20000 bytes
    
    http.addHeader("x-api-key", apiKey);
    http.addHeader("message", message);

    int httpCode = http.GET(); // Send GET request

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Response:");
      Serial.println(payload);
    } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpCode);
    }

    http.end(); // Close connection
  } else {
    Serial.println("WiFi not connected!");
  }
}


