#pragma once
#include <SPIFFS.h>  // For FS & FileConfig

typedef struct {
    char ssidName[33];      // WiFi name
    char ssidPassword[65];  // WiFi password
    char personMail[65];
    char personId[65];
    char token[11];
    char host[65];
    char brPort[6];
    char deviceId[32];
    char productId[32];
    char webAuthPass[32];
} UserData;

extern String ssidName;
extern String ssidPassword;
extern String personMail;
extern String personId;
extern String token;
extern String host;
extern String brPort;
extern String deviceId;
extern String mac;
extern String productId;
extern String webAuthPass;

bool initFiles();
bool loadConfig();
bool saveConfig();
bool eraseFlash();
bool writeSettings(const char *filePath, uint8_t *data, size_t sizeOfData);
bool readSettings(const char *filePath, uint8_t *data, size_t sizeOfData);
