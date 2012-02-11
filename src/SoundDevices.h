
#ifndef SOUNDDEVICES_H
#define SOUNDDEVICES_H


class AppOptions;
class Synth;

#ifdef _RT_AUDIO

#include "RT/RtAudio.h"
#include "RT/RtMidiInput.h"


typedef RtAudio::Api AudioPort;
typedef RtAudio::DeviceInfo DeviceInfo;
typedef vector< DeviceInfo > DeviceInfoList;


class MidiPortMap : public map< string, INT16 >
{
public:
    string getPortName( UINT16 portNum );
    INT16 getPortNumber( const string& portName );
    bool exists( const string& portName );
    void remove( const string& portName );
};


class SoundOptions
{
public:
    SoundOptions();

    AudioPort audioPort_;
    string audioOutDevice_;
    INT16 audioOutChannel_;
    MidiPortMap midiInPorts_;
    UINT32 bufferSize_;
    UINT32 sampleRate_;
    bool audioRun_;
};


class SoundDevices
{
public:
    SoundDevices();

    void open();
    void openPort( AudioPort port );
    void close();
    void reset();
    void suspend() { running_ = false; }
    void resume() { running_ = true; }
    void getDeviceInfos( bool inputs );
    void getMidiDeviceInfos();

    AudioPort getAudioPort()    { return options_->audioPort_; }
    
    void setAudioOutDevice( INT16 id );
    DeviceInfo* getAudioOutDevice();

    void setAudioOutChannel( INT16 id );
    INT16 getAudioOutChannel() { return options_->audioOutChannel_; }

    void setBufferSize( UINT32 bufferSize, bool apply );
    UINT32 getBufferSize() const { return options_->bufferSize_; }

    void setSampleRate( UINT32 sampleRate );
    UINT32 getSampleRate()  const { return options_->sampleRate_; }

    long getLatency() const { return latency_; }

    void openMidiInPort( const string& portName );
    void closeMidiInPort( const string& portName );
    bool midiInPortIsOpen( const string& portName );

	static int process( void* output, void* input, unsigned int numFrames,
						double streamTime, RtAudioStreamStatus status, void* data );

    void setSynth( Synth* synth )               { synth_ = synth; }
    void setOptions( SoundOptions* options )    { options_ = options; }
    bool isStandalone()                         { return true; }

    enum State { NotReady, Ready };
    UINT16 getState()                           { return state_; }

    DeviceInfoList deviceInfos_;
    MidiPortMap midiInInfo_, midiOutInfo_;

protected:
    void openMidiIn();
    void openStream();
    void closeStream();
    INT16 getDeviceNumber( const string& deviceName );
    string getDeviceName( INT16 deviceId );
        
    MidiEvent midiEvent_;
    SoundOptions* options_;
	RtMidiInput* midiIn_;
	RtAudio* audio_;
    Synth* synth_;
    bool running_;
    UINT32 bufferSize_;
    long latency_;
    State state_;
};

#else 

#include "Definitions.h"

class SoundDevices
{
public:
    void open() {}
    void close() {}
    void getDeviceInfos( bool inputs ) {}
    UINT32 getSampleRate()  { return 0; }
    bool isStandalone()     { return false; }
    
    enum State { NotReady, Ready };
    UINT16 getState()       { return Ready; }

    void suspend()   {};
    void resume()    {};

    void setSynth( Synth* synth ) {};
};

#endif // _RT_AUDIO
#endif // SOUNDDEVICES_