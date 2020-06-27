#ifndef ButtonArray_h
#define ButtonArray_h

#include <Arduino.h>
#include <Wire.h>

#include "Button.h"

const int IODIRA = 0x00;
const int IODIRB = 0x01;
const int IPOLA = 0x02;
const int IPOLB = 0x03;
const int GPINTENA = 0x04;
const int GPINTENB = 0x05;
const int DEFVALA = 0x06;
const int DEFVALB = 0x07;
const int INTCONA = 0x08;
const int INTCONB = 0x09;
const int IOCONA = 0x0A;
const int IOCONB = 0x0B;
const int GPPUA = 0x0C;
const int GPPUB = 0x0D;
const int INTFA = 0x0E;
const int INTFB = 0x0F;
const int INTCAPA = 0x10;
const int INTCAPB = 0x11;
const int GPIOA = 0x12;
const int GPIOB = 0x13;
const int OLATA = 0x14;
const int OLATB = 0x15;

#define BUTTON_ARRAY_MAX_SIZE 8

class ButtonArray
{
private:
    int _i2caddress;
    bool _hasInterrupt;
    bool _interruptHandled;

    unsigned long _readintervall = 1000;
    unsigned long _prevread = 0;

    byte McpReadRegister(byte reg)
    {
        Wire.beginTransmission(_i2caddress);
        Wire.write(reg);
        Wire.endTransmission();
        Wire.requestFrom(_i2caddress, (int)1);
        return Wire.read();
    }

    void McpWriteRegister(byte reg, byte value)
    {
        Wire.beginTransmission(_i2caddress);
        Wire.write(reg);
        Wire.write(value);
        Wire.endTransmission();
    }

    byte BitPosition(int value)
    {
        return log(value) / log(2);
    }

protected:
    Button *_buttons[BUTTON_ARRAY_MAX_SIZE];

public:
    ButtonArray(int i2caddress)
    {
        _i2caddress = i2caddress;
        _hasInterrupt = false;
        _interruptHandled = true;

        for (int i = 0; i < BUTTON_ARRAY_MAX_SIZE; i++)
        {
            char name[50];
            sprintf(name, "array%#x::button%#x", _i2caddress, i);
            _buttons[i] = new Button(i, name);
        }
    }

    void Initialize()
    {
        // setup port A as input
        McpWriteRegister(IODIRA, 0xff); // all pins are inputs
        McpWriteRegister(GPPUA, 0xff);  // enable pull-ups for all pins

        // setup port A interrupt
        byte iocon = McpReadRegister(IOCONA);
        bitWrite(iocon, 7, 0); // set bank mode = 0
        bitWrite(iocon, 6, 0); // set mirroring off
        bitWrite(iocon, 2, 0); // set open-drain to active driver output (configured in polarity bit (bit 1))
        bitWrite(iocon, 1, 0); // set the int polarity to LOW
        McpWriteRegister(IOCONA, iocon);

        McpWriteRegister(INTCONA, 0x00);  // setup all pins to trigger the interrupt when its value changes
        McpWriteRegister(GPINTENA, 0xff); // enable interrupt for all pins
    }

    Button **Buttons()
    {
        return _buttons;
    }

    void SetInterrupt()
    {
        _hasInterrupt = true;
        _interruptHandled = false;
    }

    int Run()
    {
        int result = 0;
        if (_hasInterrupt && !_interruptHandled)
        {
            _interruptHandled = true;
            byte intfa = McpReadRegister(INTFA);
            byte intcapa = McpReadRegister(INTCAPA);

            byte idx = BitPosition(intfa);
            if (idx >= BUTTON_ARRAY_MAX_SIZE)
                return -1;

            byte isset = (intfa & intcapa) == 0;
            if (isset)
                _buttons[idx]->SetState(1);
            else
                _buttons[idx]->SetState(0);

            _hasInterrupt = false;

            result = idx;
        }
        else
        {
            result = 0;
        }

        unsigned long now = millis();
        if (now > (_prevread + _readintervall))
        {
            _prevread = now;
            McpReadRegister(GPIOA);
        }

        return result;
    }
};

#endif