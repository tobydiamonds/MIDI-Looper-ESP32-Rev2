#ifndef Button_h
#define Button_h

#include <Arduino.h>

class Button
{
private:
    int _index;
    byte _state;
    String _name;
    int _value;

public:
    void (*Pressed)(Button *) = 0;
    void (*Released)(Button *) = 0;

    Button(int index, String name)
    {
        _index = index;
        _name = name;
    }

    bool IsPressed() { return _state == 1; }
    bool IsReleased() { return _state == 0; }
    int Index() { return _index; }
    void SetValue(int value) {_value = value;}
    int GetValue() {return _value;}

    void SetState(byte state)
    {
        _state = state;
        if (Pressed != 0 && _state == 1)
            Pressed(this);
        if (Released != 0 && _state == 0)
            Released(this);
    }
};

#endif