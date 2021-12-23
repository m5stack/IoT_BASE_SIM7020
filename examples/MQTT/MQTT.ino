#include <M5Stack.h>
#include <M5GFX.h>
#include "IoT_BASE_SIM7020.h"
#include <PubSubClient.h>
#include <TinyGsmClient.h>
#include <time.h>
#include <sys/time.h>


#define MQTT_BROKER   "mqtt.m5stack.com"
#define MQTT_PORT     1883
#define MQTT_USERNAME "IoT_BASE_CATM"
#define MQTT_PASSWORD "IoT_BASE_PWD"
#define MQTT_D_TOPIC  "IoT_BASE_CATM/D"
#define MQTT_U_TOPIC  "IoT_BASE_CATM/U"  //  上传数据主题

#define UPLOAD_INTERVAL 10000
uint32_t lastReconnectAttempt = 0;

TinyGsm modem(SerialAT, IoT_BASE_SIM7020_RESET);

TinyGsmClient tcpClient(modem);
PubSubClient mqttClient(MQTT_BROKER, MQTT_PORT, tcpClient);

void mqttCallback(char *topic, byte *payload, unsigned int len);
bool mqttConnect(void);
void nbConnect(void);

// Your GPRS credentials, if any
const char apn[]      = "YourAPN";
const char gprsUser[] = "";
const char gprsPass[] = "";

struct tm now;
char s_time[50];

M5GFX display;
M5Canvas canvas(&display);

void log(String info) {
    canvas.println(info);
    canvas.pushSprite(0, 0);
    SerialMon.println(info);
}

void setup() {
    M5.begin();
    iotBaseInit();
    display.begin();
    canvas.setColorDepth(1);  // mono color
    canvas.setFont(&fonts::efontCN_14);
    canvas.createSprite(display.width(), display.height());
    canvas.setTextSize(2);
    canvas.setPaletteColor(1, GREEN);
    canvas.setTextScroll(true);
    canvas.println(">>IoT BASE MQTT TEST");
    canvas.pushSprite(0, 0);
    SerialAT.begin(SIM7020_BAUDRATE, SERIAL_8N1, IoT_BASE_SIM7020_RX,
                   IoT_BASE_SIM7020_TX);
    nbConnect();
    mqttClient.setCallback(mqttCallback);
}

void loop() {
    static unsigned long timer = 0;

    if (!mqttClient.connected()) {
        log(">>MQTT NOT CONNECTED");
        // Reconnect every 10 seconds
        uint32_t t = millis();
        if (t - lastReconnectAttempt > 3000L) {
            lastReconnectAttempt = t;
            if (mqttConnect()) {
                lastReconnectAttempt = 0;
            }
        }
        delay(100);
    }

    if (millis() >= timer) {
        timer = millis() + UPLOAD_INTERVAL;
        mqttClient.publish(MQTT_U_TOPIC, "hello");  // 发送数据
    }

    mqttClient.loop();
}

void mqttCallback(char *topic, byte *payload, unsigned int len) {
    char info[len];
    memcpy(info, payload, len);
    log("Message arrived [" + String(topic) + "]: ");
    log(info);
}

bool mqttConnect(void) {
    log("Connecting to ");
    log(MQTT_BROKER);
    // Connect to MQTT Broker
    String mqttid = ("MQTTID_" + String(random(65536)));
    bool status =
        mqttClient.connect(mqttid.c_str(), MQTT_USERNAME, MQTT_PASSWORD);
    if (status == false) {
        log(" fail");
        return false;
    }
    log("MQTT CONNECTED!");
    mqttClient.publish(MQTT_U_TOPIC, "CATM MQTT CLIENT ONLINE");
    mqttClient.subscribe(MQTT_D_TOPIC);
    return mqttClient.connected();
}

void nbConnect(void) {
    unsigned long start = millis();
    log("Initializing modem...");
    while (!modem.init()) {
        log("waiting...." + String((millis() - start) / 1000) + "s");
    };

    start = millis();
    log("Waiting for network...");
    while (!modem.waitForNetwork()) {
        log("waiting...." + String((millis() - start) / 1000) + "s");
    }
    log("success");
}
