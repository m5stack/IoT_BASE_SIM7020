#include <M5Stack.h>
#include <M5GFX.h>
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>
#include "IoT_BASE_SIM7020.h"


const int numCoils            = 10;
const int numDiscreteInputs   = 10;
const int numHoldingRegisters = 10;
const int numInputRegisters   = 10;

M5GFX display;
M5Canvas canvas(&display);

RS485Class RS485(Serial2, IoT_BASE_RS485_RX, IoT_BASE_RS485_TX, -1, -1);

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
    canvas.println(">>ModBus Slave Test");
    canvas.println(">>ID: 0x2A");
    canvas.pushSprite(0, 0);

    // start the Modbus RTU server, with (slave) id 42
    if (!ModbusRTUServer.begin(42, 9600)) {
        Serial.println("Failed to start Modbus RTU Server!");
        while (1)
            ;
    }
    // configure coils at address 0x00
    ModbusRTUServer.configureCoils(0x00, numCoils);
    // configure discrete inputs at address 0x00
    ModbusRTUServer.configureDiscreteInputs(0x00, numDiscreteInputs);
    // configure holding registers at address 0x00
    ModbusRTUServer.configureHoldingRegisters(0x00, numHoldingRegisters);
    // configure input registers at address 0x00
    ModbusRTUServer.configureInputRegisters(0x00, numInputRegisters);
}

void loop() {
    // poll for Modbus RTU requests
    ModbusRTUServer.poll();

    // map the coil values to the discrete input values
    for (int i = 0; i < numCoils; i++) {
        int coilValue = ModbusRTUServer.coilRead(i);

        ModbusRTUServer.discreteInputWrite(i, coilValue);
    }

    // map the holding register values to the input register values
    for (int i = 0; i < numHoldingRegisters; i++) {
        long holdingRegisterValue = ModbusRTUServer.holdingRegisterRead(i);

        ModbusRTUServer.inputRegisterWrite(i, holdingRegisterValue);
    }
}