#include "relay_state.h"

#include "file_system/src/file_system.h"

RelayState::RelayState(const char* name, const char* id, Node* node, PROPERTY_TYPE type, bool settable, bool retained,
                       const char* data_type)
    : Property(name, id, node, type, settable, retained, data_type) {
    node->AddProperty(this);

    pinMode(kRelayPin, OUTPUT);
}

bool RelayState::Init(Homie* homie) {  // initialize toggles for notification
    bool status = true;
    if (!Property::Init(homie)) status = false;

    String after_reboot_value = node_->GetProperty("boot-state")->GetValue();
    Serial.println("value of boot-state recieved");
    if (after_reboot_value == "Last") {
        LoadSettings();
    } else {
        value_ = after_reboot_value == "On" ? "true" : "false";
    }

    SetValue(value_ == "true" ? "true" : "false");

    return status;
}

void RelayState::HandleCurrentState() {
    button_.tick();
    if (button_.isPress()) SetValue(value_ == "true" ? "false" : "true");
}

void RelayState::HandleSettingNewValue() {
    Serial.println(value_ == "true" ? "Relay on" : "Relay off");
    digitalWrite(kRelayPin, value_ == "true");

    if (HasNewValue()) {
        Device* device = node_->GetDevice();
        bool is_sys_notif_enabled = device->IsSysNotifyEnabled();
        if (is_sys_notif_enabled) device->SendNotification(value_ == "true" ? "Relay on" : "Relay off");
        SetHasNewValue(false);
    }
    if (node_->GetProperty("boot-state")->GetValue() == "Last") {
        SaveSettings();
    }
}

bool RelayState::LoadSettings() {
    String file_name = "/" + id_ + ".txt";
    bool status = ReadSettings(file_name.c_str(), reinterpret_cast<byte*>(&value_), sizeof(value_));
    return status;
}

bool RelayState::SaveSettings() {
    if (value_ == "") return false;
    String file_name = "/" + id_ + ".txt";
    bool status = WriteSettings(file_name.c_str(), reinterpret_cast<byte*>(&value_), sizeof(value_));
    return status;
}
