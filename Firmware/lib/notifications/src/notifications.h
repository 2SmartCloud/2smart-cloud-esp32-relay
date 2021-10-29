#pragma once

#include <Arduino.h>

#include <map>
#include <string>

typedef struct {
    bool system;
    bool update;
} NotificationSettings;

extern NotificationSettings notifySettings;

bool initNotify();  // initialize toggles for notification
bool sendNotifyState();
bool subscribeNotifyMqtt();
bool handleNotifyMessage(char *topic, byte *payload, unsigned int length);

bool loadNotifySettings();
bool saveNotifySettings();
