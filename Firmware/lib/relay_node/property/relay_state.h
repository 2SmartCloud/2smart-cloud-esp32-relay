#pragma once

#include <Arduino.h>
#include <EncButton.h>

#include <map>
#include <string>

#include "homie.h"

class RelayState : public Property {
 public:
    RelayState(const char* name, const char* id, Node* node, PROPERTY_TYPE type, bool settable, bool retained,
               const char* data_type);

    bool Init(Homie* homie);

    void HandleCurrentState();

    void HandleSettingNewValue();

 private:
    bool LoadSettings();

    bool SaveSettings();

    bool state_saved = false;

    enum AfterRebootState { ON, OFF, LAST };
    EncButton<EB_TICK, 13> button_;

    const uint8_t kRelayPin = 4;

    uint32_t period_loop_ = millis();
};
