#pragma once

#include <Arduino.h>

#include <map>
#include <string>

#include "homie.h"
// #include "node/node.h"
// #include "property/property.h"

class SaveState : public Property {
 public:
    SaveState(const char* name, const char* id, Node* node, PROPERTY_TYPE type, bool settable, bool retained,
              const char* data_type, String format = "", String unit = "");

    bool Init(Homie* homie);

    void HandleSettingNewValue();

 private:
    bool LoadSettings();

    bool SaveSettings();

    enum AfterRebootState { ON, OFF, LAST };

    uint32_t period_loop_ = millis();
};
