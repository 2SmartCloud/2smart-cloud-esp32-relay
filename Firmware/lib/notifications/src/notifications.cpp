#include "notifications.h"

#include "fileSystem.h"
#include "mqtt.h"

NotificationSettings notifySettings = {true, true};

enum topics { T_NOTIFY_SYSTEM, T_NOTIFY_UPDATE };  // T-topic

std::map<uint8_t, std::string> subsNotifyTopics = {{T_NOTIFY_SYSTEM, "notifications/system/set"},
                                                   {T_NOTIFY_UPDATE, "notifications/update/set"}};

bool initNotify() {  // initialize toggles for notification
    std::map<const char *, const char *> topics = {{"notifications/$name", "Notifications"},
                                                   {"notifications/$state", "ready"},
                                                   {"notifications/system/$name", "System Notifications"},
                                                   {"notifications/system/$settable", "true"},
                                                   {"notifications/system/$retained", "true"},
                                                   {"notifications/system/$datatype", "boolean"},
                                                   {"notifications/update/$name", "Update Notifications"},
                                                   {"notifications/update/$settable", "true"},
                                                   {"notifications/update/$retained", "true"},
                                                   {"notifications/update/$datatype", "boolean"}};

    return mqttPublish(topics) && sendNotifyState() && subscribeNotifyMqtt();
}

bool sendNotifyState() {
    loadNotifySettings();
    return mqttPublish("notifications/system", notifySettings.system ? "true" : "false", true) &&
           mqttPublish("notifications/update", notifySettings.update ? "true" : "false", true);
}

bool subscribeNotifyMqtt() { return (mqttSubscribe(subsNotifyTopics)); }

bool handleNotifyMessage(char *topic, byte *payload, unsigned int length) {
    String topicHolder = topic;

    bool messHandled = false;

    std::string buffSuffix = subsNotifyTopics.find(T_NOTIFY_SYSTEM)->second;

    if (topicHolder.endsWith(buffSuffix.c_str())) {  // toggle system notification
        notifySettings.system = static_cast<char>(payload[0]) == 't';

        mqttPublish(removeSlashSetSubtopic(buffSuffix), (notifySettings.system) ? "true" : "false", true);
        messHandled = true;
    }

    buffSuffix = subsNotifyTopics.find(T_NOTIFY_UPDATE)->second;

    if (topicHolder.endsWith(buffSuffix.c_str())) {  // toggle update notification
        notifySettings.update = static_cast<char>(payload[0]) == 't';

        mqttPublish(removeSlashSetSubtopic(buffSuffix), (notifySettings.update) ? "true" : "false", true);
        messHandled = true;
    }
    if (messHandled) saveNotifySettings();
    return messHandled;
}

bool loadNotifySettings() {
    return readSettings("/notifyconf.txt", reinterpret_cast<byte *>(&notifySettings), sizeof(notifySettings));
}

bool saveNotifySettings() {
    return writeSettings("/notifyconf.txt", reinterpret_cast<byte *>(&notifySettings), sizeof(notifySettings));
}
