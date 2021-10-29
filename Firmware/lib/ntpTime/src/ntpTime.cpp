#include "ntpTime.h"

#include <NTPClient.h>
#include <WiFiUdp.h>

#define TIME_ZONE 3            // Kiev +3
#define UPDATE_INTERVAL 55000  // in ms

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void initNtp() {
    timeClient.begin();
    timeClient.setUpdateInterval(UPDATE_INTERVAL);
    timeClient.setTimeOffset(TIME_ZONE * 60 * 60);  // time zone * 60min * 60 sec
}

uint8_t getHours() {
    timeClient.update();
    return timeClient.getHours();
}

uint8_t getMinutes() {
    timeClient.update();
    return timeClient.getMinutes();
}
