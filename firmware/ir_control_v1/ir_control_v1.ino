#include <Arduino.h>
#include <Wire.h>

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
constexpr uint16_t kTxRepeats = 2;  // Three copies total: original + 2 repeats.

struct Preset {
  const char* line1;
  const char* line2;
  const uint8_t* state;
};

// Each frame was captured from the original Sharp remote and checksum-validated.
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
    {"COOL 19C", "FAN: REMOTE STATE", kCool19},
    {"COOL 20C", "FAN: REMOTE STATE", kCool20},
    {"COOL 20C", "FAN: AUTO", kCool20FanAuto},
    {"COOL 20C", "FAN: HIGH", kCool20FanHigh},
};
constexpr uint8_t kPresetCount = sizeof(kPresets) / sizeof(kPresets[0]);

IRrecv receiver(kIrReceivePin, 1024, 50, true);
IRsend transmitter(kIrTransmitPin);
decode_results result;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);

uint8_t selected = 0;
bool buttonDown = false;
uint32_t buttonDownAt = 0;
uint32_t transmissions = 0;
uint32_t received = 0;

void drawMenu(const char* status = "TAP:NEXT HOLD:SEND") {
  char heading[24];
  snprintf(heading, sizeof(heading), "AC %u/%u", selected + 1, kPresetCount);

  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tf);
  oled.drawStr(0, 12, heading);
  oled.drawHLine(0, 15, 128);
  oled.drawStr(0, 30, kPresets[selected].line1);
  oled.drawStr(0, 43, kPresets[selected].line2);
  oled.drawStr(0, 60, status);
  oled.sendBuffer();
}

void sendSelected() {
  const Preset& preset = kPresets[selected];
  drawMenu("SENDING...");
  transmitter.sendSharpAc(preset.state, sizeof(kCool19), kTxRepeats);
  transmissions++;
  Serial.printf("TX_SENT id=%lu preset=%u\n", transmissions, selected + 1);
  delay(250);
  drawMenu("SENT: CHECK AC");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(kBootButtonPin, INPUT_PULLUP);

  Wire.begin(kI2cSdaPin, kI2cSclPin);
  oled.begin();
  drawMenu();

  transmitter.begin();
  receiver.setUnknownThreshold(12);
  receiver.setTolerance(35);
  receiver.enableIRIn();

  Serial.println("IR_CONTROL_V1_READY");
  Serial.println("Tap BOOT to select; hold BOOT to send selected preset.");
}

void loop() {
  const bool buttonNow = digitalRead(kBootButtonPin) == LOW;
  if (!buttonDown && buttonNow) {
    buttonDown = true;
    buttonDownAt = millis();
  } else if (buttonDown && !buttonNow) {
    const uint32_t pressedFor = millis() - buttonDownAt;
    buttonDown = false;
    if (pressedFor >= kLongPressMs) {
      sendSelected();
    } else {
      selected = (selected + 1) % kPresetCount;
      drawMenu();
      Serial.printf("PRESET_SELECTED index=%u\n", selected + 1);
    }
  }

  if (receiver.decode(&result) && !result.overflow && result.decode_type != UNKNOWN) {
    received++;
    Serial.printf("IR_VALID id=%lu protocol_id=%u bits=%u\n", received,
                  static_cast<uint16_t>(result.decode_type), result.bits);
  }
  delay(10);
}
