#ifndef Output_h
#define Output_h

#include <Arduino.h>

struct Output
{
    byte Channel;
    bool IsEnabled;

    Output(byte channel)
    {
        Channel = channel;
        IsEnabled = true;
    }
};

#endif