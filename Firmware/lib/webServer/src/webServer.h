
#pragma once
#include <ESPAsyncWebServer.h>
#include <Update.h>

extern const char *http_username;

extern String ssidName;
extern String ssidPassword;
extern String personMail;
extern String personId;
extern String token;
extern String host;
extern String brPort;
extern String webAuthPass;
extern String deviceId;
extern String mac;
extern String productId;
extern String firmwareName;

extern bool newRelayMqttData;

String fillPlaceholders(const String &var);
void setupWebServer();
void onRequestWithAuth(AsyncWebServerRequest *request, ArRequestHandlerFunction onRequest);
void onFirmwareUpload(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len,
                      bool final);
String sha256(String rec_data);
