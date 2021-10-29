#include "relayMqtt.h"

#include "gpio.h"
#include "mqtt.h"
bool newRelayMqttData = false;

enum topics { T_RELAY_BTN };  // T-topic

std::map<uint8_t, std::string> subsRelayTopics = {{T_RELAY_BTN, "relay/state/set"}};

bool sendRelaySettings() {
    std::map<const char*, const char*> topics = {
        {"relay/$name", "Relay"},          {"relay/$state", "ready"},         {"relay/state/$name", "State"},
        {"relay/state/$settable", "true"}, {"relay/state/$retained", "true"}, {"relay/state/$datatype", "boolean"}};
    return (mqttPublish(topics));
}

bool subscribeRelayMqtt() { return (mqttSubscribe(subsRelayTopics)); }

bool sendRelayParams() {  // Send actual params on init
    std::map<const char*, std::string> relaySettingsTopics = {{"relay/state", ""}};

    relaySettingsTopics["relay/state"] = (_ledState) ? "true" : "false";

    for (auto it = relaySettingsTopics.begin(); it != relaySettingsTopics.end(); ++it) {
        if (!mqttPublish(it->first, it->second.c_str(), true)) {
            return false;
        }
    }
    return (true);
}

bool updateRelayState(bool notifyEnabled) {
    if (notifyEnabled) sendNotification(_ledState ? "Relay on" : "Relay off");
    return (mqttPublish("relay/state", (_ledState) ? "true" : "false", true));
}

bool handleRelayMessage(char* topic, byte* payload, unsigned int length) {
    String topicHolder = topic;

    bool messHandled = false;

    std::string buffSuffix = subsRelayTopics.find(T_RELAY_BTN)->second;

    if (topicHolder.endsWith(buffSuffix.c_str())) {  // auto update
        _ledState = static_cast<char>(payload[0]) == 't';

        digitalWrite(LED, _ledState);
        mqttPublish(removeSlashSetSubtopic(buffSuffix), (_ledState) ? "true" : "false", true);
        messHandled = true;
    }

    return messHandled;
}
