#pragma once

#include <Arduino.h>

#include <map>
#include <string>

enum fwStates { FW_CHECK, FW_ACTUAL, FW_UPDATING, FW_UPDATED, FW_FAILED };  // T-topic

bool sendFwSettings();
bool sendFwParams();
bool subscribeFwMqtt();
bool sendUpdated();
bool handleFwMessage(char* topic, byte* payload, unsigned int length);
bool updateFwState(uint8_t state);
