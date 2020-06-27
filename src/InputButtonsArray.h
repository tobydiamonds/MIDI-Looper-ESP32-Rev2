#ifndef InputButtonsArray_h
#define InputButtonsArray_h

#include "ButtonArray.h"
#include "Button.h"

#define BTN_IDX_INPUT1 7
#define BTN_IDX_INPUT2 6
#define BTN_IDX_INPUT3 5
#define BTN_IDX_INPUT4 4
#define BTN_IDX_ARM 3

class InputButtonsArray : public ButtonArray
{

public:
    InputButtonsArray(byte i2caddress) : ButtonArray(i2caddress)
    {
    }

    Button *Input1Button()
    {
        return _buttons[BTN_IDX_INPUT1];
    }

    Button *Input2Button()
    {
        return _buttons[BTN_IDX_INPUT2];
    }

    Button *Input3Button()
    {
        return _buttons[BTN_IDX_INPUT3];
    }

    Button *Input4Button()
    {
        return _buttons[BTN_IDX_INPUT4];
    }

    Button *ArmButton()
    {
        return _buttons[BTN_IDX_ARM];
    }

    bool AnyInputButtonsPressed()
    {
        return _buttons[BTN_IDX_INPUT1]->IsPressed() || _buttons[BTN_IDX_INPUT2]->IsPressed() || _buttons[BTN_IDX_INPUT3]->IsPressed() || _buttons[BTN_IDX_INPUT4]->IsPressed();
    }
};

#endif