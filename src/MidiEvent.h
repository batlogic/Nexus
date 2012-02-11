#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include "Definitions.h"



class MidiEvent 
{ 
protected:
	UINT8 data_[4];
	long channel_;       // The message channel (not limited to 16!).
	FLOAT time_;         // The message time stamp in seconds (delta or absolute).
	int deltaFrames_;    // Sample frames related to the current block start sample position 

public:
	MidiEvent() : channel_(0), time_(0.0), deltaFrames_(0) {}
	MidiEvent( vector<unsigned char>* bytes, double timeStamp );
	MidiEvent( char bytes[4], int deltaFrames );

	long getChannel() const			    { return channel_; }
	UINT8 getData( UINT8 index ) const	{ return data_[index]; }

	void dump() const;

	enum Types
	{
		NoteOff               = 128,
		NoteOn                = 144,
		PolyPressure          = 160,
		ControlChange         = 176,
		ProgramChange         = 192,
		AfterTouch            = 208,
		ChannelPressure       = 208,
		PitchWheel            = 224,
		PitchChange           = 49,

		Clock                 = 248,
		SongStart             = 250,
		Continue              = 251,
		SongStop              = 252,
		ActiveSensing         = 254,
		SystemReset           = 255,

		Volume                = 7,
		ModWheel              = 1,
		Breath                = 2,
		FootControl           = 4,
		Portamento            = 65,
		Balance               = 8,
		Pan                   = 10,
		Sustain               = 64,
		Expression            = 11,

		AfterTouch_Cont       = 128
	};
};


#endif // MIDIEVENT_H