#include "fileSystem.h"

bool initFiles() {
    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return false;
    }

    File root = SPIFFS.open("/");
    File file = root.openNextFile();

    while (file) {
        file = root.openNextFile();
    }
    return true;
}

bool loadConfig() {
    if (ssidName != "Wifi_Name" && ssidName != "") {
        return false;
    }

    UserData userData = {"", "", "", "", "", "", "", "", "", ""};
    readSettings("/config.txt", reinterpret_cast<byte *>(&userData), sizeof(userData));
    if (strlen(userData.ssidName) == 0) {
        eraseFlash();
    }

    ssidName = userData.ssidName;
    ssidPassword = userData.ssidPassword;
    personMail = userData.personMail;
    personId = userData.personId;
    token = userData.token;
    host = userData.host;
    brPort = userData.brPort;
    deviceId = userData.deviceId;
    productId = userData.productId;
    webAuthPass = userData.webAuthPass;

    return true;
}

bool saveConfig() {
    UserData userData = {"", "", "", "", "", "", "", "", "", ""};
    ssidName.toCharArray(userData.ssidName, ssidName.length() + 1);
    ssidPassword.toCharArray(userData.ssidPassword, ssidPassword.length() + 1);
    personMail.toCharArray(userData.personMail, personMail.length() + 1);
    personId.toCharArray(userData.personId, personId.length() + 1);
    token.toCharArray(userData.token, token.length() + 1);
    host.toCharArray(userData.host, host.length() + 1);
    brPort.toCharArray(userData.brPort, brPort.length() + 1);
    deviceId.toCharArray(userData.deviceId, deviceId.length() + 1);
    productId.toCharArray(userData.productId, productId.length() + 1);
    webAuthPass.toCharArray(userData.webAuthPass, webAuthPass.length() + 1);

    return writeSettings("/config.txt", reinterpret_cast<byte *>(&userData), sizeof(userData));
}

bool eraseFlash() {
    Serial.println("Create default config file");
    UserData userData = {"Wifi_Name", "", "", "", "", "", "", "", "", "admin"};
    productId.toCharArray(userData.productId, productId.length() + 1);

    if (writeSettings("/config.txt", reinterpret_cast<byte *>(&userData), sizeof(userData))) ESP.restart();
    return false;
}

bool writeSettings(const char *filePath, uint8_t *data, size_t sizeOfData) {
    File configFile = SPIFFS.open(filePath, FILE_WRITE);
    if (!configFile) {
        Serial.printf("Failed to open %s for writing\r\n", filePath);
        return false;
    }
    configFile.write(data, sizeOfData);
    configFile.close();
    Serial.printf("File %s saved\r\n", filePath);
    return true;
}

bool readSettings(const char *filePath, uint8_t *data, size_t sizeOfData) {
    File configFile = SPIFFS.open(filePath, FILE_READ);
    if (!configFile) {
        Serial.printf("Failed to open %s for reading\r\n", filePath);
        return false;
    }
    size_t size = configFile.size();
    if (size > 1024) {
        Serial.printf("file %s is too large \r\n", filePath);
        return false;
    }

    configFile.read(data, sizeOfData);
    configFile.close();

    return true;
}
