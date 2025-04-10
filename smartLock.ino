#include <ESP8266WiFi.h>
#include <Keypad.h>


const char* targetSSID = "MyESP8266";
const char* correctPassword = "2468";  


const int ledPin = D0;


bool ssidDetected = false;
bool lastSsidDetected = false;
unsigned long lastScanTime = 0;
const unsigned long scanInterval = 10000; 


const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {D1, D2, D3, D4};
byte colPins[COLS] = {D5, D6, D7, D8};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


String inputPassword = "";


void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.println("The system is up and running.");
}

void loop() {
  
  if (millis() - lastScanTime >= scanInterval) {
    lastScanTime = millis();
    ssidDetected = scanForSSID();
    
    if (ssidDetected != lastSsidDetected) {
      if (ssidDetected) {
        Serial.println("Detected SSID: " + String(targetSSID));
      } else {
        digitalWrite(ledPin, LOW);
        Serial.println("Undetected SSID: " + String(targetSSID));
      }
      lastSsidDetected = ssidDetected;
    }
  }
  
  
  char key = keypad.getKey();
  if (key) {
    if (key == '#') {  
      
      if (!ssidDetected) {
        blinkLedError();
        Serial.println("\nUse smart key before entering password");
        inputPassword = "";  
      } else {
        checkPassword();
        
        ssidDetected = false;
        lastSsidDetected = false;
        inputPassword = "";
      }
    }
    else if (key == '*') {  
      inputPassword = "";
      Serial.println("\nPassword deleted.");
    }
    else {
      inputPassword += key;
      blinkLedInput();
      Serial.print("*");
    }
  }
  
  delay(100); 
}


bool scanForSSID() {
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; i++) {
    if (WiFi.SSID(i) == String(targetSSID)) {
      return true;
    }
  }
  return false;
}


void checkPassword() {
  Serial.println();
  Serial.print("Enter password: ");
  Serial.println(inputPassword);
  
  if (inputPassword == String(correctPassword)) {
    Serial.println("✔️ Password is correct. Open the door.");
    digitalWrite(ledPin, HIGH);  
  } 
  else {
    Serial.println("❌ Password is incorrect.");
    blinkLedError();
  }
}


void blinkLedError() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
}
void blinkLedInput() {
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);
}