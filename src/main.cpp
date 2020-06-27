#include <Arduino.h>
#include <Wire.h>

// MIDI includes
#include "MidiDefs.h"
#include "TempoDetecter.h"
#include "MidiMessage.h"
#include "MidiMessageBuilder.h"
#include "Output.h"

// UI includes
#include "Button.h"
#include "InputButtonsArray.h"
#include "OutputButtonsArray.h"

// MIDI
#define RXD1 15
#define TXD1 4
#define RXD2 13
#define TXD2 12
#define TRACKS 4
#define CLIPS_PR_TRACK 8
#define CLIP_LENGTH 2 // how many bars to be able to store
#define OVERDUBBING_PR_CLIP 4

TempoDetector *tempoDetector = new TempoDetector(90);
SongSignature *songSignature = new SongSignature();
MidiMessageBuilder *builder1 = new MidiMessageBuilder();

u_int position = 0;
MidiMessage *messages[TRACKS][CLIPS_PR_TRACK][CLIP_LENGTH * MIDI_PPQ][OVERDUBBING_PR_CLIP];
Output *outputs[TRACKS];
bool metronomeEnabled = true;
byte metronomeChannel = 0;

u_int currentTrack = 0;
u_int currentClip = 0;

// UI

// timing

//https://sites.uci.edu/camp2014/2014/05/19/timing-in-midi-files/
int BpmToPpqHz(int bpm, int signtureBottom)
{
  /*
   60bpm = 1hz
   120bpm = 2hz 

   120 BPM, PPQ 96 => 192hz
   */
  double hz = bpm / 60.0;
  return hz * MIDI_PPQ * signtureBottom;
}

byte GetStatus(byte type, byte channel)
{
  //return ((byte)type | ((channel - 1) & 0x0F));
  return type | channel;
}

byte metronomeNote = 0x00;
void SoundMetronome(u_int position)
{
  if(position == 0)
  {
    byte status = GetStatus(MidiType::NoteOn, metronomeChannel);
    Serial1.write(status);
    Serial1.write(0x4b); // Eb
    Serial1.write(0x64);
  }
  else if(position % (songSignature->Bottom * MIDI_PPQ) == 0)
  {
    byte status = GetStatus(MidiType::NoteOn, metronomeChannel);
    Serial1.write(status);
    Serial1.write(0x48); // C
    Serial1.write(0x64);
  }
}

void NoSoundMetronome(u_int position)
{
  if (position == 8)
  {
    byte status = GetStatus(MidiType::NoteOff, metronomeChannel);
    Serial1.write(status);
    Serial1.write(0x4b); // Eb
    Serial1.write(0x64);
  }
  if (position % (songSignature->Bottom * MIDI_PPQ + 8) == 0)
  {
    byte status = GetStatus(MidiType::NoteOff, metronomeChannel);
    Serial1.write(status);
    Serial1.write(0x48); // C
    Serial1.write(0x64);
  }
}

hw_timer_t *timer0 = NULL;
portMUX_TYPE timer0Mux = portMUX_INITIALIZER_UNLOCKED;
void IRAM_ATTR onTimer0()
{
  portENTER_CRITICAL_ISR(&timer0Mux);
  //clipManager->Tick(position);

  if (position < songSignature->Bottom * MIDI_PPQ * 4) // 4 bars?
    position++;
  else
  {
    position = 0;
  }

  if (metronomeEnabled)
  {
    SoundMetronome(position);
    NoSoundMetronome(position);
  }

  portEXIT_CRITICAL_ISR(&timer0Mux);
}

void OnMidiMessageRecieved(int inputidx, MidiMessage *message)
{
  // char s[50];
  // sprintf(s, "in:  0x%02X 0x%02X 0x%02X port:%d", message->Type, message->Data1, message->Data2, inputidx);
  // Serial.println(s);

  messages[currentTrack][currentClip][position][0]->Type = message->Type;
  messages[currentTrack][currentClip][position][0]->Data1 = message->Data1;
  messages[currentTrack][currentClip][position][0]->Data2 = message->Data2;

  for (int t = 0; t < TRACKS; t++)
  {
    Output *output = outputs[t];

    if (output->IsEnabled || message->Type == 0x80) // send note off even though output is disbaled - to ensure that notes end even while playing when the disable happens
    {
      byte status = GetStatus(message->Type, output->Channel);
      byte messageLength = builder1->ExpectedMessageLength(message->Type);
      Serial1.write(status);
      if (messageLength >= 1)
      {
        Serial1.write(message->Data1);
      }
      if (messageLength >= 2)
      {
        Serial1.write(message->Data2);
      }
    }
  }
}

void setup()
{
  // serial
  Serial.begin(115200);

  Serial1.begin(31250, SERIAL_8N1, RXD1, TXD1);
  Serial2.begin(31250, SERIAL_8N1, RXD2, TXD2);

  // init message data array
  for (int t = 0; t < TRACKS; t++)
  {
    for (int c = 0; c < CLIPS_PR_TRACK; c++)
    {
      for (int p = 0; p < CLIP_LENGTH * MIDI_PPQ; p++)
      {
        for (int o = 0; o < OVERDUBBING_PR_CLIP; o++)
        {
          messages[t][c][p][o] = new MidiMessage(); // the struct will init all props to 0x00
        }
      }
    }
  }
  // init outputs
  for (int t = 0; t < TRACKS; t++)
  {
    outputs[t] = new Output(t); // midi channels are zero-based!?! so value 0 => channel 1
  }

  songSignature = tempoDetector->GetSongSignature();
  builder1->MessageBuild = OnMidiMessageRecieved;

  // since timer-divider is set to 80, the timer will run 1.000.000 times pr second
  unsigned long pre = 1000000 / BpmToPpqHz(tempoDetector->GetBPM(), songSignature->Bottom);
  Serial.print("prescalar: ");
  Serial.println(pre);

  // setup timers
  timer0 = timerBegin(0, 80, true);
  timerAttachInterrupt(timer0, &onTimer0, true);
  timerAlarmWrite(timer0, pre, true);
  timerAlarmEnable(timer0);
}

void loop()
{
  if (Serial1.available())
  {
    while (Serial1.available())
    {
      builder1->Parse(Serial1.read(), 0);
    }
  }
}