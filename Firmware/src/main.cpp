#include "main.h"

#include "gpio.h"
#include "homie.h"
#include "property/relay_state.h"
#include "property/save_state.h"
#include "web_server.h"

MqttClient *mqtt_client = new MqttClient();
Homie homie(mqtt_client);
Notifier notifier(mqtt_client);
Device device(&homie);
WebServer web_server(&device);
NtpTimeClient *time_client = new NtpTimeClient();

WifiClient wifi_client;

void setup() {
    Serial.begin(115200);
    setGpios();

    if (!InitFiles() || !LoadConfig()) {
        delay(5000);
        ESP.restart();
    }

    String mac = WiFi.macAddress().c_str();
    if (device_id.length() <= 1) {
        String bufferMacAddr = WiFi.macAddress();
        bufferMacAddr.toLowerCase();
        bufferMacAddr.replace(":", "-");
        device_id = bufferMacAddr;
    }
    String ip_addr = WiFi.localIP().toString();

    // ---------------------------------------------- Homie convention init
    AutoUpdateFw *firmware = new AutoUpdateFw("Firmware", "firmware", &device);                   // (name, id,device)
    Notifications *notifications = new Notifications("Notifications", "notifications", &device);  // (name,id, device)

    Property *update_status = new Property("update status", "updatestate", firmware, SENSOR, false, false, "string");
    Property *update_button = new Property("update button", "update", firmware, SENSOR, true, false, "boolean");
    Property *update_time = new Property("update time", "updatetime", firmware, SENSOR, true, true, "string");
    Property *auto_update = new Property("autoUpdate", "autoupdate", firmware, SENSOR, true, true, "boolean");
    Property *fw_version = new Property("version", "version", firmware, SENSOR, false, true, "integer");
    // ------------- notification`s properties
    Property *system_notification =
        new Property("System Notifications", "system", notifications, SENSOR, true, true, "boolean");
    Property *update_notification =
        new Property("Update Notifications", "update", notifications, SENSOR, true, true, "boolean");

    // ------------- Device
    DeviceData device_data{device_name, device_version, product_id.c_str(), ip_addr.c_str(), "esp32",
                           mac.c_str(), "ready",        device_id.c_str()};

    notifier.SetUserHash(person_id);

    device.SetCredentials(device_data);
    device.SetNotifier(&notifier);

    Property *dev_ip = new Property("ipw", "ipw", &device, TELEMETRY, false, true, "string");

    /* -------------------- Start init your nodes and properties --------------------*/

    Node *relay = new Node("Relay", "relay", &device);  // (name, id,device)
    RelayState *relay_state = new RelayState("State", "state", relay, SENSOR, true, true, "boolean");
    SaveState *after_reboot_state =
        new SaveState("boot-state", "boot-state", relay, OPTION, true, true, "enum", "Off,On,Last");

    /* -------------------- End init your nodes and properties --------------------*/

    homie.SetDevice(&device);

    WifiAp wifiAP;
    if (ssid_name == "Wifi_Name" || ssid_name == "") {
        wifiAP.Start(device_name);
        web_server.Init();
    }
    while (ssid_name == "Wifi_Name" || ssid_name == "") {
        // Handling buttons and offline logic
        device.HandleCurrentState();
        wifiAP.Blink();
    }
    wifi_client.SetCredentials(ssid_name, ssid_password);
    while (!wifi_client.Connect()) {
        // Handling buttons and offline logic
        device.HandleCurrentState();
        if (erase_flag) {
            EraseFlash();
        }
    }
    time_client->Init();
    web_server.Init();

    ip_addr = WiFi.localIP().toString();
    Serial.print("IP: ");
    Serial.println(ip_addr);
    dev_ip->SetValue(ip_addr);

    while (!homie.Init(person_id, host, broker_port, token, HandleMessage)) {
        device.HandleCurrentState();
    }

    // ---------------------------------------------- Homie convention end
}
void loop() {
    wifi_client.Connect();

    homie.HandleCurrentState();  // mqttLoop();

    if (erase_flag) {
        EraseFlash();
    }
}
void HandleMessage(char *topic, byte *payload, unsigned int length) {
    homie.HandleMessage(String(topic), payload, length);
}
