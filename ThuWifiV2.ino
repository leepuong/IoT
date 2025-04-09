#define BLYNK_TEMPLATE_ID "TMPL6fdFKJXou"
#define BLYNK_TEMPLATE_NAME "WIFI DEV"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

const char* targetSSID = "ESP8266_AP";

char auth[] = "MZoNSQwNYJkUJxynBSY7ZZ8cKJtCgRJh";     // Replace with Auth Token from your Blynk
char ssid[] = "Anytime Coffee";        // Replace with your WiFi name
char pass[] = "Camonquykhach";


const int Red = D1;
const int Green = D2;
const int Light = D3;
const int btn = D5;

#define RSSI_THRESHOLD -45
#define BUFFER_SIZE 5
#define BLINK_INTERVAL 500    
// Flashes Red every 500ms


long rssiBuffer[BUFFER_SIZE] = {0};
int bufferIndex = 0;
bool offSystem = false;

// Declare variables to store the previous state of the lights
bool redVirtualState = false;
bool greenVirtualState = false;
bool lightVirtualState = false;

unsigned long lastBlinkTime = 0;
bool redBlinkState = false;

// Function to check and synchronize LED status to Blynk
void syncLEDState(uint8_t ledPin, uint8_t virtualPin, bool &prevState) {
  // Read the current state of the light (HIGH or LOW)
  bool currState = digitalRead(ledPin);
  
  // If the status has changed since the last update
  if (currState != prevState) {
    prevState = currState;        // Update old status
    Blynk.virtualWrite(virtualPin, currState);  // Sync state to Blynk
  }
}


void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    pinMode(Red, OUTPUT);
    pinMode(Green, OUTPUT);
    pinMode(Light, OUTPUT);
    pinMode(btn, INPUT);

    digitalWrite(Red, HIGH);
    digitalWrite(Green, LOW);
    digitalWrite(Light, LOW);

    
  // Khởi tạo kết nối Blynk
  Blynk.begin(auth, ssid, pass);
}

bool checkWeakSignal() {
    int count = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (rssiBuffer[i] < RSSI_THRESHOLD) {
            count++;
        }
    }
    return (count >= 3);
}






void loop() {

    // Make sure to call Blynk.run() to handle Blynk communications
  Blynk.run();
    // 1. Check D5 button status
    offSystem = (digitalRead(btn) == LOW);

    // 2. If offSystem then only turn off Light, do not touch Red & Green
    if (offSystem) {
        digitalWrite(Light, LOW);
    }

    // 3. Wi-Fi Scan
    int n = WiFi.scanNetworks();
    bool found = false;

    for (int i = 0; i < n; ++i) {
        if (WiFi.SSID(i) == targetSSID) {
            long rssi = WiFi.RSSI(i);
            Serial.print("Tín hiệu từ ESP8266_AP: ");
            Serial.print(rssi);
            Serial.println(" dBm");

            rssiBuffer[bufferIndex] = rssi;
            bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;

            if (!checkWeakSignal()) {
                // Strong signal
                digitalWrite(Red, LOW);
                digitalWrite(Green, HIGH);
                if (!offSystem) {
                    digitalWrite(Light, HIGH);
                }
            } else {
                //Weak signal
                digitalWrite(Red, HIGH);
                digitalWrite(Green, LOW);
                digitalWrite(Light, LOW);
            }

            found = true;
            break;
        }
    }

    if (!found) {
        // No AP found -> flashing Red, Green/Light OFF
        digitalWrite(Green, LOW);
        digitalWrite(Light, LOW);

        unsigned long currentMillis = millis();
        if (currentMillis - lastBlinkTime >= BLINK_INTERVAL) {
            lastBlinkTime = currentMillis;
            redBlinkState = !redBlinkState;
            digitalWrite(Red, redBlinkState ? HIGH : LOW);
        }
    }
    syncLEDState(Red,  V0, redVirtualState);
  syncLEDState(Green, V1, greenVirtualState);
  syncLEDState(Light, V2, lightVirtualState);

    // If you need to sync the sent state (e.g. Blynk or other), handle it here
    // if (signal1 != signal2) {
    //     signal2 = signal1;        
    //     // Send status over network
    // }
}
