#pragma once
#include <Arduino.h>

#include "file_system/src/file_system.h"
#include "gpio.h"
#include "homie.h"
#include "web_server.h"
#include "wifi_ap/src/wifi_ap.h"
#include "wifi_client/src/wifi_client.h"

#include "property/relay_state.h"
#include "property/save_state.h"

#include "custom_nodes/reset_button/rst_button.h"
#include "custom_sensors/UpdateTime.h"
#include "custom_sensors/wifisignal/WifiSignal.h"

// ----------------------------------------------------------HTTP-----------
String ssid_name = "Wifi_Name";  // WiFi name
String ssid_password = "";       // WiFi password
String ap_password = "";         // initial access point password, 8-63 symbols
                                 // |NOTE| if left empty, AP will start open, with no auth
String person_mail = "";
String person_id = "";
String token = "";
String host = "cloud.2smart.com";
String broker_port = "1883";
String web_auth_password = "";
const char *http_username = "admin";
// -------------------------------------------------------Production settings
String product_id = "1625479282444746";  // product id
String device_id = "";  // DeviceID/MAC:adress
const char* device_name = "2Smart Relay";
const char* device_version = "8";
const char *firmware_name = product_id.c_str();
// --------------------------------------------------------------------------

void HandleMessage(char* topic, byte* payload, unsigned int length);
