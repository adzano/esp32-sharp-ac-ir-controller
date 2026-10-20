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

IRrecv receiver(kIrReceivePin, 1024, 50, true);
IRsend transmitter(kIrTransmitPin);
decode_results result;

// Initial driver selection for a 0.96 inch, 128x64 OLED found at I2C 0x3C.
// If it does not render, test the SH1106 variant without changing wiring.
U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);

uint32_t validCaptures = 0;
uint32_t ignoredNoise = 0;
uint32_t lastRefresh = 0;

void drawStatus() {
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tf);
  oled.drawStr(0, 12, "SMART HOME");
  oled.drawHLine(0, 15, 128);
  oled.drawStr(0, 30, "OLED: OK  I2C: 0x3C");
  oled.drawStr(0, 43, "IR RX: GPIO27");
  oled.drawStr(0, 56, "IR TX: GPIO26 READY");
  oled.sendBuffer();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(kI2cSdaPin, kI2cSclPin);
  oled.begin();
  drawStatus();

  transmitter.begin();  // Initialise only. This sketch never transmits IR.
  receiver.setUnknownThreshold(12);
  receiver.setTolerance(35);
  receiver.enableIRIn();

  Serial.println("OLED_STATUS_READY");
  Serial.println("OLED controller=SSD1306 address=0x3C");
  Serial.println("IR transmitter initialized; no IR transmission is enabled.");
}

void loop() {
  if (receiver.decode(&result)) {
    if (result.overflow || result.decode_type == UNKNOWN) {
      ignoredNoise++;
    } else {
      validCaptures++;
      Serial.printf("IR_VALID id=%lu protocol_id=%u bits=%u\n", validCaptures,
                    static_cast<uint16_t>(result.decode_type), result.bits);
    }
  }

  if (millis() - lastRefresh >= 10000) {
    lastRefresh = millis();
    Serial.printf("OLED_STATUS waiting valid_ir=%lu ignored_noise=%lu input=%d\n",
                  validCaptures, ignoredNoise, digitalRead(kIrReceivePin));
  }
  delay(1);
}
