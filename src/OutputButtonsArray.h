#ifndef OutputButtonsArray_h
#define OutputButtonsArray_h

#include "ButtonArray.h"
#include "Button.h"

#define BTN_IDX_OUTPUT1 7
#define BTN_IDX_OUTPUT2 6
#define BTN_IDX_OUTPUT3 5
#define BTN_IDX_OUTPUT4 4
#define BTN_IDX_OUTPUT5 3

class OutputButtonsArray : public ButtonArray
{

public:
    OutputButtonsArray(byte i2caddress) : ButtonArray(i2caddress)
    {
        _buttons[BTN_IDX_OUTPUT1]->SetValue(0); // output 0
        _buttons[BTN_IDX_OUTPUT2]->SetValue(1);
        _buttons[BTN_IDX_OUTPUT3]->SetValue(2);
        _buttons[BTN_IDX_OUTPUT4]->SetValue(3);
        _buttons[BTN_IDX_OUTPUT5]->SetValue(4);
    }

    Button *Output1Button()
    {
        return _buttons[BTN_IDX_OUTPUT1];
    }
    Button *Output2Button()
    {
        return _buttons[BTN_IDX_OUTPUT2];
    }
    Button *Output3Button()
    {
        return _buttons[BTN_IDX_OUTPUT3];
    }
    Button *Output4Button()
    {
        return _buttons[BTN_IDX_OUTPUT4];
    }
    Button *Output5Button()
    {
        return _buttons[BTN_IDX_OUTPUT5];
    }
};

#endif