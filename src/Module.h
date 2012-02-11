
#ifndef _MODULE_H
#define _MODULE_H

#include "Definitions.h"
#include "Polyphony.h"
#include "Ports.h"

class Synth;
class ModuleKit;
class ModuleData;


// Pointer to processAudio()
// Allows replacing methods at runtime
typedef void (Module::*PtrProcess)(void) throw();


class Module : public MidiListener
{
public:
	Module();
    virtual ~Module();

	virtual void init( ModuleData* data, Synth* synth );
	virtual void initPorts();
	virtual void initData() {}
	virtual void initParameters();
    
    void processAudio() throw() {}
	virtual void processControl() throw () {}
    virtual void processEvent( FLOAT value, UINT16 voices ) throw() {}

	virtual void suspend() {}
	virtual void resume() {}

	virtual void setSampleRate( INT32 newRate, INT32 oldRate )	{ sampleRate_ = newRate; }
	void setNumVoices( UINT16 numVoices );
    UINT16 getNumVoices() { return numVoices_; }

    virtual void setParameter( UINT16 paramId, FLOAT value, FLOAT modulation=0.f, INT16 voice=-1 ) {}
    virtual void setModulation( LinkData* linkData );

    virtual void onGate( FLOAT gate, UINT16 voice );
    virtual void onPitch( FLOAT pitch, UINT16 voice );
    virtual void onController( UINT16 eventNum, FLOAT value );
    virtual void onPitchbend( FLOAT value ) {}

    PtrProcess ptrProcessAudio_;
    virtual void portConnected( AudioIn* port ) {}
    
    ModuleData* data_;
    UINT16 renderType_;

    InPortList inPorts_;
    OutPortList outPorts_;
    
	OutPort* getOutPort( UINT16 id );
    FLOAT* connect( UINT16 portId );
    void disconnect( UINT16 portId );
    virtual void checkPorts() {}
	
protected:
    virtual void initInPorts();
    virtual void initOutPorts();

	INT32 sampleRate_;
	UINT16 numVoices_;
    bool mono_;
	Polyphony* polyphony_;
};


#endif // MODULE_H
