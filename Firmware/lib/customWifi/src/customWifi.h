#pragma once
#include <Arduino.h>
#include <WiFi.h>

#include "gpio.h"

extern String ssidName;
extern String ssidPassword;
extern String deviceName;
extern const uint8_t LED_STATUS;
extern const uint16_t SEC;

const uint8_t WIFI_RECONNECT_TIME = 10;  // sec
const byte BLINKING_PAIRING_TIME = 250;  // ms

bool wifiInit();
void startWifiAp();
