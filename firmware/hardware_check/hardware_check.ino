#include <Arduino.h>
#include <Wire.h>

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>

// Wiring uji:
// IR receiver DATA -> GPIO27
// IR transmitter DAT -> GPIO26
// OLED SDA -> GPIO21, SCL -> GPIO22
constexpr uint16_t kIrReceivePin = 27;
constexpr uint16_t kIrTransmitPin = 26;
constexpr uint8_t kI2cSdaPin = 21;
constexpr uint8_t kI2cSclPin = 22;

IRrecv receiver(kIrReceivePin, 1024, 50, true);
IRsend transmitter(kIrTransmitPin);
decode_results result;

uint32_t captures = 0;
uint32_t lastStatus = 0;

void scanI2c() {
  uint8_t devices = 0;
  Serial.println("I2C_SCAN_BEGIN");
  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.printf("I2C_DEVICE address=0x%02X\n", address);
      devices++;
    }
  }
  Serial.printf("I2C_SCAN_END devices=%u\n", devices);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(kI2cSdaPin, kI2cSclPin);
  transmitter.begin();
  receiver.setUnknownThreshold(12);
  receiver.setTolerance(35);
  receiver.enableIRIn();

  Serial.println("HARDWARE_CHECK_READY");
  Serial.printf("IR_RECEIVER gpio=%u\n", kIrReceivePin);
  Serial.printf("IR_TRANSMITTER gpio=%u status=initialized_no_transmission\n",
                kIrTransmitPin);
  Serial.printf("I2C sda=%u scl=%u\n", kI2cSdaPin, kI2cSclPin);
  scanI2c();
}

void loop() {
  if (receiver.decode(&result)) {
    Serial.printf("IR_RECEIVED id=%lu protocol_id=%u bits=%u overflow=%u\n",
                  ++captures, static_cast<uint16_t>(result.decode_type),
                  result.bits, result.overflow);
  }

  if (millis() - lastStatus >= 10000) {
    lastStatus = millis();
    Serial.printf("HARDWARE_CHECK_WAITING captures=%lu receiver_input=%d\n",
                  captures, digitalRead(kIrReceivePin));
  }
  delay(1);
}
