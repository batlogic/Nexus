#ifndef MIDICONSTANTS_H
#define MIDICONSTANTS_H



/***** MIDI COMPATIBLE MESSAGES *****/
/*** (Status bytes for channel=0) ***/

enum MIDI
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
#endif