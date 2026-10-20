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

// Captured from the original Sharp remote: Power On, Cool, 19C.
const uint8_t kSharpCool19[] = {
    0xAA, 0x5A, 0xCF, 0x10, 0xC4, 0x31, 0x52,
    0x00, 0x08, 0x80, 0x04, 0xE0, 0xB1,
};

IRrecv receiver(kIrReceivePin, 1024, 50, true);
IRsend transmitter(kIrTransmitPin);
decode_results result;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);

bool previousButton = HIGH;
uint32_t transmissions = 0;
uint32_t validCaptures = 0;
uint32_t lastStatus = 0;

void drawScreen(const char* message) {
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tf);
  oled.drawStr(0, 12, "SMART HOME IR");
  oled.drawHLine(0, 15, 128);
  oled.drawStr(0, 30, "TX: GPIO26  RX: GPIO27");
  oled.drawStr(0, 43, message);
  oled.drawStr(0, 56, "BOOT = COOL 19C ON");
  oled.sendBuffer();
}

void sendCool19() {
  drawScreen("SENDING COOL 19C...");
  // Send the exact 104-bit remote frame three times to improve reliability.
  transmitter.sendSharpAc(kSharpCool19, sizeof(kSharpCool19), 2);
  transmissions++;
  Serial.printf("TX_SENT id=%lu state=COOL_19C_ON bytes=%u\n", transmissions,
                sizeof(kSharpCool19));
  delay(250);
  drawScreen("SENT: CHECK AC");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(kBootButtonPin, INPUT_PULLUP);

  Wire.begin(kI2cSdaPin, kI2cSclPin);
  oled.begin();
  drawScreen("READY");

  transmitter.begin();
  receiver.setUnknownThreshold(12);
  receiver.setTolerance(35);
  receiver.enableIRIn();

  Serial.println("IR_TX_BUTTON_TEST_READY");
  Serial.println("Press BOOT once to send: Power On, Cool, 19C.");
}

void loop() {
  const bool buttonNow = digitalRead(kBootButtonPin);
  if (previousButton == HIGH && buttonNow == LOW) sendCool19();
  previousButton = buttonNow;

  if (receiver.decode(&result) && !result.overflow && result.decode_type != UNKNOWN) {
    validCaptures++;
    Serial.printf("IR_VALID id=%lu protocol_id=%u bits=%u\n", validCaptures,
                  static_cast<uint16_t>(result.decode_type), result.bits);
  }

  if (millis() - lastStatus >= 10000) {
    lastStatus = millis();
    Serial.printf("IR_TX_READY sent=%lu received=%lu\n", transmissions,
                  validCaptures);
  }
  delay(10);
}
