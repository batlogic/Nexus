
#include "SoundDevices.h"


#ifdef _RT_AUDIO
#include "Synth.h"


//------------------------------------------------------
// class MidiPortMap
//------------------------------------------------------

string MidiPortMap::getPortName( UINT16 portNum )
{
    for( iterator it=begin(); it!=end(); it++ )
    {
        if( it->second == portNum ) {
            return it->first;
        }
    }
    return "";
}


INT16 MidiPortMap::getPortNumber( const string& portName )
{
    iterator pos = find( portName );
    return pos != end() ? pos->second : -1;
}


bool MidiPortMap::exists( const string& portName )
{
    iterator pos = find( portName );
    return pos != end();
}


void MidiPortMap::remove( const string& portName )
{
    iterator pos = find( portName );
    if( pos != end() ) {
        erase( pos );
    }
}



//------------------------------------------------------
// class SoundOptions
//------------------------------------------------------

SoundOptions::SoundOptions() :
    audioPort_( RtAudio::WINDOWS_ASIO ),
    audioOutChannel_( 0 ),
    bufferSize_( 512 ),
    sampleRate_( 44100 ),
    audioRun_( true )
    {}



//------------------------------------------------------
// class SoundDevices
//------------------------------------------------------

SoundDevices::SoundDevices() :
    audio_( NULL ),
    midiIn_( NULL ),
    synth_( NULL ),
    options_( NULL ),
    running_( false ),
    bufferSize_( 0 ),
    latency_( 0 ),
    state_( NotReady )
{}
    

void SoundDevices::open()
{
    ASSERT( synth_ );
    ASSERT( options_ );

    getMidiDeviceInfos();
    openMidiIn();
    openPort( options_->audioPort_ );
    openStream();
    resume();
}


void SoundDevices::close()
{
	suspend();

    if( midiIn_ != NULL ) {
        midiIn_->closeAllPorts();
	    delete midiIn_;
        midiIn_ = NULL;
    }
	if( audio_ != NULL ) {
		delete audio_;
        audio_ = NULL;
	}
    latency_ = 0;
    state_   = NotReady;
}


void SoundDevices::reset()
{
    options_->audioOutDevice_  = "";
    options_->audioOutChannel_ = 0;
    closeStream();
}


int SoundDevices::process( 
    void* output, 
    void* input, 
    unsigned int numFrames,
    double streamTime, 
    RtAudioStreamStatus status, 
    void* data )
{
	FLOAT* out1   = (FLOAT*)output;	    // left channel
	FLOAT* out2   = out1 + numFrames;   // right channel
	FLOAT* out[2] = { out1, out2 };

    SoundDevices* dev = (SoundDevices*)data;
    dev->synth_->processReplacing( NULL, out, numFrames );

	while( dev->running_ && dev->midiIn_->popEvent( dev->midiEvent_ )) 
	{
		dev->synth_->processEvent( dev->midiEvent_ );
	}
	return 0;
}


void SoundDevices::openPort( AudioPort port )
{
    options_->audioPort_ = port;

    if( options_->audioRun_ )
    {
        if( audio_ != NULL ) {
            delete audio_;
        }
        audio_ = new RtAudio( port );
        getDeviceInfos( false );
    }
}


void SoundDevices::openStream()
{
    state_      = NotReady;
    latency_    = 0;
    bufferSize_ = options_->bufferSize_;
    

    if( options_->audioRun_ == false )
        return;
    
    INT16 deviceId = getDeviceNumber( options_->audioOutDevice_ );
    if( audio_  && deviceId > -1 )
    {
        audio_->showWarnings( true );

	    RtAudio::StreamParameters params;
	    params.deviceId     = deviceId;
	    params.nChannels    = NUMOUTPUTS;
        params.firstChannel = options_->audioOutChannel_;

	    RtAudio::StreamOptions options;
	    options.flags |= RTAUDIO_SCHEDULE_REALTIME;
	    options.flags |= RTAUDIO_MINIMIZE_LATENCY;
	    options.flags |= RTAUDIO_NONINTERLEAVED;

	    try {
		    audio_->openStream( 
                &params, 
                NULL, 
                sizeof( FLOAT ) == 8 ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32, 
                options_->sampleRate_, 
                &bufferSize_, 
                &process, 
                this, 
                &options );
	    }
	    catch( const RtError& e ) {
            TRACE( e.what() );
            options_->audioOutDevice_ = "";
		    return;
	    }
	    try { 
		    audio_->startStream(); 
	    }
	    catch( const RtError &e ) {
		    TRACE( e.what() );
	    }
        latency_ = audio_->getStreamLatency();
        state_  = Ready;
    }
}


void SoundDevices::closeStream()
{
    if( audio_ )
        if( audio_->isStreamOpen() ) 
            audio_->closeStream();

    state_ = NotReady;
}


