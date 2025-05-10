// check pation is near home using current and home gps locations
// Haversine formula to calculate distance between two points in meters
double calculateDistanceMeters(GPSLocation loc1, GPSLocation loc2) {
  const double R = 6371000; // Earth radius in meters
  double lat1Rad = radians(loc1.latitude);
  double lat2Rad = radians(loc2.latitude);
  double deltaLat = radians(loc2.latitude - loc1.latitude);
  double deltaLon = radians(loc2.longitude - loc1.longitude);

  double a = sin(deltaLat / 2) * sin(deltaLat / 2) +
             cos(lat1Rad) * cos(lat2Rad) *
             sin(deltaLon / 2) * sin(deltaLon / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));

  return R * c;
}

// Called if GPS fix is not obtained in 5 minutes
void handleNoGPSFix() {
  Serial.println("GPS fix not found within 5 minutes. Exiting...");
  isUser_atHome = false;
  send_message_Render("potential_zone_left"); // ⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️
  // You can add your own custom handling code here (e.g., restart device, alert user, etc.)
}

// Function to check if current location is near home
bool isNearHome(GPSLocation current, GPSLocation home, double thresholdMeters = 100.0) {
  double distance = calculateDistanceMeters(current, home);
  Serial.print("Distance from home: ");
  Serial.print(distance);
  Serial.println(" meters");
  return distance <= thresholdMeters;
}

void potential_zone_left(){
  GPSLocation current_location = getGPSLocation(); 
  GPSLocation home_location = getHomeLocation();
  if (current_location.latitude == 0.0 && current_location.longitude == 0.0) { // current_location = {};
    handleNoGPSFix();
    return;
  }
 
  if (isNearHome(current_location, home_location)) {
    Serial.println("User is near home.");
  } else {
    Serial.println("User is away from home.");
    isUser_atHome = false;
    send_message_Render("absolute_house_left"); // ⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️
  }
}

// **** tracking function ****📍🗺️📍🗺️📍🗺️📍🗺️📍🗺️
void trackUser(){
  GPSLocation current_location = getGPSLocation(); 
  GPSLocation home_location = getHomeLocation();
  if (current_location.latitude == 0.0 && current_location.longitude == 0.0) { // current_location = {};
    Serial.println("Unable to access the location !!");
    return;
  }
 
  if (isNearHome(current_location, home_location)) {
    Serial.println("User has came to home.");
    isUser_atHome = true;
  } else {
    Serial.println("User is away from home.");
    send_data_Redis(current_location);   // send GPS location to redis
  }
}