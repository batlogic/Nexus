
#include <map>
#include <signal.h>

#include "Stk.h"
#include "StkWrapper.h"
#include "RtMidi.h"
//#include "BeeThree.h"
//#include "Moog.h"
//#include "SineWave.h"
//#include "Resonate.h"
#include "Simple.h"


using namespace stk;


StkWrapper* theApp;



static void close( int ignore )
{ 
	theApp->close(); 
}


int _tmain( int argc, TCHAR* argv[] ) 
{
	setlocale( LC_ALL, "");
	(void)signal( SIGINT, close );

	theApp = new StkWrapper();
	theApp->init( argc, argv );
	theApp->run();

	delete theApp;

	return 0;
}


int process( void* output, void* input, unsigned int numFrames,
			 double streamTime, RtAudioStreamStatus status, void* data )
{
	return theApp->process( output, input, numFrames, streamTime, status );
}



StkWrapper::StkWrapper() : 
	m_state( UNINITIALIZED ),
	m_audio( NULL ),
	m_messagePending( false )
{
}


StkWrapper::~StkWrapper()
{
	for( int i=0; i<NUMVOICES; i++ ) 
	{
		if( m_instruments[i] != NULL) {
			delete m_instruments[i];
		}
	}
	if( m_audio != NULL ) {
		m_audio->closeStream(); 
		delete m_audio;
	}
}


int StkWrapper::init( int argc, TCHAR* argv[] )
{
	Stk::setSampleRate( SAMPLERATE );
	Stk::setRawwavePath( RAWWAVEPATH );

	m_state = UNINITIALIZED;
	CmdLine args( argc, argv );

	if( initVoices() != 0) {
		return 1;
	}
	if( initMidi() != 0) {
		return 1;
	}
	if( initAudio( args.port, args.device, args.buffer ) != 0 ) {
		return 1;
	}

	if( args.probe == true ) 
	{ 
		midiProbe();
		audioProbe();
		m_state = DONE;
	}
	return 0;
}


int StkWrapper::initVoices()
{
	try {
		for( int i=0; i<NUMVOICES; i++ ) 
		{
			m_instruments[i] = NULL;
			m_instruments[i] = new Simple();
			m_voicer.addInstrument( m_instruments[i] );
		}
	}
	catch ( StkError& e ) {
		e.printMessage();
		return 1;
	}
	return 0;
}


int StkWrapper::initMidi()
{
	if( m_messager.startMidiInput() == false ) {
		return 1;
	}
	return 0;
}


