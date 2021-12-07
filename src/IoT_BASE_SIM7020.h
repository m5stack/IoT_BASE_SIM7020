
#include <Arduino.h>

#define TINY_GSM_MODEM_SIM7020

#define SerialMon           Serial
#define MONITOR_BAUDRATE    115200

#define SerialAT            Serial1
#define SIM7020_BAUDRATE    115200

#define IoT_BASE_SIM7020_RESET       -1
#define IoT_BASE_SIM7020_EN          12
#define IoT_BASE_SIM7020_TX          0
#define IoT_BASE_SIM7020_RX          35

#define IoT_BASE_RS485_TX          15
#define IoT_BASE_RS485_RX          13

void iotBaseInit() {
    pinMode(IoT_BASE_SIM7020_EN, OUTPUT);
    digitalWrite(IoT_BASE_SIM7020_EN, 1);
};
