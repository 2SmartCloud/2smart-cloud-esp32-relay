#include "autoUpd.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>

#include "fileSystem.h"
#include "ntpTime.h"

const uint8_t FIRMWARE_DELAY = 60;  // sec
uint32_t lastMillisFirmware = 0;
bool forceUpdate = false;
bool newFwSettings = false;

FwSettings fwSettings = {1, 0, 0, 0};

const char* URL_FW_VERSION_PATH = "/firmwares/v1/products/";  // then ad "productId" and "firmware-version"
const char* URL_FW_BIN_PATH = "/firmwares/v1/";               // then ad "productId".bin

void checkFirmware(uint8_t firmwareVer, bool notifyEnabled) {
    if (firmwareVer != fwSettings.version) {
        if ((notifyEnabled && sendUpdated()) || !notifyEnabled) {
            fwSettings.version = firmwareVer;
            newFwSettings = true;
            updateFwState(FW_UPDATED);
        }
        sendFwParams();
    }
    if (millis() - lastMillisFirmware > FIRMWARE_DELAY * 1000) {
        if (fwSettings.autoUpdate) {
            uint8_t hours = getHours();
            if (hours == fwSettings.hours) {
                uint8_t minutes = getMinutes();
                if (minutes == fwSettings.minutes) {
                    if (checkFirmwareVersion() > firmwareVer) {
                        Serial.println("Available new firmware");
                        updateFirmaware();
                    } else {
                        Serial.println("Already on latest version");
                    }
                }
            }
        }
        lastMillisFirmware = millis();
    }
    if (forceUpdate) {
        updateFwState(FW_CHECK);
        if (checkFirmwareVersion() > firmwareVer) {
            Serial.println("Available new firmware");
            updateFirmaware();
        } else {
            updateFwState(FW_ACTUAL);
            Serial.println("Already on latest version");
            forceUpdate = false;
        }
    }
    if (newFwSettings) {
        if (saveFwSettings()) newFwSettings = false;
    }
}

uint8_t checkFirmwareVersion() {
    uint8_t newVersion = 0;
    HTTPClient httpClient;
    String url = "https://" + host + URL_FW_VERSION_PATH + productId + "/firmware-version";
    if (httpClient.begin(url)) {  // HTTPS
        int httpCode = httpClient.GET();
        if (httpCode == 200) {
            String newFWVersion = httpClient.getString();
            StaticJsonDocument<200> doc;
            deserializeJson(doc, newFWVersion);
            newVersion = doc["firmware_version"];
        } else {
            Serial.print("Firmware version check failed, got HTTP response code ");
            Serial.println(httpCode);
        }
        httpClient.end();
    }
    return newVersion;
}

void updateFirmaware() {
    updateFwState(FW_UPDATING);
    Serial.println("updating");
    HTTPClient httpClient;
    String url = "https://" + host + URL_FW_BIN_PATH + productId + ".bin";
    if (httpClient.begin(url)) {  // HTTPS
        uint8_t updateStatus = httpUpdate.update(httpClient);
        switch (updateStatus) {
            case HTTP_UPDATE_FAILED:
                Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", httpUpdate.getLastError(),
                              httpUpdate.getLastErrorString().c_str());
                updateFwState(FW_FAILED);
                forceUpdate = false;
                break;

            case HTTP_UPDATE_NO_UPDATES:
                Serial.println("HTTP_UPDATE_NO_UPDATES");
                break;
        }
    }
}

bool loadFwSettings() {
    readSettings("/fwconf.txt", reinterpret_cast<byte*>(&fwSettings), sizeof(fwSettings));

    if (fwSettings.version) {
        return true;
    }

    return false;
}

bool saveFwSettings() { return writeSettings("/fwconf.txt", reinterpret_cast<byte*>(&fwSettings), sizeof(fwSettings)); }
