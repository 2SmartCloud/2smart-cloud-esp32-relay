#pragma once

#include <Arduino.h>

#include "auMqtt.h"

typedef struct {
    bool autoUpdate;
    uint8_t version;
    uint8_t hours;
    uint8_t minutes;
} FwSettings;

extern String host;
extern String productId;  // from main.h
extern FwSettings fwSettings;

extern const uint8_t FIRMWARE_DELAY;  // sec

extern bool forceUpdate;
extern bool newFwSettings;

void checkFirmware(uint8_t firmwareVer, bool notifyEnabled);
uint8_t checkFirmwareVersion();
void updateFirmaware();
bool loadFwSettings();
bool saveFwSettings();
