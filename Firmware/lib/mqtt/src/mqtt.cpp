#include "mqtt.h"

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "fileSystem.h"
#include "gpio.h"
#include "utils.h"

WiFiClient WifiClient;
PubSubClient client(WifiClient);

const char mainTopic[] = "sweet-home/";  // homie convention topic
const char implementation[] = "esp32";
const char state[] = "ready";

String topicPrefix;

bool reconnectMqtt = false;
bool mqttReconnected = false;
uint32_t reconnectMqttTime = 0;

bool initMqtt(MQTT_CALLBACK_SIGNATURE) {
    client.setServer(host.c_str(), brPort.toInt());
    client.setCallback(callback);
    topicPrefix = personId + "/" + mainTopic + deviceId + "/";
    return (reconnect());
}

bool reconnect() {
    if (personId == "") return false;
    if (!client.connected()) {
        std::string sessionId = "session_";
        sessionId.append(randomString(millis(), 8));

        if (client.connect(sessionId.c_str(), personId.c_str(), token.c_str())) {
            Serial.println("MQTT connected");
            initDevice();
            if (reconnectMqtt) mqttReconnected = true;
            reconnectMqtt = false;
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            reconnectMqtt = true;
            reconnectMqttTime = millis();
        }
    } else {
        if (reconnectMqtt) mqttReconnected = true;
        reconnectMqtt = false;
    }
    return (!reconnectMqtt);
}

bool mqttSubscribe(const char *topic) {  // susbcribe 1 topic
    std::string fullTopic = topicPrefix.c_str();
    fullTopic.append(topic);
    return client.subscribe(fullTopic.c_str());
}

bool mqttSubscribe(std::map<uint8_t, std::string> subsTopics) {  // subscribe map
    for (auto it = subsTopics.begin(); it != subsTopics.end(); ++it) {
        std::string subsTopic = topicPrefix.c_str() + it->second;
        if (!client.subscribe(subsTopic.c_str())) {
            Serial.println("Error in subscribing");
            Serial.printf("%s\r\n", subsTopic.c_str());
            return false;
        }
    }
    Serial.println("Subscribed");
    return true;
}

bool mqttPublish(const char *topic, const char *payload, bool retained = 1) {  // publish 1 topic
    std::string fullTopic = topicPrefix.c_str();
    fullTopic.append(topic);
    if (client.publish(fullTopic.c_str(), payload, retained)) {
        return true;
    } else {
        Serial.printf("Err publishing %s\r\n", fullTopic.c_str());
        return false;
    }
}

bool mqttPublish(topicsWithValues mapToSend) {  // publish map
    for (auto it = mapToSend.begin(); it != mapToSend.end(); ++it) {
        std::string fullTopic = topicPrefix.c_str();
        fullTopic.append(it->first);
        if (!client.publish(fullTopic.c_str(), it->second, true)) {
            Serial.printf("Err publishing %s\r\n", fullTopic.c_str());
            return false;
        }
    }
    return true;
}

bool initDevice() {
    bool statusPub = true;
    if (!mqttPublish("$name", deviceName.c_str(), true)) statusPub = false;

    if (!mqttPublish("$fw/name", productId.c_str(), true)) statusPub = false;

    if (!mqttPublish("$fw/version", deviceVersion.c_str(), true)) statusPub = false;

    if (!mqttPublish("$localip", localIp.c_str(), true)) statusPub = false;

    if (!mqttPublish("$implementation", implementation, true)) statusPub = false;

    if (!mqttPublish("$mac", mac.c_str(), true)) statusPub = false;

    if (!mqttPublish("$state", state, true)) statusPub = false;

    if (!mqttPublish("$telemetry/ipw", localIp.c_str(), true)) statusPub = false;

    if (!mqttPublish("$telemetry/ipw/$name", "IP WIFI", true)) statusPub = false;

    if (!mqttPublish("$telemetry/ipw/$settable", "false", true)) statusPub = false;

    if (!mqttPublish("$telemetry/ipw/$retained", "true", true)) statusPub = false;

    if (!mqttPublish("$telemetry/ipw/$datatype", "string", true)) statusPub = false;

    if (!mqttPublish("$heartbeat/set", " ", true)) statusPub = false;  // ------------Heartbit

    Serial.printf("Published %s \r\n", (statusPub) ? "success" : "failed");
    return statusPub;
}

bool sendNotification(const char *payload) {  //  publish in topic for notifications
    std::string fullTopic = personId.c_str();
    fullTopic.append("/notifications/create");
    StaticJsonDocument<256> doc;
    doc["logLevel"] = "info";
    doc["senderId"] = deviceId;
    doc["message"] = payload;
    doc["type"] = "text";
    doc["senderType"] = "device";
    String response;
    serializeJson(doc, response);
    if (client.publish(fullTopic.c_str(), response.c_str(), false)) {
        return true;
    } else {
        Serial.println("Err publishing notification");
        return false;
    }
}

void sendHeartbit() {
    Serial.print("heartbit ");
    const char *_space = " ";
    String topic = topicPrefix + "$heartbeat/" + "set";
    client.publish(topic.c_str(), _space, true);
    topic = topicPrefix + "$heartbeat";
    if (client.publish(topic.c_str(), _space, true)) {
        Serial.println("success");
    } else {
        Serial.println("reconnect");
        reconnectMqtt = true;
        reconnect();
    }
}

const char *removeSlashSetSubtopic(std::string topic) {
    size_t index = topic.find("/set");
    if (index == std::string::npos) return topic.c_str();

    topic.replace(index, 4, "");
    return topic.c_str();
}

void mqttLoop() { client.loop(); }