void SoundDevices::getDeviceInfos( bool inputs )
{
    deviceInfos_.clear();
    if( audio_ )
    {
        UINT16 numDevices = audio_->getDeviceCount();

	    for( unsigned int i=0; i<numDevices; i++ ) 
        {
		    try {
                DeviceInfo info = audio_->getDeviceInfo( i );

                if( info.isInput && info.isOutput || 
                    info.isOutput && !info.isInput && inputs == false ||
                    !info.isOutput && info.isInput && inputs == true
                   ) {
                    deviceInfos_.push_back( info );
                }
            }
            catch( const exception& e ) {
                TRACE( e.what() );
            }
	    }

        // check if device in options exists
        INT16 deviceNum = getDeviceNumber( options_->audioOutDevice_ );
        if( deviceNum == -1 ) {
            options_->audioOutDevice_ = "";
        }
    }
}


void SoundDevices::getMidiDeviceInfos()
{
	RtMidiIn* midiin = 0;
	RtMidiOut* midiout = 0;

	// MidiIn
	try {
		midiin = new RtMidiIn();
	}
	catch ( const RtError& e ) {
		TRACE( e.what() );
	}

	// Check inputs.
	unsigned int nPorts = midiin->getPortCount();
	string portName;
	unsigned int i;
	for( i=0; i<nPorts; i++ ) {
		try {
			portName = midiin->getPortName(i);
            midiInInfo_.insert( MidiPortMap::value_type( portName, i ));
		}
		catch ( const RtError& e ) {
			TRACE( e.what() );
		}
	}

	// MidiOut
	try {
		midiout = new RtMidiOut();
	}
	catch ( const exception &e ) {
		TRACE( e.what() );
	}

	// Check outputs.
	nPorts = midiout->getPortCount();
	for ( i=0; i<nPorts; i++ ) {
		try {
			portName = midiout->getPortName( i );
            midiOutInfo_.insert( MidiPortMap::value_type( portName, i ));
		}
		catch ( const exception& e ) {
            TRACE( e.what() );
		}
	}
	if( midiin != NULL ) delete midiin;
	if( midiout != NULL ) delete midiout;

    // check if options contain available ports and assign port number
    for( MidiPortMap::iterator it=options_->midiInPorts_.begin(); it!=options_->midiInPorts_.end(); )
    {
		MidiPortMap::iterator pos = midiInInfo_.find( it->first );
        if( pos != midiInInfo_.end() )
        {
            it->second = pos->second;
            it++;
        }
        else {
            options_->midiInPorts_.erase( it++ );
        }
    }
}


INT16 SoundDevices::getDeviceNumber( const string& deviceName )
{
    for( UINT16 i=0; i<deviceInfos_.size(); i++ )
    {
        DeviceInfo& info = deviceInfos_[ i ];
        if( info.name == deviceName ) {
            return i;
        }
    }
    return -1;
}


string SoundDevices::getDeviceName( INT16 deviceId )
{
    return ( deviceId >= 0 && deviceId < (INT16)deviceInfos_.size()) ? deviceInfos_[ deviceId ].name : "";
}


void SoundDevices::setAudioOutDevice( INT16 id )
{
    string deviceName = getDeviceName( id );
    if( options_->audioOutDevice_ != deviceName )
    {
        options_->audioOutDevice_ = deviceName;
        closeStream();
        openStream();
    }
}


DeviceInfo* SoundDevices::getAudioOutDevice()
{
    INT16 id = getDeviceNumber( options_->audioOutDevice_ );
    return ( id >= 0 && id < (INT16)deviceInfos_.size()) ? &deviceInfos_[ id ] : NULL;
}


void SoundDevices::setAudioOutChannel( INT16 id )
{
    if( options_->audioOutChannel_ != id )
    {
        options_->audioOutChannel_ = id;
        closeStream();
        openStream();
    }
}


void SoundDevices::setBufferSize( UINT32 bufferSize, bool apply )
{
    if( options_->bufferSize_ != bufferSize )
    {
        options_->bufferSize_ = bufferSize;

        if( apply ) {
            closeStream();
            openStream();
        }
    }
}


void SoundDevices::setSampleRate( UINT32 sampleRate )
{
    if( options_->sampleRate_ != sampleRate )
    {
        options_->sampleRate_ = sampleRate;
        closeStream();
        synth_->setSampleRate( sampleRate );
        openStream();
    }
}


void SoundDevices::openMidiIn()
{
    midiIn_ = new RtMidiInput();

    for( MidiPortMap::iterator it=options_->midiInPorts_.begin(); it!=options_->midiInPorts_.end(); )
    {
        ASSERT( it->second >= 0 );
        if( midiIn_->openPort( it->second ) == false ) {
            options_->midiInPorts_.erase( it++ );
        }
        else it++;
    }
}


void SoundDevices::openMidiInPort( const string& portName )
{
    INT16 portNum = midiInInfo_.getPortNumber( portName );
    if( midiIn_->isPortOpen( portNum ) == false ) 
    {
        if( midiIn_->openPort( portNum ) == true ) {
            options_->midiInPorts_[ portName ] = portNum;
        } else {
            options_->midiInPorts_.remove( portName );
        }
    }
}


void SoundDevices::closeMidiInPort( const string& portName )
{
    INT16 portNum = midiInInfo_.getPortNumber( portName );

    if( midiIn_->isPortOpen( portNum ) == true ) 
    {
        midiIn_->closePort( portNum );
        options_->midiInPorts_.remove( portName );
    }
}


bool SoundDevices::midiInPortIsOpen( const string& portName )
{
    return options_->midiInPorts_.exists( portName );
}


#endif // _RT_AUDIO