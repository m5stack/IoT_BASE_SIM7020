#include <M5Stack.h>
#include <M5GFX.h>
#include "IoT_BASE_SIM7020.h"
#include <PubSubClient.h>
#include <TinyGsmClient.h>
#include <time.h>
#include <sys/time.h>

#define MQTT_BROKER             "mqtt.m5stack.com"
#define MQTT_PORT               1883
#define MQTT_USERNAME           "IoTBase-Thermal"
#define MQTT_PASSWORD           "IoTBase-Thermal"
#define MQTT_SYNC_TIME_D_TOPIC  "IoTBase/thermal/timesync/down"
#define MQTT_SYNC_TIME_U_TOPIC  "IoTBase/thermal/timesync/up"
#define MQTT_TOPIC              "temp/humidity/1"                      //  上传数据主题

#define UPLOAD_INTERVAL     10000

TinyGsm       modem(SerialAT, IoT_BASE_SIM7020_RESET);

TinyGsmClient tcpClient(modem);
PubSubClient  mqttClient(MQTT_BROKER, MQTT_PORT, tcpClient);

void mqttCallback(char *topic, byte *payload, unsigned int len);
void mqttConnect(void);
void nbConnect(void);

struct tm now;
char s_time[50];

M5GFX display;
M5Canvas canvas(&display);

void log(String info) {
  canvas.println(info);
  canvas.pushSprite(0, 0);
  SerialMon.println(info);
}


void setup()
{

    M5.begin();
    iotBaseInit();
    display.begin();
    canvas.setColorDepth(1); // mono color
    canvas.setFont(&fonts::efontCN_14);
    canvas.createSprite(display.width(), display.height());
    canvas.setTextSize(2);
    canvas.setPaletteColor(1, GREEN);
    canvas.setTextScroll(true);
    canvas.println(">>IoT BASE MQTT TEST");
    canvas.pushSprite(0, 0);
    SerialAT.begin(SIM7020_BAUDRATE, SERIAL_8N1, IoT_BASE_SIM7020_RX, IoT_BASE_SIM7020_TX);
    nbConnect();
    mqttClient.setBufferSize(4096);
    mqttClient.setCallback(mqttCallback);
    mqttClient.setKeepAlive(300);
}

void loop()
{
    static unsigned long timer = 0;

    if (!mqttClient.connected()) {
        if (!modem.isNetworkConnected()) {
            nbConnect();
        }
        log(">>MQTT NOT CONNECTED");
        mqttConnect();
    }

    if (millis() >= timer) {
        timer = millis() + UPLOAD_INTERVAL;
        mqttClient.publish(MQTT_TOPIC, "hello");   // 发送数据
    }

    mqttClient.loop();
}


void mqttCallback(char *topic, byte *payload, unsigned int len)
{
    char info[len];
    memcpy(info, payload, len);
    log("Message arrived [" + String(topic) + "]: ");
    log(info);
}

void mqttConnect(void)
{
    log("Connecting to " + String(MQTT_BROKER) + "...");

    // Connect to MQTT Broker
    String mqttid = ("MQTTID_" + String(random(65536)));
    while (!mqttClient.connect(mqttid.c_str(), MQTT_USERNAME, MQTT_PASSWORD)) {
        log(" fail");
    }
    log(" success");
    mqttClient.subscribe(MQTT_SYNC_TIME_D_TOPIC);
    while (!mqttClient.publish(MQTT_SYNC_TIME_U_TOPIC, "time sync")){
        delay(1000);
    }
}

void nbConnect(void)
{

    unsigned long start = millis();
    log("Initializing modem...");
    while (!modem.init()) {
        log("waiting...." + String((millis() - start)/1000) + "s");
    };

    start = millis();
    log("Waiting for network...");
    while (!modem.waitForNetwork()) {
        log("waiting...." + String((millis() - start)/1000) + "s");
    }
    log("success");
}



