//********************************** Render server ***********

// APN for SIM card
const char* apn = "dialogbb";  // e.g., "dialogbb" or "mobitel3g"

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
    http.addHeader("Content-Type", "application/json");

        // Create JSON string
    String jsonPayload = 
      String("{\"x-api-key\":\"") + x_apiKey +
      "\", \"my-api-key\":\"" + my_apiKey +
      "\", \"command\":\"" + command +
      "\", \"message\":\"" + message + "\"}";

    int httpResponseCode = http.POST(jsonPayload); // Send GET request

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
    Serial.println("WiFi not connected. Using GSM...");
    useGSMToSend(command, message);
  }
}

void sendAT(String cmd) {
  gsm.println(cmd);
  delay(1000);
  while (gsm.available()) {
    Serial.write(gsm.read());
  }
}

void useGSMToSend(String command, String message) {
  String jsonPayload = 
    String("{\"x-api-key\":\"") + x_apiKey +
    "\", \"my-api-key\":\"" + my_apiKey +
    "\", \"command\":\"" + command +
    "\", \"message\":\"" + message + "\"}";

  sendAT("AT");
  sendAT("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
  sendAT("AT+SAPBR=3,1,\"APN\",\"" + String(apn) + "\"");
  sendAT("AT+SAPBR=1,1");
  delay(2000);
  sendAT("AT+HTTPINIT");
  sendAT("AT+HTTPPARA=\"CID\",1");
  sendAT(String("AT+HTTPPARA=\"URL\",\"") + serverURL + "\"");
  sendAT("AT+HTTPPARA=\"CONTENT\",\"application/json\"");

  sendAT("AT+HTTPDATA=" + String(jsonPayload.length()) + ",10000");
  delay(1000);
  gsm.print(jsonPayload);
  delay(2000);

  sendAT("AT+HTTPACTION=1");
  delay(6000);
  sendAT("AT+HTTPREAD");
  sendAT("AT+HTTPTERM");
  sendAT("AT+SAPBR=0,1");
}


