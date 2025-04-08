#define BLYNK_TEMPLATE_ID "TMPL6fdFKJXou"
#define BLYNK_TEMPLATE_NAME "WIFI DEV"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

const char* targetSSID = "ESP8266_AP";

char auth[] = "MZoNSQwNYJkUJxynBSY7ZZ8cKJtCgRJh";     // Thay bằng Auth Token từ Blynk của bạn
char ssid[] = "Anytime Coffee";        // Thay bằng tên WiFi của bạn
char pass[] = "Camonquykhach";


const int Red = D1;
const int Green = D2;
const int Light = D3;
const int btn = D5;

#define RSSI_THRESHOLD -45
#define BUFFER_SIZE 5
#define BLINK_INTERVAL 500    // nháy Red mỗi 500ms


long rssiBuffer[BUFFER_SIZE] = {0};
int bufferIndex = 0;
bool offSystem = false;

// Khai báo các biến lưu trạng thái trước của các đèn
bool redVirtualState = false;
bool greenVirtualState = false;
bool lightVirtualState = false;

unsigned long lastBlinkTime = 0;
bool redBlinkState = false;

// Hàm kiểm tra và đồng bộ trạng thái LED lên Blynk
void syncLEDState(uint8_t ledPin, uint8_t virtualPin, bool &prevState) {
  // Đọc trạng thái hiện tại của đèn (HIGH hoặc LOW)
  bool currState = digitalRead(ledPin);
  
  // Nếu trạng thái thay đổi so với lần cập nhật trước
  if (currState != prevState) {
    prevState = currState;        // Cập nhật lại trạng thái cũ
    Blynk.virtualWrite(virtualPin, currState);  // Đồng bộ trạng thái lên Blynk
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

    // Đảm bảo gọi Blynk.run() để xử lý các giao tiếp của Blynk
  Blynk.run();
    // 1. Kiểm tra trạng thái nút D5
    offSystem = (digitalRead(btn) == LOW);

    // 2. Nếu offSystem thì chỉ tắt Light, không đụng vào Red & Green
    if (offSystem) {
        digitalWrite(Light, LOW);
    }

    // 3. Quét Wi-Fi
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
                // Tín hiệu mạnh
                digitalWrite(Red, LOW);
                digitalWrite(Green, HIGH);
                if (!offSystem) {
                    digitalWrite(Light, HIGH);
                }
            } else {
                // Tín hiệu yếu
                digitalWrite(Red, HIGH);
                digitalWrite(Green, LOW);
                digitalWrite(Light, LOW);
            }

            found = true;
            break;
        }
    }

    if (!found) {
        // Không tìm thấy AP -> nhấp nháy Red, Green/Light OFF
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

    // Nếu cần đồng bộ trạng thái gửi lên (ví dụ: Blynk hoặc khác), xử lý ở đây
    // if (signal1 != signal2) {
    //     signal2 = signal1;
    //     // Gửi trạng thái qua mạng
    // }
}
