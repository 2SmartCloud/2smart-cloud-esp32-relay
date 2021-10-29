#include "auMqtt.h"

#include "autoUpd.h"
#include "mqtt.h"
bool newUpdMqttData = false;

enum topics { T_UPDATE_BTN, T_AUTO_UPDATE, T_TIME };  // T-topic

std::map<uint8_t, std::string> subsFwTopics = {{T_UPDATE_BTN, "firmware/update/set"},
                                               {T_AUTO_UPDATE, "firmware/autoupdate/set"},
                                               {T_TIME, "firmware/updatetime/set"}};

std::map<uint8_t, std::string> fwStates = {{FW_CHECK, "Checking for updates"},
                                           {FW_ACTUAL, "Firmware is up to date"},
                                           {FW_UPDATING, "Updating"},
                                           {FW_UPDATED, "Updated successfully"},
                                           {FW_FAILED, "Update failed"}};

bool sendFwSettings() {
    std::map<const char*, const char*> topics = {{"firmware/$name", "Firmware"},
                                                 {"firmware/$state", "ready"},
                                                 {"firmware/update", "false"},
                                                 {"firmware/update/$name", "update"},
                                                 {"firmware/update/$settable", "true"},
                                                 {"firmware/update/$retained", "false"},
                                                 {"firmware/update/$datatype", "boolean"},
                                                 {"firmware/autoupdate/$name", "autoUpdate"},
                                                 {"firmware/autoupdate/$settable", "true"},
                                                 {"firmware/autoupdate/$retained", "true"},
                                                 {"firmware/autoupdate/$datatype", "boolean"},
                                                 {"firmware/updatetime/$name", "updateTime"},
                                                 {"firmware/updatetime/$settable", "true"},
                                                 {"firmware/updatetime/$retained", "true"},
                                                 {"firmware/updatetime/$datatype", "string"},
                                                 {"firmware/updatestate", "Firmware is up to date"},
                                                 {"firmware/updatestate/$name", "updateState"},
                                                 {"firmware/updatestate/$settable", "false"},
                                                 {"firmware/updatestate/$retained", "false"},
                                                 {"firmware/updatestate/$datatype", "string"},
                                                 {"firmware/version/$name", "Version"},
                                                 {"firmware/updatetime/$settable", "false"},
                                                 {"firmware/updatetime/$retained", "true"},
                                                 {"firmware/updatetime/$datatype", "string"}};
    return (mqttPublish(topics));
}

bool subscribeFwMqtt() { return (mqttSubscribe(subsFwTopics)); }

bool sendFwParams() {  // Send actual params on init
    std::map<const char*, std::string> fwSettingsTopics = {
        {"firmware/autoupdate", ""}, {"firmware/updatetime", ""}, {"firmware/version", ""}};

    char messageBuffer[6];  // max length of message (time)

    fwSettingsTopics["firmware/autoupdate"] = (fwSettings.autoUpdate) ? "true" : "false";

    snprintf(messageBuffer, sizeof(messageBuffer), "%d:%d", fwSettings.hours, fwSettings.minutes);
    fwSettingsTopics["firmware/updatetime"] = messageBuffer;

    snprintf(messageBuffer, sizeof(messageBuffer), "%d", fwSettings.version);
    fwSettingsTopics["firmware/version"] = messageBuffer;

    for (auto it = fwSettingsTopics.begin(); it != fwSettingsTopics.end(); ++it) {
        if (!mqttPublish(it->first, it->second.c_str(), true)) {
            return false;
        }
    }
    return (true);
}

bool sendUpdated() { return sendNotification("Firmware updated"); }

bool updateFwState(uint8_t state) {
    return (mqttPublish("firmware/updatestate", fwStates.find(state)->second.c_str(), false));
}

bool handleFwMessage(char* topic, byte* payload, unsigned int length) {
    String topicHolder = topic;

    bool messHandled = false;

    std::string buffSuffix = subsFwTopics.find(T_UPDATE_BTN)->second;

    if (topicHolder.endsWith(buffSuffix.c_str())) {  // update
        if (static_cast<char>(payload[0]) == 't') {
            mqttPublish(removeSlashSetSubtopic(buffSuffix), "true", true);
            forceUpdate = true;
        }
        messHandled = true;
    }

    buffSuffix = subsFwTopics.find(T_AUTO_UPDATE)->second;
    if (topicHolder.endsWith(buffSuffix.c_str())) {  // auto update
        fwSettings.autoUpdate = static_cast<char>(payload[0]) == 't';

        mqttPublish(removeSlashSetSubtopic(buffSuffix), (fwSettings.autoUpdate) ? "true" : "false", true);
        newFwSettings = true;
        messHandled = true;
    }

    buffSuffix = subsFwTopics.find(T_TIME)->second;

    if (topicHolder.endsWith(subsFwTopics.find(T_TIME)->second.c_str())) {  // time
        bool invalidType = true;
        if (length < 6) {
            String value = "";
            for (int i = 0; i < length; i++) {
                value += (static_cast<char>(payload[i]));
            }
            uint8_t separatorPos = value.indexOf(":");
            if (separatorPos == 1 || separatorPos == 2) {
                uint8_t hours = value.substring(0, separatorPos).toInt();
                uint8_t minutes = value.substring(separatorPos + 1, length).toInt();
                if (hours < 24 && minutes < 60) {
                    fwSettings.hours = hours;
                    fwSettings.minutes = minutes;
                    invalidType = false;
                    newFwSettings = true;
                }
            }
            mqttPublish(removeSlashSetSubtopic(buffSuffix), value.c_str(), true);
        }
        if (invalidType) {
            char messageBuffer[6];
            snprintf(messageBuffer, sizeof(messageBuffer), "%d:%d", fwSettings.hours, fwSettings.minutes);
            mqttPublish(removeSlashSetSubtopic(buffSuffix), messageBuffer, true);
        }
        messHandled = true;
    }
    return messHandled;
}
