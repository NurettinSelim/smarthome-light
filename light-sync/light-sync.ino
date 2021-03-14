#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>

// Set these to run.
#define FIREBASE_HOST "project-id.firebaseio.com"
#define FIREBASE_AUTH "auth"
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "password"

#define LED_PIN D3

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.stream("/");
  
  if (Firebase.failed()) {
    Serial.println("error");
    Serial.println(Firebase.error());
    ESP.reset();
  }
  
  digitalWrite(LED_PIN, Firebase.getBool("light/OnOff/on"));
  
}

void loop() {
  if (Firebase.failed()) {
    Serial.println("streaming error");
    Serial.println(Firebase.error());
    ESP.reset();
  }
  
  if (Firebase.available()) {
    FirebaseObject event = Firebase.readEvent();
    String eventType = event.getString("type");
    eventType.toLowerCase();
    Serial.print("event: ");
    Serial.println(eventType);
    if (eventType == "patch") {
      String path = event.getString("path");
      Serial.print("Path:");
      Serial.println(path);
      
      if(path.startsWith("/light")){
        bool on_off = event.getBool("data/on");
        Serial.print("OnOff: ");
        Serial.println(on_off);
        
        digitalWrite(LED_PIN, on_off);
      }
    }
    Serial.println("------------------");
  }   
}
