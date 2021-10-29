#pragma once
#include <Arduino.h>

// ----------------------------------------------------------Variables--------------------
uint32_t lastMillisHeartbit = 0;
bool eraseFlag = false;
bool needReboot = false;
// ---------------------------------------------------------Constants----------------
const uint16_t BLINKING_TIME_AP_MODE = 1000;  // once in N msec
const uint8_t HEARTBIT_DELAY = 9;             // sec
// ----------------------------------------------------------HTTP-----------
String ssidName = "Wifi_Name";  // WiFi name
String ssidPassword = "";       // WiFi password
String personMail = "";
String personId = "";
String token = "";
String host = "cloud.2smart.com";
String brPort = "1883";
String webAuthPass = "";
const char *http_username = "admin";
// -------------------------------------------------------Production settings
String sandboxLogin = "";
String deviceId = "";  // DeviceID/ MAC:adress
String deviceName = "2Smart Relay";
String firmwareName = "1625479282444746";  // will be changed
String productId = "1625479282444746";     // will be changed
String deviceVersion = "3";
// -------------------------------------------------------MQTT variables
String localIp = "";
String mac = "";

bool initNodes();
void handleMessage(char *topic, byte *payload, unsigned int length);
