#include <Arduino.h>
#include <Preferences.h>
#include <PubSubClient.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include <Wire.h>

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>

#include "secrets.h"

constexpr uint16_t kIrReceivePin = 27;
constexpr uint16_t kIrTransmitPin = 26;
constexpr uint8_t kI2cSdaPin = 21;
constexpr uint8_t kI2cSclPin = 22;
constexpr uint8_t kBootButtonPin = 0;
constexpr uint32_t kLongPressMs = 700;
constexpr uint16_t kTxRepeats = 2;
constexpr char kCommandTopic[] = "smarthome/ac-sharp-1/command";
constexpr char kStateTopic[] = "smarthome/ac-sharp-1/state";

struct Preset { const char* id; const char* title; const uint8_t* state; };

const uint8_t kCool19[] = {0xAA, 0x5A, 0xCF, 0x10, 0xC4, 0x31, 0x52, 0x00, 0x08, 0x80, 0x04, 0xE0, 0xB1};
const uint8_t kCool20[] = {0xAA, 0x5A, 0xCF, 0x10, 0xC5, 0x31, 0x52, 0x00, 0x08, 0x80, 0x04, 0xE0, 0xA1};
const uint8_t kCool20FanAuto[] = {0xAA, 0x5A, 0xCF, 0x10, 0xC5, 0x31, 0x22, 0x00, 0x08, 0x80, 0x05, 0xE0, 0xC1};
const uint8_t kCool20FanHigh[] = {0xAA, 0x5A, 0xCF, 0x10, 0xC5, 0x31, 0x72, 0x00, 0x08, 0x80, 0x05, 0xE0, 0x91};
const Preset kPresets[] = {
  {"cool_19", "Cool 19C", kCool19}, {"cool_20", "Cool 20C", kCool20},
  {"cool_20_fan_auto", "Cool 20C Auto", kCool20FanAuto},
  {"cool_20_fan_high", "Cool 20C High", kCool20FanHigh},
};
constexpr uint8_t kPresetCount = sizeof(kPresets) / sizeof(kPresets[0]);

IRrecv receiver(kIrReceivePin, 1024, 50, true);
IRsend transmitter(kIrTransmitPin);
decode_results result;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

uint8_t selected = 0;
bool buttonDown = false;
uint32_t buttonDownAt = 0;
uint32_t lastReconnectAttempt = 0;
String lastPreset = "";

void draw(const char* line1, const char* line2, const char* line3, const char* line4) {
  oled.clearBuffer(); oled.setFont(u8g2_font_6x12_tf);
  oled.drawStr(0, 12, line1); oled.drawStr(0, 28, line2);
  oled.drawStr(0, 44, line3); oled.drawStr(0, 60, line4); oled.sendBuffer();
}

void drawControl() {
  char heading[20]; snprintf(heading, sizeof(heading), "AC %u/%u", selected + 1, kPresetCount);
  draw(heading, kPresets[selected].title, mqtt.connected() ? "MQTT CONNECTED" : "MQTT WAITING", "TAP:NEXT HOLD:SEND");
}

void publishState() {
  if (!mqtt.connected()) return;
  String state = "{\"online\":true,\"last_preset\":\"" + lastPreset + "\"}";
  mqtt.publish(kStateTopic, state.c_str(), true);
}

void sendPreset(uint8_t index) {
  if (index >= kPresetCount) return;
  selected = index;
  draw("SMART HOME IR", "SENDING...", kPresets[selected].title, "");
  transmitter.sendSharpAc(kPresets[selected].state, sizeof(kCool19), kTxRepeats);
  lastPreset = kPresets[selected].id;
  publishState();
  Serial.printf("TX_SENT preset=%s\n", lastPreset.c_str());
  delay(250); drawControl();
}

int presetIndex(const String& id) {
  for (uint8_t i = 0; i < kPresetCount; i++) if (id == kPresets[i].id) return i;
  return -1;
}

void onMqttMessage(char* topic, uint8_t* payload, unsigned int length) {
  if (String(topic) != kCommandTopic) return;
  String body;
  for (unsigned int i = 0; i < length; i++) body += static_cast<char>(payload[i]);
  const String marker = "\"preset\":\"";
  const int start = body.indexOf(marker);
  if (start < 0) return;
  const int valueStart = start + marker.length();
  const int valueEnd = body.indexOf('"', valueStart);
  if (valueEnd < 0) return;
  const int index = presetIndex(body.substring(valueStart, valueEnd));
  if (index >= 0) sendPreset(index);
}

void connectMqtt() {
  if (mqtt.connected() || WiFi.status() != WL_CONNECTED) return;
  if (!mqtt.connect("esp32-sharp-ac-1", kMqttUser, kMqttPassword)) return;
  mqtt.subscribe(kCommandTopic, 1);
  publishState();
  drawControl();
  Serial.println("MQTT_CONNECTED");
}

void connectWifi() {
  Preferences preferences; preferences.begin("wifi", true);
  const String ssid = preferences.getString("ssid", "");
  const String password = preferences.getString("password", ""); preferences.end();
  if (ssid.isEmpty()) { draw("WIFI SETUP NEEDED", "Flash web firmware", "to configure WiFi", ""); return; }
  draw("WIFI CONNECTING", ssid.c_str(), "Please wait...", "");
  WiFi.mode(WIFI_STA); WiFi.begin(ssid.c_str(), password.c_str());
  const uint32_t started = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - started < 15000) delay(250);
  if (WiFi.status() == WL_CONNECTED) {
    const String ip = WiFi.localIP().toString(); draw("WIFI CONNECTED", ip.c_str(), "MQTT CONNECTING", "");
    Serial.printf("WIFI_CONNECTED ip=%s\n", ip.c_str());
  } else draw("WIFI FAILED", "Flash web firmware", "to reconfigure", "");
}

void setup() {
  Serial.begin(115200); delay(500); pinMode(kBootButtonPin, INPUT_PULLUP);
  Wire.begin(kI2cSdaPin, kI2cSclPin); oled.begin(); transmitter.begin();
  receiver.setUnknownThreshold(12); receiver.setTolerance(35); receiver.enableIRIn();
  mqtt.setServer(kMqttHost, kMqttPort); mqtt.setCallback(onMqttMessage);
  connectWifi(); connectMqtt(); drawControl();
}

void loop() {
  if (!mqtt.connected() && millis() - lastReconnectAttempt > 5000) { lastReconnectAttempt = millis(); connectMqtt(); }
  mqtt.loop();
  const bool buttonNow = digitalRead(kBootButtonPin) == LOW;
  if (!buttonDown && buttonNow) { buttonDown = true; buttonDownAt = millis(); }
  else if (buttonDown && !buttonNow) {
    const uint32_t held = millis() - buttonDownAt; buttonDown = false;
    if (held >= kLongPressMs) sendPreset(selected); else { selected = (selected + 1) % kPresetCount; drawControl(); }
  }
  if (receiver.decode(&result) && !result.overflow && result.decode_type != UNKNOWN) {
    Serial.printf("IR_VALID protocol_id=%u bits=%u\n", static_cast<uint16_t>(result.decode_type), result.bits);
  }
  delay(5);
}
