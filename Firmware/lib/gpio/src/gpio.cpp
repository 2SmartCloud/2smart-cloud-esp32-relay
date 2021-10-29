#include "gpio.h"

#include "relayMqtt.h"

#define NOISE_FILTER 300

bool _ledState = false;
uint64_t lastClickButton = 0;
hw_timer_t* timer = NULL;

void IRAM_ATTR onTimer() {
    if (!digitalRead(ERASE_FLASH)) {
        eraseFlag = true;
    } else {
        timerAlarmDisable(timer);
    }
}

void IRAM_ATTR eraseInterrupt() {
    timer = timerBegin(0, 80, true);  // 80=prescaller 80Mhz/prescaller = 1 000 000 counts in a sec
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, RELOAD_TIME * 1000000, false);  // timer overflow in (reload time) seconds
    timerAlarmEnable(timer);
}

void IRAM_ATTR buttonInterrupt() {
    if (!digitalRead(BUTTON) && millis() - lastClickButton > NOISE_FILTER) {
        _ledState = !_ledState;
        digitalWrite(LED, _ledState);
        lastClickButton = millis();
        newRelayMqttData = true;
    }
}

void setGpios() {
    pinMode(ERASE_FLASH, INPUT_PULLUP);
    attachInterrupt(ERASE_FLASH, eraseInterrupt, FALLING);
    pinMode(LED_STATUS, OUTPUT);
    digitalWrite(LED_STATUS, LOW);
    pinMode(BUTTON, INPUT_PULLUP);
    attachInterrupt(BUTTON, buttonInterrupt, FALLING);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, _ledState);
}
