//********************************** Render server ***********

void send_message_Render(String command,String message){
    // Make HTTPS GET request
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;

    // Disable SSL certificate verification (less secure, good for testing)
    // client.setInsecure();

    http.begin(client, serverURL); // Initialize HTTP with secure client

    Serial.println(String("Message: ") + message);
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.printf("Free heap before HTTPS request: %u bytes\n", ESP.getFreeHeap()); // Should return more than 20000 bytes
    
    http.addHeader("x-api-key", x_apiKey);
    http.addHeader("my-api-key", my_apiKey);

        // Create JSON string
    String jsonPayload = "{\"command\":\"" + command + "\", \"message\":\"" + message + "\"}";

    int httpResponseCode = http.GET(jsonPayload); // Send GET request

    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println("Response:");
      Serial.println(payload);
    } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Close connection
  } else {
    Serial.println("WiFi not connected!");
  }
}


