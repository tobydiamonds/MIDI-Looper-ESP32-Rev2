#ifndef MidiMessageBuilder_h
#define MidiMessageBuilder_h

#include "MidiMessage.h"
//#include "MidiParserService.h"

class MidiMessageBuilder
{
private:
    MidiMessage *_currentMessage;
    MidiMessage *_prevMessage; // used for running status

    bool _buildingMessage = false;
    int _expectedMessageLength = 0;
    int _currentMessageDataIndex = 0;

    MidiMessage *CreateMessage(byte status)
    {
        byte type = status & 0xF0; // remove the channel information
        //MidiType mt = static_cast<MidiType>(type);
        byte channel = status & 0x0F;
        MidiMessage *message;
        message = new MidiMessage(channel, type);
        message->bType = type;
        // switch (mt)
        // {
        // case MidiType::NoteOn:
        //     message = new NoteOnMessage(channel);
        //     break;
        // case MidiType::NoteOff:
        //     message = new NoteOffMessage(channel);
        //     break;
        // case MidiType::ControlChange:
        //     message = new CCMidiMessage(channel);
        //     break;
        // default:
        //     message = new DefaultMidiMessage(channel, mt);
        //     break;
        // }
        return message;
    }
    int ExpectedMessageLength(byte type)
    {
        switch (type)
        {
        // 1 byte messages
        case Start:
        case Continue:
        case Stop:
        case Clock:
        case ActiveSensing:
        case SystemReset:
        case TuneRequest:
            return 1;
            break;

            // 2 bytes messages
        case ProgramChange:
        case AfterTouchChannel:
        case TimeCodeQuarterFrame:
        case SongSelect:
            return 2;
            break;

            // 3 bytes messages
        case NoteOn:
        case NoteOff:
        case ControlChange:
        case PitchBend:
        case AfterTouchPoly:
        case SongPosition:
            return 3;
            break;

        case SystemExclusive:
            return -1;
            break;

        case InvalidType:
        default:
            return 0;
            break;
        }
    }

    //callback_function MessageBuild;

public:
    void (*MessageBuild)(int inputidx, MidiMessage *) = 0;

    // MidiMessageBuilder(callback_function callback)
    // {
    //     MessageBuild = callback;
    // }

    bool IsBuilding()
    {
        return _buildingMessage;
    }

    byte GetChanel(byte status)
    {
        // status byte: CMD 4 bits, CH 4 bits
        return status & 0x0F;
    }

    void Parse(byte data, byte inputidx)
    {

        /* if not already build message is in progress, create a new message and parse 
           if already building a message, add data to the message being build
           also take care of running status (several data pckages for one status)
            - this should lead to several message with the same status byte
         */

        // status bytes are > 0x80 - everything else is data bytes
        // char s[30];
        // sprintf(s, "##parsing data: 0x%02X", data);
        // Serial.println(s);
        if (data >= 0x80)
        {
            if (!_buildingMessage)
            {

                _currentMessage = CreateMessage(data);
                // Serial.print("#creating message: ");
                // Serial.print("type=");
                // Serial.print(_currentMessage->bType);
                // Serial.print(" channel=");
                // Serial.print(_currentMessage->Channel);

                _expectedMessageLength = ExpectedMessageLength(_currentMessage->bType);

                // Serial.print(" expected length=");
                // Serial.println(_expectedMessageLength);

                _currentMessageDataIndex = 0;
                _buildingMessage = true;
            }
            else // should we do a check for recieved bytes vs expected number of bytes?
            {
                /*
                // when anoter status byte is recieved the current message is deemed as completed
                MessageRecieved(_currentMessage);
                _currentMessage = 0;
                _currentMessageDataIndex = 0;
                _expectedMessageLength = 0;
                _buildingMessage = false;
                */
            }
        }
        else if (_currentMessage != 0 && _buildingMessage && _currentMessageDataIndex < _expectedMessageLength)
        {
            // Serial.print("#adding data to message: index=");
            // Serial.print(_currentMessageDataIndex);
            // Serial.print(" value=");
            // Serial.println(data);
            _currentMessage->AddData(data);
            _currentMessageDataIndex++;
        }
        else if (_currentMessage == 0 && _prevMessage != 0 && !_buildingMessage)
        {
            // running status - the next message arrived - this is of same type as the current message - create a similar message and reset counters
            _currentMessage = new MidiMessage(_prevMessage->Channel, _prevMessage->bType);
            _prevMessage = 0;
            _expectedMessageLength = ExpectedMessageLength(_currentMessage->bType);
            _currentMessageDataIndex = 0;
            _currentMessage->AddData(data);
            _currentMessageDataIndex++;
            _buildingMessage = true;
        }

        // handle last message from input stream
        if (_currentMessage != 0 && _buildingMessage && (_expectedMessageLength == -1 || _expectedMessageLength - 1 == _currentMessageDataIndex))
        {
            _currentMessage->Length = _currentMessageDataIndex + 1;
            // Serial.print("#completing message: length=");
            // Serial.println(_currentMessage->Length);

            //MidiParserService::MessageBuild(_currentMessage);
            MessageBuild(inputidx, _currentMessage);
            _prevMessage = new MidiMessage(_currentMessage->Channel, _currentMessage->bType);

            _currentMessage = 0;
            _currentMessageDataIndex = 0;
            _expectedMessageLength = 0;
            _buildingMessage = false;
            // Serial.println("#message completed");
            // Serial.println();
        }
    }
};

#endif