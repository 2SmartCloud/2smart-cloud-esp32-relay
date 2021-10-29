#pragma once

#include <Arduino.h>

#include <map>
#include <string>

extern bool newRelayMqttData;

bool sendRelaySettings();
bool sendRelayParams();
bool subscribeRelayMqtt();
bool handleRelayMessage(char* topic, byte* payload, unsigned int length);
bool updateRelayState(bool notifyEnabled);
