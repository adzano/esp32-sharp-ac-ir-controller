#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <U8g2lib.h>

constexpr uint16_t kIrReceivePin = 27;
constexpr uint16_t kIrTransmitPin = 26;
constexpr uint8_t kI2cSdaPin = 21;
constexpr uint8_t kI2cSclPin = 22;
constexpr uint8_t kBootButtonPin = 0;
constexpr uint32_t kLongPressMs = 700;
constexpr uint16_t kTxRepeats = 2;
constexpr char kSetupSsid[] = "Smarthome-Setup";
constexpr char kSetupPassword[] = "smarthome";

struct Preset {
  const char* title;
  const char* detail;
  const uint8_t* state;
};

const uint8_t kCool19[] = {
    0xAA, 0x5A, 0xCF, 0x10, 0xC4, 0x31, 0x52,
    0x00, 0x08, 0x80, 0x04, 0xE0, 0xB1,
};
const uint8_t kCool20[] = {
    0xAA, 0x5A, 0xCF, 0x10, 0xC5, 0x31, 0x52,
    0x00, 0x08, 0x80, 0x04, 0xE0, 0xA1,
};
const uint8_t kCool20FanAuto[] = {
    0xAA, 0x5A, 0xCF, 0x10, 0xC5, 0x31, 0x22,
    0x00, 0x08, 0x80, 0x05, 0xE0, 0xC1,
};
const uint8_t kCool20FanHigh[] = {
    0xAA, 0x5A, 0xCF, 0x10, 0xC5, 0x31, 0x72,
    0x00, 0x08, 0x80, 0x05, 0xE0, 0x91,
};

const Preset kPresets[] = {
    {"Cool 19C", "Remote fan state", kCool19},
    {"Cool 20C", "Remote fan state", kCool20},
    {"Cool 20C", "Fan auto", kCool20FanAuto},
    {"Cool 20C", "Fan high", kCool20FanHigh},
};
constexpr uint8_t kPresetCount = sizeof(kPresets) / sizeof(kPresets[0]);

IRrecv receiver(kIrReceivePin, 1024, 50, true);
IRsend transmitter(kIrTransmitPin);
decode_results result;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);
WebServer server(80);
Preferences preferences;

uint8_t selected = 0;
bool buttonDown = false;
uint32_t buttonDownAt = 0;
uint32_t transmissions = 0;
uint32_t received = 0;
bool setupMode = false;
bool showingNetwork = false;
String lastAction = "Ready";

void drawLines(const char* line1, const char* line2, const char* line3,
               const char* line4) {
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tf);
  oled.drawStr(0, 12, line1);
  oled.drawStr(0, 28, line2);
  oled.drawStr(0, 44, line3);
  oled.drawStr(0, 60, line4);
  oled.sendBuffer();
}

void drawControl() {
  char heading[24];
  snprintf(heading, sizeof(heading), "AC %u/%u", selected + 1, kPresetCount);
  drawLines(heading, kPresets[selected].title, kPresets[selected].detail,
            "TAP:NEXT HOLD:SEND");
}

void sendPreset(uint8_t index) {
  if (index >= kPresetCount) return;
  selected = index;
  const Preset& preset = kPresets[selected];
  drawLines("SMART HOME IR", "SENDING...", preset.title, preset.detail);
  transmitter.sendSharpAc(preset.state, sizeof(kCool19), kTxRepeats);
  transmissions++;
  lastAction = String("Sent ") + preset.title + " / " + preset.detail;
  Serial.printf("TX_SENT id=%lu preset=%u\n", transmissions, selected + 1);
  delay(250);
  drawControl();
}

String controlPage() {
  String page = F("<!doctype html><html><meta name=viewport content='width=device-width,initial-scale=1'>"
                  "<title>Smart Home IR</title><style>body{font-family:system-ui;margin:24px;max-width:560px}"
                  "button{display:block;width:100%;padding:16px;margin:12px 0;font-size:18px;text-align:left}"
                  "small{color:#555}</style><h1>Smart Home IR</h1><p>");
  page += WiFi.localIP().toString();
  page += F("</p><p id=status>");
  page += lastAction;
  page += F("</p>");
  for (uint8_t i = 0; i < kPresetCount; i++) {
    page += F("<button onclick='sendPreset(");
    page += String(i + 1);
    page += F(")'><b>");
    page += kPresets[i].title;
    page += F("</b><br><small>");
    page += kPresets[i].detail;
    page += F("</small></button>");
  }
  page += F("<script>async function sendPreset(n){let r=await fetch('/api/send?preset='+n,{method:'POST'});"
            "document.querySelector('#status').textContent=(await r.json()).message}</script></html>");
  return page;
}

