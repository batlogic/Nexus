
#ifndef POLYPHONY_H
#define POLYPHONY_H


#include "Definitions.h"
#include "Buffer.h"
#include "ModuleData.h"


class Synth;
class Polyphony;



class MidiListener
{
public:
    MidiListener();
    virtual ~MidiListener();

    virtual void onGate( FLOAT gate, UINT16 voice ) {}
    virtual void onPitch( FLOAT pitch, UINT16 voice ) {}
    virtual void onPitchbend( UINT16 value1, UINT16 value2 ) {}
    virtual void onNoteEvent( FLOAT pitch, FLOAT gate, UINT16 voice ) {}
    virtual void onController( UINT16 controller, FLOAT value ) {}

    enum Interest { 
        Nothing    = 0x0000,
        Gate       = 0x0001,
        Pitch      = 0x0002,
        Note       = 0x0004,
        Pitchbend  = 0x0008,
        Controller = 0x00F0
    };
    UINT16 midiInterest_;
    Polyphony* midiSource_;
};
typedef set< MidiListener* > MidiListenerSet;


enum VoiceState 
{
    Silent  = 0,
    NoteOn  = 1,
    NoteOff = 2,
};


class Voice
{
public:
	Voice();
	void reset();
    void init( INT16 id, VoiceState state, FLOAT pitch, INT32 tag );

	INT16 id_;
    VoiceState state_;
    FLOAT pitch_;
	INT32 tag_;
    INT32 unisonGroup_;
};
typedef vector< Voice > VoiceList;


class Polyphony
{
public:
	Polyphony();
    ~Polyphony();

	void setNumVoices( UINT16 numVoices );
    void setSynth( Synth* synth )   { synth_ = synth; }
	bool isActive( UINT16 voice )	{ return voices_[ voice ].state_ != Silent; }

	void noteOn( UINT16 pitch, UINT16 gate );
	void noteOff( UINT16 pitch );

	void startVoice( UINT16 voice, FLOAT pitch, FLOAT gate );
	void endVoice( UINT16 voice );
    void allNotesOff();

    void attachMidiListener( MidiListener* listener );
    void detachMidiListener( MidiListener* listener );

    void notifyPitch( FLOAT pitch, UINT16 voice );
    void notifyGate( FLOAT gate, UINT16 voice );
    void notifyNote( FLOAT pitch, FLOAT gate, UINT16 voice );
    void notifyPitchbend( UINT16 value1, UINT16 value2 );
    void notifyControlChange( UINT16 eventNum, FLOAT value );

    void setNumUnison( UINT16 numUnison );
    void setUnisonSpread( UINT16 cent );
    void setRetrigger( bool retrigger ) { retrigger_ = retrigger; }
    void setLegato( bool legato );
    FLOAT getPreviousPitch( UINT16 voice );

	VoiceList voices_;
    Buffer< UINT16 > bufSoundingVoices_;
	UINT16* soundingVoices_;
	INT16 numSounding_, numActive_;
    INT16 numUnison_;
    UINT16 unisonSpread_;

protected:
    
	INT16 getUnusedVoice();
    void retrigger( FLOAT basePitch );
    void updateSoundingVoices();
	void monitorNoteEvent( UINT16 voice, FLOAT pitch, FLOAT gate ) const;
    void dumpStack( const string& msg );
	
    MidiListenerSet listeners_;
    ModuleDataList midiGates_;
    ModuleDataList midiPitches_;
    Synth* synth_;
    UINT32 tags_;
    bool retrigger_;
    bool legato_;
    FLOAT lastPitch_;
    UINT16 maxVoices_;

    vector< Voice > stack_;
};


#endif // POLYPHONY_H