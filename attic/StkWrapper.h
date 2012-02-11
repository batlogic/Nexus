
#pragma once

#include "tchar.h"

#include "Stk.h"
#include "Voicer.h"
#include "Messager.h"
#include "SKINI.msg"
#include "RtAudio.h"
#include "CmdLineParser.h"

#define AUDIOCHANNELS 2
#define SAMPLERATE 44100
#define NUMVOICES 8 
#define RAWWAVEPATH "D:\\Dev\\C++\\Lib\\stk-4.4.2\\rawwaves"



class StkWrapper
{
public:
	StkWrapper();
	~StkWrapper();

	int init( int argc, TCHAR* argv[] );
	void run();
	void close();
	int process( void* output, void* input, unsigned int nFrames,
			 double streamTime, RtAudioStreamStatus status );


protected:
	void parseCommandLine( int argc, TCHAR* argv[] );
	int initVoices();
	int initMidi();
	int initAudio( RtAudio::Api api, int device, stk::UINT32 buffer );
	void processMessage();

	void audioProbe();
	void midiProbe();

	stk::Voicer m_voicer;
	stk::Messager m_messager;
	stk::Skini::Message m_message;

	enum State {
		UNINITIALIZED,
		RUNNING,
		DONE
	};
	State m_state;
	RtAudio* m_audio;
	bool m_messagePending;

	stk::Instrmnt* m_instruments[NUMVOICES];
};



///////////////////////////////////////////////////////////////////////////////////
// Command line helpers
///////////////////////////////////////////////////////////////////////////////////


enum { OPT_PORT, OPT_DEVICE, OPT_BUFFER, OPT_AUDIOPROBE, OPT_MIDIPROBE, OPT_HELP };

CmdLineParser::SOption g_cmdOptions[] = 
{
    { OPT_PORT, _T("-p"),           SO_REQ_CMB }, 
    { OPT_DEVICE, _T("-d"),         SO_REQ_CMB },
	{ OPT_BUFFER, _T("-b"),         SO_REQ_CMB },
    { OPT_AUDIOPROBE, _T("-probe"), SO_NONE    }, 
    { OPT_HELP, _T("-?"),		    SO_NONE    },
    SO_END_OF_OPTIONS                    
};


class CmdLine
{
public:
	CmdLine( int argc, TCHAR* argv[] );
	void usage();

	RtAudio::Api port;
	int device;
	stk::UINT32 buffer;
	bool probe;
};


