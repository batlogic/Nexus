
#ifndef MODULEIO_H
#define MODULEIO_H

#include "Definitions.h"
#include "Buffer.h"
#include "ModuleData.h"


class Module;
class OutPort;
class AudioIn;
class AudioOut;
class EventOut;


//------------------------------------------
// typedefs
//------------------------------------------

typedef vector< AudioIn* > InPortList;
typedef vector< OutPort* > OutPortList;
typedef Buffer< FLOAT* > AudioOutBuffer;

struct EventTarget 
{
	Module* target_;
	UINT16 inputId_;
};
typedef vector< EventTarget > EventOutBuffer;


enum PortAdapterType {
    NO_ADAPTER = 0,
    MONO_TO_POLY  = 1,
    POLY_TO_MONO  = 2
};


class OutPort
{
public:
    OutPort();
    virtual ~OutPort() {}

    virtual void connect( LinkData* linkData, Module* target, PortAdapterType adapter ) = 0;
    virtual void disconnect( LinkData* linkData, Module* target ) = 0;

    void setModulation( LinkData* linkData );
    void setNumVoices( UINT16 numVoices );

    void onGate( FLOAT gate, UINT16 voice );
    void onController( INT16 controller, FLOAT value );

protected:
    UINT16 addTarget( LinkData* linkData, PortAdapterType adapter );
    INT16 removeTarget( LinkData* linkData );
    INT16 getIndex( LinkData* linkData );

    void setModulation( UINT16 target, FLOAT value, INT16 voice=-1 );
    void initModulation();

    UINT16 numVoices_;
    UINT16 numTargets_;
    vector< LinkData* > targets_;

    Buffer< UINT16 > bufAdapter_;
    UINT16* adapter_;

    Buffer<FLOAT> bufModulation_;
    FLOAT* modulation_;
};


//----------------------------------------
// class AudioOut
// Stores pointers to the input buffers of all connected AudioInPorts.
//----------------------------------------

class AudioOut : public OutPort
{
public:
    AudioOut();
    void connect( LinkData* linkData, Module* target, PortAdapterType adapter );
    void disconnect( LinkData* linkData, Module* target );
    
    void __stdcall putAudio( FLOAT value, UINT16 voice=0 ) throw();

protected:
    AudioOutBuffer bufAudio_;
    FLOAT** ptrAudio_;
};


__forceinline void __stdcall AudioOut::putAudio( FLOAT value, UINT16 voice ) throw()
{
	UINT16 idxMod = voice;
    for( UINT16 target=0; target<numTargets_; target++ )
	{
		FLOAT* ptr = ptrAudio_[ target ];	         // get pointer to target
        FLOAT  val = value * modulation_[ idxMod ];  // apply modulation
        idxMod    += numVoices_;

        __assume( adapter_[ target ] < 3 );
        switch( adapter_[ target ] ) 
        {
        case NO_ADAPTER: 
            *(ptr + voice) += val; 	                // add value to existing value, per voice
            break;
        case MONO_TO_POLY: 
            for( UINT16 i=0; i<numVoices_; i++ ) {  // add value to all voices of target
                *(ptr + i) += val;
            }
            break;
        case POLY_TO_MONO:
            *ptr += val; 	                        // add value only to voice 0
            break;
        }
	}
}


//----------------------------------------
// class AudioIn
// A buffer of floats, where each float stores the value for one voice.
//----------------------------------------

class AudioIn 
{
public:
    AudioIn();
    FLOAT* setNumVoices( UINT16 numVoices );

    FLOAT* connect();
    void disconnect();
    bool isConnected();

protected:
    UINT16 numVoices_;
    INT16 numSources_;
    Buffer< FLOAT > bufAudio_;
    FLOAT* ptrAudio_;
};

/*
__forceinline FLOAT WINAPI AudioIn::get()
{
    FLOAT value  = ptrAudio_[0];  // get value
    ptrAudio_[0] = 0.0f;          // clear buffer[0] for use in next cycle

    return value;
}


__forceinline FLOAT WINAPI AudioIn::get( UINT16 voice )
{
    FLOAT value = ptrAudio_[voice];  // get value
    ptrAudio_[voice] = 0.0f;         // clear buffer[voice] for use in next cycle

    return value;
}
*/


//----------------------------------------
// class EventOut
//----------------------------------------

class EventOut : public OutPort, public EventOutBuffer
{
public:
	void connect( LinkData* linkData, Module* target, PortAdapterType adapter );
    void disconnect( LinkData* linkData, Module* target );
	void putEvent( FLOAT value, UINT16 voice );
    void putEvent( FLOAT value );
};


#endif // PORTS_H