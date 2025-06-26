// Upstash details

void send_data_Redis(const GPSLocation& location) {
  // Connect to Wi-Fi
  if (WiFi.status() != WL_CONNECTED) { // TODO: Check weather connected to Sim800L module
    Serial.println("Haven't connected to WiFi");
    return;
  }

  String coordinate = "(" + String(location.latitude,6) + "," + String(location.longitude,6) + ")";

  Serial.println("Sending GPS location as JSON via send_message_Render...");
  
  // Use the existing function to send the data
  send_message_Render("save_location", coordinate);

}

