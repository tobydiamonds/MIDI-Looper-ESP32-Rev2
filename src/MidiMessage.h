#ifndef MidiMessage_h
#define MidiMessage_h

#include <Arduino.h>

//https://users.cs.cf.ac.uk/Dave.Marshall/Multimedia/node158.html
enum MidiType
{
    InvalidType = 0x00,          ///< For notifying errors
    NoteOff = 0x80,              ///< Note Off
    NoteOn = 0x90,               ///< Note On
    AfterTouchPoly = 0xA0,       ///< Polyphonic AfterTouch
    ControlChange = 0xB0,        ///< Control Change / Channel Mode
    ProgramChange = 0xC0,        ///< Program Change
    AfterTouchChannel = 0xD0,    ///< Channel (monophonic) AfterTouch
    PitchBend = 0xE0,            ///< Pitch Bend
    SystemExclusive = 0xF0,      ///< System Exclusive
    TimeCodeQuarterFrame = 0xF1, ///< System Common - MIDI Time Code Quarter Frame
    SongPosition = 0xF2,         ///< System Common - Song Position Pointer
    SongSelect = 0xF3,           ///< System Common - Song Select
    TuneRequest = 0xF6,          ///< System Common - Tune Request
    Clock = 0xF8,                ///< System Real Time - Timing Clock
    Start = 0xFA,                ///< System Real Time - Start
    Continue = 0xFB,             ///< System Real Time - Continue
    Stop = 0xFC,                 ///< System Real Time - Stop
    ActiveSensing = 0xFE,        ///< System Real Time - Active Sensing
    SystemReset = 0xFF,          ///< System Real Time - System Reset
};

class MidiMessage
{
private:
    byte _data[3] = {0, 0, 0};
    int _index;

public:
    byte Channel;
    //MidiType Type;
    byte bType;
    byte Length;
    bool IsSet;

    MidiMessage(byte channel, byte type)
    {
        Channel = channel;
        //Type = type;
        bType = type;
        Length = 0;
        _data[0] = type;
        _data[1] = 0x00;
        _data[2] = 0x00;
        _index = 1;
        IsSet = false;
    }

    //virtual void AddData(byte data) = 0;
    void AddData(byte data)
    {
        if (_index > 0 && _index < 3)
            _data[_index++] = data;
    }

    byte GetData(byte index)
    {
        if (index >= 0 && index < Length)
        {
            return _data[index];
        }
        return -1;
    }
};
/*
class DefaultMidiMessage : public MidiMessage
{
public:
    DefaultMidiMessage(byte channel, MidiType type) : MidiMessage(channel, type)
    {
    }

    void AddData(byte data)
    {
    }
};

class NoteOnMessage : public MidiMessage
{
private:
    int _index = 0;

public:
    byte Note;
    byte Velocity;

    NoteOnMessage(byte channel) : MidiMessage(channel, MidiType::NoteOn)
    {
        Note = 0;
        Velocity = 0;
    }

    void AddData(byte data)
    {
        if (_index == 0)
        {
            Note = data;
            _index++;
        }

        if (_index == 1)
        {
            Velocity = data;
            _index++;
        }
    }
};

class NoteOffMessage : public MidiMessage
{
private:
    int _index = 0;

public:
    byte Note;
    byte Velocity;

    NoteOffMessage(byte channel) : MidiMessage(channel, MidiType::NoteOff)
    {
        Note = 0;
        Velocity = 0;
    }

    void AddData(byte data)
    {
        if (_index == 0)
        {
            Note = data;
            _index++;
        }

        if (_index == 1)
        {
            Velocity = data;
            _index++;
        }
    }
};

class CCMidiMessage : public MidiMessage
{
private:
    int _index = 0;

public:
    byte CC;
    byte Value;

    CCMidiMessage(byte channel) : MidiMessage(channel, MidiType::ControlChange)
    {
        CC = 0;
        Value = 0;
    }

    void AddData(byte data)
    {
        if (_index == 0)
        {
            CC = data;
            _index++;
        }

        if (_index == 1)
        {
            Value = data;
            _index++;
        }
    }
};
*/
#endif