int StkWrapper::initAudio( RtAudio::Api api, int device, stk::UINT32 bufferSize )
{
	m_audio = new RtAudio( api );
	m_audio->showWarnings( true );

	RtAudio::StreamParameters params;
	params.deviceId = device;
	params.nChannels = AUDIOCHANNELS;

	RtAudio::StreamOptions options;
	options.flags |= RTAUDIO_SCHEDULE_REALTIME;
	options.flags |= RTAUDIO_MINIMIZE_LATENCY;

	RtAudioFormat format = ( sizeof(StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;

	try {
		m_audio->openStream( &params, NULL, format, SAMPLERATE, &bufferSize, &::process, NULL, &options );
	}
	catch( RtError& e ) {
		e.printMessage();
		return 1;
	}

	try { 
		m_audio->startStream(); 
	}
	catch( RtError &e ) {
		e.printMessage();
		return 1;
	}
	return 0;
}


void StkWrapper::run()
{
	m_state = RUNNING;

	// Block waiting until SIGINT
	while ( m_state != DONE ) {
		Stk::sleep( 100 );
	}
}


void StkWrapper::close()
{
	m_state = DONE;
}



int StkWrapper::process( void* output, void* input, unsigned int numFrames,
			 double streamTime, RtAudioStreamStatus status )
{
	register StkFloat* out = (StkFloat*)output;	
	register float tick;
	unsigned int i;

	for( i=0; i<numFrames && m_state == RUNNING; i++ ) 
	{
		if( m_messagePending == false ) 
		{
			m_messager.popMessage( m_message );
			if( m_message.type > 0 ) {
				m_messagePending = true;
			}
		}
		
		tick = (float)m_voicer.tick();

		*out++ = tick;
		*out++ = tick;
	}

	//memset( out1, 0, numFrames* sizeof( float ));
	//memset( out2, 0, numFrames* sizeof( float ));

	if( m_messagePending == true ) {
		processMessage();
	}
	return 0;
}


void StkWrapper::processMessage()
{
	register StkFloat value1 = m_message.floatValues[0];
	register StkFloat value2 = m_message.floatValues[1];

	switch( m_message.type ) 
	{
	case __SK_Exit_:
		m_state = DONE;
		return;
	case __SK_NoteOn_:
		if ( value2 == 0.0 ) // velocity is zero ... really a NoteOff
			m_voicer.noteOff( value1, 64.0 );
		else { // a NoteOn
			m_voicer.noteOn( value1, value2 );
		}
		break;

	case __SK_NoteOff_:
		m_voicer.noteOff( value1, value2 );
		break;

	case __SK_ControlChange_:
		m_voicer.controlChange( (int)value1, value2 );
		break;

	case __SK_AfterTouch_:
		m_voicer.controlChange( 128, value1 );

	case __SK_PitchChange_:
		m_voicer.setFrequency( value1 );
		break;

	case __SK_PitchBend_:
		m_voicer.pitchBend( value1 );
		break;
	}

	m_messagePending = false;
}




void StkWrapper::audioProbe()
{
	// Create an api map.
	std::map<int, std::string> apiMap;
	apiMap[RtAudio::MACOSX_CORE] = "OS-X Core Audio";
	apiMap[RtAudio::WINDOWS_ASIO] = "Windows ASIO";
	apiMap[RtAudio::WINDOWS_DS] = "Windows Direct Sound";
	apiMap[RtAudio::UNIX_JACK] = "Jack Client";
	apiMap[RtAudio::LINUX_ALSA] = "Linux ALSA";
	apiMap[RtAudio::LINUX_OSS] = "Linux OSS";
	apiMap[RtAudio::RTAUDIO_DUMMY] = "RtAudio Dummy";

	std::vector< RtAudio::Api > apis;
	RtAudio :: getCompiledApi( apis );

	std::cout << "\nCompiled APIs:\n";
	for ( unsigned int i=0; i<apis.size(); i++ )
		std::cout << "  " << apiMap[ apis[i] ] << std::endl;

	RtAudio::DeviceInfo info;

	std::cout << "\nCurrent API: " << apiMap[ m_audio->getCurrentApi() ] << std::endl;

	unsigned int devices = m_audio->getDeviceCount();
	std::cout << "\nFound " << devices << " device(s) ...\n";

	for( unsigned int i=0; i<devices; i++ ) 
	{
		info = m_audio->getDeviceInfo(i);

		std::cout << "\nDevice Name = " << info.name << '\n';
		if( info.probed == false )
			std::cout << "Probe Status = Unsuccessful\n";
		else {
			std::cout << "Probe Status = Successful\n";
			std::cout << "Output Channels = " << info.outputChannels << '\n';
			std::cout << "Input Channels = " << info.inputChannels << '\n';
			std::cout << "Duplex Channels = " << info.duplexChannels << '\n';
			if ( info.isDefaultOutput ) std::cout << "This is the default output device.\n";
			else std::cout << "This is NOT the default output device.\n";
			if ( info.isDefaultInput ) std::cout << "This is the default input device.\n";
			else std::cout << "This is NOT the default input device.\n";
			if ( info.nativeFormats == 0 )
				std::cout << "No natively supported data formats(?)!";
			else {
				std::cout << "Natively supported data formats:\n";
				if ( info.nativeFormats & RTAUDIO_SINT8 )
					std::cout << "  8-bit int\n";
				if ( info.nativeFormats & RTAUDIO_SINT16 )
					std::cout << "  16-bit int\n";
				if ( info.nativeFormats & RTAUDIO_SINT24 )
					std::cout << "  24-bit int\n";
				if ( info.nativeFormats & RTAUDIO_SINT32 )
					std::cout << "  32-bit int\n";
				if ( info.nativeFormats & RTAUDIO_FLOAT32 )
					std::cout << "  32-bit float\n";
				if ( info.nativeFormats & RTAUDIO_FLOAT64 )
					std::cout << "  64-bit float\n";
			}
			if ( info.sampleRates.size() < 1 )
				std::cout << "No supported sample rates found!";
			else {
				std::cout << "Supported sample rates = ";
				for (unsigned int j=0; j<info.sampleRates.size(); j++)
					std::cout << info.sampleRates[j] << " ";
			}
			std::cout << std::endl;
		}
	}
	std::cout << std::endl;
}


void StkWrapper::midiProbe()
{
	RtMidiIn* midiin = 0;
	RtMidiOut* midiout = 0;

	// MidiIn
	try {
		midiin = new RtMidiIn();
	}
	catch ( RtError &error ) {
		error.printMessage();
	}

	// Check inputs.
	unsigned int nPorts = midiin->getPortCount();
	std::cout << "\n" << nPorts << " MIDI input sources available:\n";
	std::string portName;
	unsigned int i;
	for( i=0; i<nPorts; i++ ) {
		try {
			portName = midiin->getPortName(i);
		}
		catch ( RtError &error ) {
			error.printMessage();
		}
		std::cout << "  Input Port #" << i+1 << ": " << portName << '\n';
	}

	// MidiOut
	try {
		midiout = new RtMidiOut();
	}
	catch ( RtError &error ) {
		error.printMessage();
	}

	// Check outputs.
	nPorts = midiout->getPortCount();
	std::cout << "\n" << nPorts << " MIDI output ports available:\n";
	for ( i=0; i<nPorts; i++ ) {
		try {
			portName = midiout->getPortName(i);
		}
		catch ( RtError &error ) {
			error.printMessage();
		}
		std::cout << "  Output Port #" << i+1 << ": " << portName << '\n';
	}
	std::cout << '\n';

	if( midiin == NULL ) delete midiin;
	if( midiout == NULL ) delete midiout;
}


#pragma region CommandLine
////////////////////////////////////////////////////////////////////
// Command Line
////////////////////////////////////////////////////////////////////


void CmdLine::usage(void) 
{
	std::cout << "\nUsage: ";
	std::cout << "STK_synth [options]\n";
	std::cout << "-p:     [asio|ds] audio port\n";
	std::cout << "-d:     [0..n] audio device\n";
	std::cout << "-b:     [0..n] audio buffer size\n";
	std::cout << "-probe: audio/midi probe\n";
}


CmdLine::CmdLine( int argc, TCHAR* argv[] )
	: port( RtAudio::UNSPECIFIED ),
	  device( 0 ),
	  buffer( 0 ),
	  probe( false )
{
	CmdLineParser args( argc, argv, g_cmdOptions );
	
    while( args.Next() ) 
	{ 
        if( args.LastError() == SO_SUCCESS ) 
		{
            if( args.OptionId() == OPT_HELP ) {
                usage();
                return;
            }
			if( _tcscmp( args.OptionText(), _T("-p")) == 0 )  
			{
				if( _tcsicmp( args.OptionArg(), _T("asio")) == 0 ) port = RtAudio::WINDOWS_ASIO;
				if( _tcsicmp( args.OptionArg(), _T("ds")) == 0 ) port = RtAudio::WINDOWS_DS;
			}
			if( _tcscmp( args.OptionText(), _T("-d")) == 0 )  
			{
				device = _tstoi( args.OptionArg() );
			}
			if( _tcscmp( args.OptionText(), _T("-b")) == 0 )  
			{
				buffer = _tstoi( args.OptionArg() );
			}
			if( _tcscmp( args.OptionText(), _T("-probe")) == 0 )  
			{
				probe = true;
			}
        }
        else {
			std::cout <<  "Invalid argument: " << args.OptionText() << "\n";
            return;
        }
    }
}
#pragma endregion


