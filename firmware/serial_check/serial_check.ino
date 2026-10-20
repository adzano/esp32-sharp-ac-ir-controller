#include <Arduino.h>

// Initial USB/firmware test. No external GPIO or Wi-Fi is used.
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("SMARTHOME_SERIAL_CHECK ready");
  Serial.printf("Chip: %s rev %d, cores: %d, flash: %u bytes\n",
                ESP.getChipModel(), ESP.getChipRevision(), ESP.getChipCores(),
                ESP.getFlashChipSize());
}

void loop() {
  Serial.printf("SMARTHOME_OK uptime_ms=%lu\n", millis());
  delay(2000);
}
