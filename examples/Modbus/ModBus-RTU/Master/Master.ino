#include <M5Stack.h>
#include <M5GFX.h>
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>

#include "IoT_BASE_SIM7020.h"

RS485Class RS485(Serial2, IoT_BASE_RS485_RX, IoT_BASE_RS485_TX, -1, -1);
M5GFX display;
M5Canvas canvas(&display);

void setup() {
    M5.begin();
    display.begin();
    canvas.setColorDepth(1);  // mono color
    canvas.setFont(&fonts::efontCN_14);
    canvas.createSprite(display.width(), display.height());
    canvas.setTextSize(2);
    canvas.setPaletteColor(1, GREEN);
    canvas.setTextScroll(true);
    canvas.println(">>IoT BASE");
    canvas.println(">>ModBus Master Test");
    canvas.pushSprite(0, 0);

    if (!ModbusRTUClient.begin(9600, SERIAL_8N1)) {
        Serial.println("Failed to start Modbus RTU Client!");
        while (1)
            ;
    }
}

void loop() {
    // for (slave) id 1: write the value of 0x01, to the coil at address 0x00
    if (!ModbusRTUClient.coilWrite(1, 0x00, 0xff)) {
        Serial.print("Failed to write coil! ");
        Serial.println(ModbusRTUClient.lastError());
    }

    // wait for 1 second
    delay(2000);
    // for (slave) id 1: write the value of 0x00, to the coil at address 0x00
    if (!ModbusRTUClient.coilWrite(1, 0x00, 0x00)) {
        Serial.print("Failed to write coil! ");
        Serial.println(ModbusRTUClient.lastError());
    }
    // wait for 1 second
    delay(2000);
}
