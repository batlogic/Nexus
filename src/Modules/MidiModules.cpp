
#include "MidiModules.h"
#include "Synth.h"
#include "../ModuleInfo.h"
#include "../Utils.h"



//--------------------------------------------------------
// class MidiGate
//--------------------------------------------------------

MidiGate::MidiGate() : Module()
{
	outPorts_.push_back( &eventOut_ );
}


void MidiGate::init( ModuleData* data, Synth* synth )
{
    Module::init( data, synth );
    midiInterest_ = Note;
}


void MidiGate::onNoteEvent( FLOAT pitch, FLOAT gate, UINT16 voice )
{
    if( gate > -1 )
        eventOut_.putEvent( gate, voice );
}




//--------------------------------------------------------
// class MidiPitch
//--------------------------------------------------------

MidiPitch::MidiPitch() : 
    Module(),
    freq_( NULL ),
    glideDelta_( NULL ),
    glideTarget_( NULL ),
    glideFrames_( 0 ),
    glideAuto_( true ),
    bendFactor_( 1.f ),
    bendRange_( 2 )
{
    outPorts_.push_back( &freqOut_ );
}


void MidiPitch::initData()
{
    glideDelta_   = bufGlideDelta_.resize( numVoices_ );
    glideTarget_  = bufGlideTarget_.resize( numVoices_ );
    freq_         = bufFreq_.resize( numVoices_ );

    midiInterest_ = Note | Pitchbend;
}


void MidiPitch::setParameter( UINT16 paramId, FLOAT value, FLOAT modulation, INT16 voice ) 
{
	switch( paramId ) 
    {
	case PARAM_BEND_RANGE: bendRange_ = (INT16)value;         break;
    case PARAM_GLIDE_TIME: setGlideTime( value );             break;
    case PARAM_GLIDE_AUTO: glideAuto_ = value ? true : false; break;
	}
}


void MidiPitch::onNoteEvent( FLOAT pitch, FLOAT gate, UINT16 voice )
{
    FLOAT freq   = (FLOAT)Utils::pitch2freq( pitch );
    freq_[voice] = freq;

    if( gate != 0 )
    {
        calcGlide( freq, voice );
        freqOut_.putEvent( freq_[voice] * bendFactor_, voice );
    }
}


void MidiPitch::onPitchbend( UINT16 value1, UINT16 value2 )
{
    UINT16 value = Utils::combine14Bits( value1, value2 );
    FLOAT cents  = ((value - 0x2000) / (FLOAT)0x2000) * bendRange_ * 100;
    bendFactor_  = pow( 2, cents / 1200.f );

    UINT16 maxVoices = min( numVoices_, polyphony_->numSounding_ );
	for( UINT16 i=0; i<maxVoices; i++ )
    {
        UINT16 v = polyphony_->soundingVoices_[i];
        freqOut_.putEvent( freq_[v] * bendFactor_, v );
    }
}


void MidiPitch::processControl() throw()
{
	if( glideFrames_ == 0 )
        return;

    UINT16 maxVoices = min( numVoices_, polyphony_->numSounding_ );
	for( UINT16 i=0; i<maxVoices; i++ )
	{
		UINT16 v = mono_ ? 0 : polyphony_->soundingVoices_[i];

        if( glideDelta_[v] > 0 && freq_[v] >= glideTarget_[v] || glideDelta_[v] < 0 && freq_[v] <= glideTarget_[v] ) {
            glideDelta_[v] = 0.0f;
        } 
        else {
            freq_[v] += glideDelta_[v];
            freqOut_.putEvent( freq_[v] * bendFactor_, v );
        }
    }
}


void MidiPitch::calcGlide( FLOAT freq, UINT16 voice )
{
    if( glideFrames_ > 0) 
    {
        glideTarget_[voice] = freq;
        FLOAT prevPitch     = polyphony_->getPreviousPitch( voice );

        if( prevPitch == -1 || glideAuto_ && polyphony_->numActive_ <= polyphony_->numUnison_ ) {
            bufGlideDelta_.set( 0 );
        }
        else {
            FLOAT prevFreq     = (FLOAT)Utils::pitch2freq( prevPitch );
            FLOAT diff         = freq - prevFreq;
            glideDelta_[voice] = diff / glideFrames_;
            freq_[voice]       = prevFreq;
        }
    }
}


void MidiPitch::setGlideTime( FLOAT time )
{
    FLOAT msPerFrame = ( 1 / (FLOAT)CONTROLRATE ) * 1000;
    glideFrames_     = time / msPerFrame;
}


//--------------------------------------------------------
// class MidiInput
//--------------------------------------------------------

MidiInput::MidiInput() : 
    MidiPitch()
{
    outPorts_.push_back( &gateOut_ );
}


void MidiInput::onNoteEvent( FLOAT pitch, FLOAT gate, UINT16 voice )
{
    //TRACE( "MIDIINPUT pitch=%f gate=%f voice=%d\n", pitch, gate, voice );

    MidiPitch::onNoteEvent( pitch, gate, voice );

    if( gate > -1 ) {
        gateOut_.putEvent( gate, voice );
    }
}


