#include <ESP8266WiFi.h>

const char* ssid = "MyESP8266";
const char* password = "12345678";

const int buttonPin = D5;     
const int ledPin = D2;        

bool wifiActive = false;
unsigned long wifiStartTime = 0;
const unsigned long wifiDuration = 40000; 
const unsigned long blinkInterval = 150;

unsigned long previousBlinkMillis = 0;
bool ledState = false;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); 
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(115200);
}

void loop() {
  static bool lastButtonState = HIGH;
  bool currentButtonState = digitalRead(buttonPin);

  
  if (lastButtonState == HIGH && currentButtonState == LOW && !wifiActive) {
    WiFi.softAP(ssid, password);
    wifiStartTime = millis();
    wifiActive = true;
    Serial.println("SoftAP started.");
  }
  lastButtonState = currentButtonState;

  if (wifiActive) {
    unsigned long currentMillis = millis();

    
    if (currentMillis - wifiStartTime < 3000) {
      if (currentMillis - previousBlinkMillis >= blinkInterval) {
        previousBlinkMillis = currentMillis;
        ledState = !ledState;
        digitalWrite(ledPin, ledState);
      }
    }
    
    else if (currentMillis - wifiStartTime < wifiDuration) {
      digitalWrite(ledPin, HIGH);
    }
    
    else {
      WiFi.softAPdisconnect(true);
      digitalWrite(ledPin, LOW);
      wifiActive = false;
      Serial.println("SoftAP stopped.");
    }
  }
}
