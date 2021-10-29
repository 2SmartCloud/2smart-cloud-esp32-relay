#pragma once
#include <Arduino.h>

#include <functional>
#include <map>
#include <string>

#define MSG_BUFFER_SIZE 256
#define DELAY_FOR_RECOONECT_MQTT 5000  // 5 sec

typedef std::map<const char *, const char *> topicsWithValues;

extern String personMail;
extern String personId;
extern String token;
extern String host;
extern String brPort;

extern String deviceId;
extern String deviceName;
extern String productId;
extern String deviceVersion;
extern String localIp;
extern String mac;

extern bool _ledState;

extern bool mqttReconnected;
extern bool reconnectMqtt;
extern uint32_t reconnectMqttTime;

bool initMqtt(std::function<void(char *, uint8_t *, unsigned int)> callback);  // MQTT_CALLBACK_SIGNATURE
bool reconnect();
bool initDevice();
void sendHeartbit();
void mqttLoop();

bool mqttSubscribe(const char *topic);
bool mqttSubscribe(std::map<uint8_t, std::string> subsTopics);

const char *removeSlashSetSubtopic(std::string topic);

bool sendNotification(const char *payload);
bool mqttPublish(const char *topic, const char *payload, bool retained);
bool mqttPublish(topicsWithValues mapToSend);
