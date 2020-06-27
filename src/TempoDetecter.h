#ifndef TempoDetector_h
#define TempoDetector_h

class SongSignature
{
public:
    int Top;
    int Bottom;
};

class TempoDetector
{
private:
    int _bpm;

public:
    TempoDetector(int bpm)
    {
        _bpm = bpm;
    }

    int GetBPM()
    {
        return _bpm;
    }

    SongSignature *GetSongSignature()
    {
        SongSignature *signature = new SongSignature();
        signature->Top = 4;
        signature->Bottom = 4;
        return signature;
    }
};

#endif