#pragma once

// Salin file ini menjadi secrets.h dan isi nilai lokalnya.
// Public MQTTS broker. Do not use TCP port 1883 for an Internet connection.
constexpr char kMqttHost[] = "mqtt.elang.my.id";
constexpr uint16_t kMqttPort = 8883;
constexpr char kMqttUser[] = "smarthome";
constexpr char kMqttPassword[] = "replace-with-local-secret";
