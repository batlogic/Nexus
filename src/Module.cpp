
#include "Module.h"
#include "ModuleData.h"
#include "ModuleInfo.h"
#include "Synth.h"
//#include "Program.h"



//---------------------------------------------------------
// Module
//---------------------------------------------------------

Module::Module() :
    MidiListener(),
	data_( NULL ),
    renderType_( 0 ),
	sampleRate_( SAMPLERATE ),
	polyphony_( NULL ),
    ptrAudioFunc_( NULL )
{}


Module::~Module()
{}


void Module::init( ModuleData* data, Synth* synth )
{
	data_       = data;
    sampleRate_ = synth->getSampleRate();
	polyphony_  = synth->getPolyphony();

    midiInterest_ = MidiListener::Gate | MidiListener::Pitch;
    polyphony_->attachMidiListener( this );

	setNumVoices( synth->getNumVoices() );
}


void Module::initPorts()
{
    initInPorts();
    initOutPorts();
}


void Module::initInPorts()
{
    for( UINT16 i=0; i<inPorts_.size(); i++ ) {
		inPorts_[ i ]->setNumVoices( numVoices_ );
	}
}


void Module::initOutPorts()
{
	for( UINT16 i=0; i<outPorts_.size(); i++ ) {
		outPorts_[ i ]->setNumVoices( numVoices_ );
	}
}


void Module::initParameters()
{
	for( ParamDataMap::iterator it = data_->params_.begin(); it != data_->params_.end(); it++ ) 
    {
		ParamData& paramData = it->second;
        setParameter( it->first, paramData.value_, 0, -1 );    	// init working parameters

        if( paramData.controller_ >= 0 ) {
            midiInterest_ |= Controller;
        }
	}
}


void Module::setNumVoices( UINT16 numVoices )
{
	VERIFY( data_ );

	mono_      = data_->polyphony_ == MONOPHONIC;
    numVoices_ = mono_ ? 1 : numVoices;

	initPorts();
	initData();
	initParameters();
}


void Module::setModulation( LinkData* linkData )
{
    UINT16 portId = linkData->outputId_;
    ASSERT( portId < outPorts_.size() );

    outPorts_[ portId ]->setModulation( linkData );
}


OutPort* Module::getOutPort( UINT16 id )  
{ 
    return id < outPorts_.size() ? outPorts_.at( id ) : NULL;
}


FLOAT* Module::connect( UINT16 portId )   
{ 
    VERIFY( portId >= 0 && portId < inPorts_.size() );
    VERIFY( numVoices_ > 0 );

    AudioIn* audioIn = inPorts_.at( portId );
    FLOAT* ptr = audioIn->connect();

    checkPorts();
    return ptr;
}


void Module::disconnect( UINT16 portId )
{
    VERIFY( portId >= 0 && portId < inPorts_.size() );

    AudioIn* audioIn = inPorts_.at( portId );
    audioIn->disconnect();
    checkPorts();
}


void Module::onGate( FLOAT gate, UINT16 voice )
{
    if( gate <= 0 )
        return;

    for( ParamDataMap::iterator it = data_->params_.begin(); it!=data_->params_.end(); it++ )
    {
        ParamData& paramData = it->second;
        if( paramData.veloSens_ )
        {
            gate *= paramData.veloSens_;
            gate += paramData.value_;
            paramData.makeLogarithmic( gate );

            setParameter( it->first, gate, 0, voice );
        }
    }

    for( UINT16 i=0; i<outPorts_.size(); i++ ) {        // set links
        outPorts_[i]->onGate( gate, voice );
    }
}


void Module::onPitch( FLOAT pitch, UINT16 voice )
{
    FLOAT offset = pitch - 60;
    offset      /= 128;

    for( ParamDataMap::iterator it = data_->params_.begin(); it!=data_->params_.end(); it++ )
    {
        ParamData& paramData = it->second;
        if( paramData.keyTrack_ )
        {
            offset *= paramData.keyTrack_;
            offset += paramData.value_;
            paramData.makeLogarithmic( offset );

            setParameter( it->first, offset, 0, voice );
        }
    }

    for( UINT16 i=0; i<outPorts_.size(); i++ ) {        // set links
        outPorts_[i]->onGate( offset, voice );          // TODO: das sollte nicht onGate heissen
    }
}


void Module::onController( UINT16 controller, FLOAT value )
{
    for( ParamDataMap::iterator it = data_->params_.begin(); it!=data_->params_.end(); it++ )
    {
        ParamData& paramData = it->second;
        if( paramData.scaleController( controller, value ))
        {
            paramData.value_ = value;
            setParameter( it->first, value );
        }
    }

    for( UINT16 i=0; i<outPorts_.size(); i++ ) {      // set links
        outPorts_[i]->onController( controller, value );
    }
}