String setupPage() {
  return F("<!doctype html><html><meta name=viewport content='width=device-width,initial-scale=1'>"
           "<title>Smart Home Setup</title><style>body{font-family:system-ui;margin:24px;max-width:480px}"
           "input,button{box-sizing:border-box;width:100%;padding:14px;margin:10px 0;font-size:16px}</style>"
           "<h1>Hubungkan Wi-Fi</h1><p>Masukkan Wi-Fi rumah 2,4 GHz untuk ESP32.</p>"
           "<form method=post action='/setup'><input name=ssid placeholder='Nama Wi-Fi' required>"
           "<input name=password type=password placeholder='Password Wi-Fi' required>"
           "<button>Simpan dan hubungkan</button></form></html>");
}

void handleRoot() { server.send(200, "text/html", setupMode ? setupPage() : controlPage()); }

void handleSend() {
  if (setupMode || !server.hasArg("preset")) {
    server.send(400, "application/json", "{\"message\":\"Invalid request\"}");
    return;
  }
  const int requested = server.arg("preset").toInt();
  if (requested < 1 || requested > kPresetCount) {
    server.send(400, "application/json", "{\"message\":\"Unknown preset\"}");
    return;
  }
  sendPreset(requested - 1);
  server.send(200, "application/json", String("{\"message\":\"") + lastAction + "\"}");
}

void handleSetup() {
  const String ssid = server.arg("ssid");
  const String password = server.arg("password");
  if (ssid.isEmpty() || password.isEmpty()) {
    server.send(400, "text/plain", "Nama dan password Wi-Fi diperlukan.");
    return;
  }
  preferences.begin("wifi", false);
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.end();
  server.send(200, "text/html", "Tersimpan. ESP32 akan restart dan mencoba Wi-Fi rumah.");
  delay(800);
  ESP.restart();
}

void startSetupNetwork() {
  setupMode = true;
  WiFi.mode(WIFI_AP);
  WiFi.softAP(kSetupSsid, kSetupPassword);
  drawLines("WIFI SETUP", "SSID: Smarthome-Setup", "PASS: smarthome", "OPEN: 192.168.4.1");
  Serial.println("WIFI_SETUP_AP ssid=Smarthome-Setup ip=192.168.4.1");
}

void connectNetwork() {
  preferences.begin("wifi", true);
  const String ssid = preferences.getString("ssid", "");
  const String password = preferences.getString("password", "");
  preferences.end();
  if (ssid.isEmpty()) {
    startSetupNetwork();
    return;
  }

  drawLines("WIFI CONNECTING", ssid.c_str(), "Please wait...", "");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  const uint32_t started = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - started < 15000) delay(250);
  if (WiFi.status() == WL_CONNECTED) {
    setupMode = false;
    showingNetwork = true;
    const String ip = WiFi.localIP().toString();
    drawLines("WIFI CONNECTED", ssid.c_str(), "OPEN THIS IP:", ip.c_str());
    Serial.printf("WIFI_CONNECTED ssid=%s ip=%s\n", ssid.c_str(), ip.c_str());
  } else {
    WiFi.disconnect(false);
    startSetupNetwork();
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(kBootButtonPin, INPUT_PULLUP);
  Wire.begin(kI2cSdaPin, kI2cSclPin);
  oled.begin();
  transmitter.begin();
  receiver.setUnknownThreshold(12);
  receiver.setTolerance(35);
  receiver.enableIRIn();

  connectNetwork();
  server.on("/", HTTP_GET, handleRoot);
  server.on("/setup", HTTP_POST, handleSetup);
  server.on("/api/send", HTTP_POST, handleSend);
  server.begin();
  Serial.println("IR_WEB_CONTROL_V1_READY");
}

void loop() {
  server.handleClient();

  const bool buttonNow = digitalRead(kBootButtonPin) == LOW;
  if (!buttonDown && buttonNow) {
    buttonDown = true;
    buttonDownAt = millis();
  } else if (buttonDown && !buttonNow) {
    const uint32_t heldFor = millis() - buttonDownAt;
    buttonDown = false;
    if (!setupMode) {
      if (heldFor >= kLongPressMs) sendPreset(selected);
      else {
        selected = (selected + 1) % kPresetCount;
        showingNetwork = false;
        drawControl();
      }
    }
  }

  if (receiver.decode(&result) && !result.overflow && result.decode_type != UNKNOWN) {
    received++;
    Serial.printf("IR_VALID id=%lu protocol_id=%u bits=%u\n", received,
                  static_cast<uint16_t>(result.decode_type), result.bits);
  }
  delay(5);
}
