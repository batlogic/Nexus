
#ifndef MIDIMODULES_H
#define MIDIMODULES_H

#include "Module.h"
#include "Buffer.h"



//--------------------------------------------------------
// class MidiGate
//--------------------------------------------------------

class MidiGate : public Module
{
public:
	MidiGate();

	void init( ModuleData* data, Synth* synth );
    void onNoteEvent( FLOAT pitch, FLOAT gate, UINT16 voice );

protected:
	EventOut eventOut_;
};



//--------------------------------------------------------
// class MidiPitch
//--------------------------------------------------------

class MidiPitch : public Module
{
public:
	MidiPitch();

    void initData();
    void setParameter( UINT16 paramId, FLOAT value, FLOAT modulation=0.f, INT16 voice=-1 );
    void onNoteEvent( FLOAT pitch, FLOAT gate, UINT16 voice );
    void onPitchbend( UINT16 value1, UINT16 value2 );
    void processControl() throw();

protected:
    void calcGlide( FLOAT freq, UINT16 voice );
    void setGlideTime( FLOAT time );

    enum Params { 
        PARAM_BEND_RANGE = 0,
        PARAM_GLIDE_TIME = 1,
        PARAM_GLIDE_AUTO = 2,
    };

    Buffer< FLOAT > bufGlideDelta_, bufGlideTarget_, bufFreq_;
    FLOAT *freq_, *glideTarget_, *glideDelta_;

    FLOAT glideFrames_;
    bool glideAuto_;

    FLOAT bendFactor_;
    INT16 bendRange_;

	EventOut freqOut_;
};



//--------------------------------------------------------
// class MidiInput
//--------------------------------------------------------

class MidiInput : public MidiPitch
{
public:
    MidiInput();
    void onNoteEvent( FLOAT pitch, FLOAT gate, UINT16 voice );

protected:
    EventOut gateOut_;
};



#endif // MIDIMODULES_H