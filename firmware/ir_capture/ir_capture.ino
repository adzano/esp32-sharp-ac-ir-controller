#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRac.h>
#include <IRutils.h>

// Receiver DATA -> GPIO27, VCC -> 3V3, GND -> GND.
// Capture only: no IR transmitter or network activity.
IRrecv receiver(27, 1024, 50, true);
decode_results result;
uint32_t captures = 0;
uint32_t lastStatus = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  receiver.setUnknownThreshold(12);
  // Captures showed shortened marks; retain protocol checksum validation.
  receiver.setTolerance(35);
  receiver.enableIRIn();
  Serial.println("IR_CAPTURE_READY gpio=27 baud=115200 buffer=1024 timeout_ms=50 tolerance=35");
  Serial.println("Library: " _IRREMOTEESP8266_VERSION_STR);
}

void loop() {
  if (receiver.decode(&result)) {
    Serial.printf("\nCAPTURE_BEGIN id=%lu uptime_ms=%lu overflow=%u\n",
                  ++captures, millis(), result.overflow);
    Serial.print(resultToHumanReadableBasic(&result));
    const String state = IRAcUtils::resultAcToString(&result);
    if (state.length()) Serial.println("AC_STATE: " + state);
    Serial.println(resultToSourceCode(&result));
    Serial.println("CAPTURE_END");
  }
  if (millis() - lastStatus >= 10000) {
    lastStatus = millis();
    Serial.printf("IR_WAITING captures=%lu input=%d\n", captures, digitalRead(27));
  }
  delay(1);
}
