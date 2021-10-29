#include "customWifi.h"

#define WIFI_RECONNECT_TIME 5000
uint32_t wifiReconnectTime = 0;
uint32_t lastBlinkTime = 0;

bool wifiInit() {
    if (WiFi.status() != WL_CONNECTED) {  // If not connected
        if ((millis() - WIFI_RECONNECT_TIME > wifiReconnectTime && millis() > WIFI_RECONNECT_TIME) ||
            !wifiReconnectTime) {
            WiFi.mode(WIFI_STA);
            WiFi.begin(ssidName.c_str(), ssidPassword.c_str());
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP());
            Serial.println(ssidName.c_str());
            Serial.println(ssidPassword.c_str());
            wifiReconnectTime = millis();
        } else if (millis() - BLINKING_PAIRING_TIME > lastBlinkTime) {
            lastBlinkTime = millis();
            Serial.print(".");
            digitalWrite(LED_STATUS, !digitalRead(LED_STATUS));
        }
        return false;
    }
    digitalWrite(LED_STATUS, true);
    return true;
}

void startWifiAp() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(deviceName.c_str());
    IPAddress local_ip(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
}
