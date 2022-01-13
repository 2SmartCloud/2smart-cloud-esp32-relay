#include "save_state.h"

#include "file_system/src/file_system.h"

SaveState::SaveState(const char* name, const char* id, Node* node, PROPERTY_TYPE type, bool settable, bool retained,
                     const char* data_type, String format, String unit)
    : Property(name, id, node, type, settable, retained, data_type, format, unit) {
    node->AddProperty(this);
}

bool SaveState::Init(Homie* homie) {  // initialize toggles for notification
    bool status = true;
    if (!Property::Init(homie)) status = false;
    LoadSettings();
    SetValue(value_);
    return status;
}

void SaveState::HandleSettingNewValue() {
    if (value_ == "Last") node_->GetProperty("state")->HandleSettingNewValue();
    SaveSettings();
}

bool SaveState::LoadSettings() {
    String file_name = "/" + id_ + ".txt";
    bool status = ReadSettings(file_name.c_str(), reinterpret_cast<byte*>(&value_), sizeof(value_));
    return status;
}

bool SaveState::SaveSettings() {
    String file_name = "/" + id_ + ".txt";
    return WriteSettings(file_name.c_str(), reinterpret_cast<byte*>(&value_), sizeof(value_));
}
