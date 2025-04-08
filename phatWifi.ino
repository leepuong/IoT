#include <ESP8266WiFi.h>

const char *ssid = "ESP8266_AP";  // Tên Wi-Fi ESP8266 phát ra
const char *password = "12345678";  // Mật khẩu (tối thiểu 8 ký tự)
#define LED_PIN 2
void setup() {
    Serial.begin(115200);
    WiFi.softAP(ssid, password);  // Bật Access Point
    Serial.println("ESP8266 AP Mode Started!");
    Serial.print("SSID: "); Serial.println(ssid);
    Serial.print("IP Address: "); Serial.println(WiFi.softAPIP());
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    digitalWrite(LED_BUILTIN, LOW);  // Turn LED ON
    delay(1000);                     // Wait 1 second
    digitalWrite(LED_BUILTIN, HIGH); // Turn LED OFF
    delay(10); 
}
