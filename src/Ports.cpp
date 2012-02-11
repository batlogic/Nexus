
#include "Ports.h"
#include "Module.h"



//-------------------------------------------------------
// class OutPort
//-------------------------------------------------------

OutPort::OutPort() : 
    numVoices_( 0 ),
    numTargets_( 0 ),
    adapter_( NULL ),
    modulation_( NULL )
{}


UINT16 OutPort::addTarget( LinkData* linkData, PortAdapterType adapter )
{
    targets_.push_back( linkData );
    numTargets_++;

    adapter_                  = bufAdapter_.resize( numTargets_ );
    adapter_[ numTargets_-1 ] = adapter;
    initModulation();

    return numTargets_;
}


INT16 OutPort::removeTarget( LinkData* linkData )
{
    INT16 idx = getIndex( linkData );
    VERIFY( idx >= 0 );

    numTargets_--;
    targets_.erase( targets_.begin() + idx );

    adapter_ = bufAdapter_.resize( numTargets_ );
    initModulation();
    
    return idx;
}


void OutPort::setNumVoices( UINT16 numVoices )
{
    numVoices_  = numVoices;
    initModulation();
}


void OutPort::setModulation( UINT16 target, FLOAT value, INT16 voice )
{
    if( voice == -1 )
    {
        for( UINT16 i=0; i<numVoices_; i++ ) {
            modulation_[ target*numVoices_ + i ] = value;
        }
    }
    else {
        voice = min( numVoices_-1, voice );
        modulation_[ target*numVoices_ + voice ] = value;
    }
}


void OutPort::setModulation( LinkData* linkData )
{
    INT16 idx = getIndex( linkData );
    ASSERT( idx >= 0 );
    if( idx >= 0 ) 
    {
        setModulation( idx, linkData->value_, -1 );
    }
}


void OutPort::initModulation()
{
    bufModulation_.resize( 0 );
    modulation_ = bufModulation_.resize( numTargets_ * numVoices_ );
    
    for( UINT16 i=0; i<targets_.size(); i++ )
    {
        LinkData* linkData = targets_[i];
        setModulation( i, linkData->value_ );
    }
}


void OutPort::onGate( FLOAT gate, UINT16 voice )
{
    for( UINT16 i=0; i<targets_.size(); i++ )
    {
        LinkData* linkData = targets_[i];
        if( linkData->veloSens_ )
        {
            float value = linkData->makeLogarithmic( linkData->value_ );
            gate       *= linkData->veloSens_;
            value      += gate;
            setModulation( i, value, voice );
        }
    }
}


void OutPort::onController( INT16 controller, FLOAT value )
{
    for( UINT16 i=0; i<targets_.size(); i++ )
    {
        LinkData* linkData = targets_[i];
        if( linkData->scaleController( controller, value )) 
        {
            linkData->value_ = value;
            setModulation( i, value );
        }
    }
}



INT16 OutPort::getIndex( LinkData* linkData )
{
    for( UINT16 i=0; i<targets_.size(); i++ ) 
    {
        const LinkData& next = *targets_.at( i );
        if( next == linkData ) {
            return i;
        }
    }
    return -1;
}



//-------------------------------------------------------
// class AudioOut
//-------------------------------------------------------

AudioOut::AudioOut() : 
    ptrAudio_( NULL )
    {}


void AudioOut::connect( LinkData* linkData, Module* target, PortAdapterType adapter )
{
    VERIFY( target );
    addTarget( linkData, adapter );

	ptrAudio_                  = bufAudio_.resize( numTargets_ );
    FLOAT* ptrTarget           = target->connect( linkData->inputId_ );
	ptrAudio_[ numTargets_-1 ] = ptrTarget;

 //   FLOAT* ptrTarget   = target->connect( linkData->inputId_ );
 //   UINT16 idx         = addTarget( linkData );
	//ptrAudio_          = bufAudio_.resize( idx );
	//ptrAudio_[ idx-1 ] = ptrTarget;
}


void AudioOut::disconnect( LinkData* linkData, Module* target )
{
    VERIFY( target );
    
    INT16 idx = removeTarget( linkData );
    if( idx >= 0 ) 
    {
        ptrAudio_ = bufAudio_.removeAt( idx );
        target->disconnect( linkData->inputId_ );
    }
}



//-------------------------------------------------------
// class AudioIn
//-------------------------------------------------------

AudioIn::AudioIn() :
    numVoices_( 0 ),
    numSources_( 0 ),
    ptrAudio_( NULL )
    {}


FLOAT* AudioIn::setNumVoices( UINT16 numVoices )
{ 
    numVoices_ = numVoices; 
    ptrAudio_  = bufAudio_.resize( numVoices_ );

    return ptrAudio_;
}


FLOAT* AudioIn::connect()
{
    numSources_++;

    VERIFY( ptrAudio_ );
    return ptrAudio_;
}


void AudioIn::disconnect()
{
    VERIFY( numSources_ >= 1 );
    numSources_--;
}


bool AudioIn::isConnected() 
{ 
    VERIFY( numSources_ >= 0 );
    return numSources_ > 0; 
}



//-------------------------------------------------------
// class EventOut
//-------------------------------------------------------

void EventOut::connect( LinkData* linkData, Module* target, PortAdapterType adapter )
{
	VERIFY( target );

	EventTarget data;
	data.target_  = target;
	data.inputId_ = linkData->inputId_;

    addTarget( linkData, adapter );

    resize( numTargets_ );
	at( numTargets_-1 ) = data;
}


void EventOut::disconnect( LinkData* linkData, Module* target )
{
	INT16 idx = removeTarget( linkData );
    erase( begin() + idx );
}


void EventOut::putEvent( FLOAT value, UINT16 voice )
{
	for( UINT16 target=0; target<numTargets_; target++) 
	{
		FLOAT modulation     = modulation_[ target*numVoices_ + voice ];
        EventTarget* data    = &operator[]( target );
		Module* targetModule = data->target_; 
        ASSERT( targetModule );
        
        __assume( adapter_[ target ] < 3 );
        switch( adapter_[ target ] ) 
        {
        case NO_ADAPTER: 
            targetModule->setParameter( data->inputId_, value, modulation, voice );
            break;
        case MONO_TO_POLY: 
            for( UINT16 i=0; i<numVoices_; i++ ) {    
                targetModule->setParameter( data->inputId_, value, modulation, i );
            }
            break;
        case POLY_TO_MONO:
            targetModule->setParameter( data->inputId_, value, modulation, 0 );
            break;
        }
	}
}


void EventOut::putEvent( FLOAT value )
{
	for( UINT16 target=0; target<numTargets_; target++) 
	{
		for( UINT16 voice=0; voice<numVoices_; voice++ )
        {
            FLOAT modulation     = modulation_[ target*numVoices_ + voice ];
            EventTarget* data    = &operator[]( target );
		    Module* targetModule = data->target_; 
            ASSERT( targetModule );
        
            switch( adapter_[ target ] ) 
            {
            case NO_ADAPTER: 
            case MONO_TO_POLY: 
                targetModule->setParameter( data->inputId_, value, modulation, voice );
                break;
            case POLY_TO_MONO:
                targetModule->setParameter( data->inputId_, value, modulation, 0 );
                voice = numVoices_;
                break;
            }
        }
	}
}



