

#include "Polyphony.h"
#include "Synth.h"
#include "Trace.h"




//---------------------------------------------------------------
// class MidiListener
//---------------------------------------------------------------

MidiListener::MidiListener() :
    midiInterest_( Nothing ),
    midiSource_( NULL )
    {}

MidiListener::~MidiListener()
{
    if( midiSource_ ) 
        midiSource_->detachMidiListener( this );
    midiSource_ = NULL;
}


//-------------------------------------------------------
// class Voice
//-------------------------------------------------------

Voice::Voice()	 
{ 
    reset(); 
}


void Voice::reset()
{
    id_          = -1;
    state_       = Silent;
    pitch_       = -1;
    tag_         = -1;
    unisonGroup_ = -1;
}
        
        
void Voice::init( INT16 id, VoiceState state, FLOAT pitch, INT32 tag )
{
    id_     = id;
    state_  = state;
	pitch_  = pitch;
    tag_    = tag;
}


//---------------------------------------------------------------
// class Polyphony
//---------------------------------------------------------------

Polyphony::Polyphony() : 
    numSounding_( 0 ),
    numActive_( 0 ),
    numUnison_( 1 ),
    unisonSpread_( 5 ),
    tags_( 0 ),
    retrigger_( false ),
    legato_( false ),
    lastPitch_( -1 )
{
	setNumVoices( 1 );
    stack_.reserve( 100 );
}


Polyphony::~Polyphony()
{}


void Polyphony::noteOn( UINT16 pitch, UINT16 gate )
{
    FLOAT g = (FLOAT)gate;
    g      /= ((numUnison_ > 1) ? (numUnison_*0.66f) : 1);
    g      *= ONE_OVER_128;

    INT32 unisonGroup = tags_;

    for( UINT16 i=0; i<numUnison_; i++ )                    // trigger numUnison voices
    {
        UINT16 voice = getUnusedVoice();
        if( voice >= 0 ) 
        {
            Voice* current        = &voices_[ voice ];
            current->unisonGroup_ = unisonGroup;
            current->pitch_       = pitch + i * (unisonSpread_ / 100.f);

            if( i == 0 ) {                                  // put the played note on the stack
                Voice v = *current;
                v.init( voice, NoteOn, pitch, tags_ );
                stack_.push_back( v );
            }
            
            startVoice( voice, current->pitch_, g );
        }
    }
    retrigger( pitch );                                     // retrigger all sounding voices if retrigger is enabled
    lastPitch_ = pitch;
    dumpStack( "NOTE_ON" );
}


void Polyphony::noteOff( UINT16 pitch )
{
    INT16 idxStack  = -1;
    Voice* offVoice = NULL; 
    for( idxStack=stack_.size()-1; idxStack>=0; idxStack-- )    // find voice
    {
        if( stack_[idxStack].pitch_ == pitch ) {
            offVoice = &stack_[idxStack];
            break;
        } 
    }
    if( offVoice == NULL ) {
        //ASSERT( offVoice );
        return;
    }
        
    //if( retrigger_ && stack_.size() >= 2 )                      // retrigger remaining voices
    //{
    //    Voice* prev = &stack_.at( stack_.size()-2 );             // note that was previously played

    //    if( prev->tag_ != offVoice->tag_ )
    //    {
    //        lastPitch_ = offVoice->pitch_;
    //        retrigger( prev->pitch_ );                          // retrigger all sounding notes
    //        lastPitch_ = prev->pitch_;
    //    }
    //}

    for( UINT16 i=0; i<maxVoices_; i++ )                    // set state to NoteOff for the voice and all assigned unisonVoices
    {
        Voice* next = &voices_[i];
        if( next->state_ > Silent && (next->unisonGroup_ == offVoice->unisonGroup_ || legato_ )) 
        {
            next->state_ = NoteOff;
            notifyGate( 0, next->id_ );

            FLOAT gate = legato_ && numActive_ > numUnison_ ? -1.f : 0.f;
            notifyNote( next->pitch_, gate, next->id_ );

            numActive_--;
            ASSERT( numActive_ >= 0 );
        }
    }

    if( retrigger_ && stack_.size() >= 2 )                      // retrigger remaining voices
    {
        Voice* prev = &stack_.at( stack_.size()-2 );             // note that was previously played

        if( prev->tag_ != offVoice->tag_ )
        {
            lastPitch_ = offVoice->pitch_;
            retrigger( prev->pitch_ );                          // retrigger all sounding notes
            lastPitch_ = prev->pitch_;
        }
    }

    stack_.erase( stack_.begin()+idxStack );                    // remove note from stack
    dumpStack( "NOTE_OFF" );
}


void Polyphony::startVoice( UINT16 voice, FLOAT pitch, FLOAT gate )
{
    if( numSounding_ == 0 ) 
        synth_->beginProfiling();
	
    voices_[voice].init( voice, NoteOn, pitch, tags_ );

    notifyPitch( pitch, voice );
    notifyGate( gate, voice );

    if( legato_ && numActive_ > numUnison_ ) { 
        gate = -1;
    }
    notifyNote( pitch, gate, voice );

	tags_++;
    numSounding_++;
    numActive_++;
	updateSoundingVoices();
	monitorNoteEvent( voice, pitch, gate );
    ASSERT( numSounding_ <= (INT16)maxVoices_ );
}


void Polyphony::endVoice( UINT16 voice )
{
	TRACE( "END_VOICE voice=%d\n", voice );

    Voice& v = voices_[ voice ];
	if( v.state_ ) 
	{
		v.reset();
		numSounding_--;
		updateSoundingVoices();
        monitorNoteEvent( voice, v.pitch_, 0 );
	}
	ASSERT( numSounding_ >= 0 );
    if( numSounding_ == 0 )
        synth_->endProfiling();
}


void Polyphony::allNotesOff()
{
    for( UINT16 i=0; i<voices_.size(); i++ ) {
        voices_[i].reset();
    }
    stack_.clear();

    numSounding_ = 0;
    numActive_ = 0;
    updateSoundingVoices();
    monitorNoteEvent( 0, 0, 0 );
}


INT16 Polyphony::getUnusedVoice()
{
	INT16 idxKill = -1;

    for( UINT16 i=0; i<maxVoices_; i++ ) 
	{
		if( voices_[i].state_ == Silent ) {         // voices available?
			return i;
		}
	}

	for( UINT16 i=0; i<maxVoices_; i++ )		// all voices sounding, so interrupt the oldest.
    {
		if( idxKill == -1 ) {
			idxKill = i;
		}
		else if( voices_[i].tag_ < voices_[idxKill].tag_ ) {
			idxKill = i;
		}
	}
	if( idxKill >= 0 ) {
        endVoice( idxKill );
	}
    return idxKill;
}


void Polyphony::updateSoundingVoices()
{
	UINT16 counter = 0;

	for( UINT16 i=0; i<maxVoices_; i++ )
	{
		if( voices_[i].state_ ) {
			soundingVoices_[ counter++ ] = i;
		}
	}
}


void Polyphony::notifyPitch( FLOAT pitch, UINT16 voice )
{
    for( MidiListenerSet::iterator it=listeners_.begin(); it!=listeners_.end(); it++ )
    {
        MidiListener* listener = (*it); 
        if( listener->midiInterest_ & MidiListener::Pitch )
            listener->onPitch( pitch, voice );
    }
}


void Polyphony::notifyGate( FLOAT gate, UINT16 voice )
{
    for( MidiListenerSet::iterator it=listeners_.begin(); it!=listeners_.end(); it++ )
    {
        MidiListener* listener = (*it); 
        if( listener->midiInterest_ & MidiListener::Gate )
            listener->onGate( gate, voice );
    }
}


void Polyphony::notifyNote( FLOAT pitch, FLOAT gate, UINT16 voice )
{
    for( MidiListenerSet::iterator it=listeners_.begin(); it!=listeners_.end(); it++ )
    {
        MidiListener* listener = (*it); 
        if( listener->midiInterest_ & MidiListener::Note )
            listener->onNoteEvent( pitch, gate, voice );
    }
}


void Polyphony::notifyPitchbend( UINT16 value1, UINT16 value2 )
{
    for( MidiListenerSet::iterator it=listeners_.begin(); it!=listeners_.end(); it++ )
    {
        MidiListener* listener = (*it); 
        if( listener->midiInterest_ & MidiListener::Pitchbend )
            listener->onPitchbend( value1, value2 );
    }
}


void Polyphony::notifyControlChange( UINT16 eventNum, FLOAT value )
{
    for( MidiListenerSet::iterator it=listeners_.begin(); it!=listeners_.end(); it++ )
    {
        MidiListener* listener = (*it); 
        if( listener->midiInterest_ & MidiListener::Controller )
            listener->onController( eventNum, value );
    }
}


void Polyphony::attachMidiListener( MidiListener* listener )
{
    listeners_.insert( listener );
    listener->midiSource_ = this;
}


void Polyphony::detachMidiListener( MidiListener* listener )
{
    MidiListenerSet::iterator pos = listeners_.find( listener );

    if( pos != listeners_.end() ) {
        listeners_.erase( pos );
        listener->midiSource_ = NULL;
    }
}


void Polyphony::setNumVoices( UINT16 numVoices )
{
	stack_.clear();
    voices_.clear();
    voices_.resize( numVoices );

	numSounding_    = 0;
    numActive_      = 0;
    soundingVoices_ = bufSoundingVoices_.resize( numVoices );
    maxVoices_      = legato_ ? numUnison_ : voices_.size();
	updateSoundingVoices();
}


void Polyphony::setNumUnison( UINT16 numUnison )
{
    maxVoices_ = legato_ ? numUnison : voices_.size();
    numUnison_ = min( maxVoices_, numUnison );
    allNotesOff();
}


void Polyphony::setUnisonSpread( UINT16 cent )
{
    unisonSpread_ = cent;
}


void Polyphony::setLegato( bool legato )       
{ 
    legato_ = legato; 
    maxVoices_ = legato_ ? numUnison_ : voices_.size();
}


FLOAT Polyphony::getPreviousPitch( UINT16 voice )
{
    if( lastPitch_ >= 0 && voice >= 0 && voice < maxVoices_ )
    {
        Voice& current = voices_[ voice ];
        INT32 diff     = current.tag_ - current.unisonGroup_;

        ASSERT( diff >= 0 );
        return lastPitch_ + diff * (unisonSpread_ / 100.f);
    }
    return -1;
}


void Polyphony::retrigger( FLOAT basePitch )
{
    if( retrigger_ )
    {
        for( UINT16 i=0; i<stack_.size(); i++ )                 // iterate over all played voices
        {
            Voice* first   = &stack_[i];
            UINT16 counter = 0;

            for( UINT16 j=0; j<maxVoices_ && counter < numUnison_; j++ )    // iterate over unison voices
            {
                Voice* next = &voices_[j];
                if( next->unisonGroup_ == first->unisonGroup_ && next->state_ > Silent )
                {
                    FLOAT pitch  = basePitch + counter * (unisonSpread_ / 100.f );
                    //next->pitch_ = pitch;
                    notifyNote( pitch, -1, j );       
                    counter++;
                }
            }
        }
    }
}


void Polyphony::monitorNoteEvent( UINT16 voice, FLOAT pitch, FLOAT gate ) const
{
    synth_->setMonitor( pitch, gate, numSounding_ );
   
    //const char* s = gate > 0 ? "On" : "Off";
	//TRACE( "voice%s: [%d, %d] voice: %d/%d\n", s, (INT16)pitch, (INT16)gate, (INT16)voice, (INT16)numSounding_ );
}


void Polyphony::dumpStack( const string& msg )
{
    TRACE( string( msg + " (dumping stack)\n" ).c_str());

    for( UINT16 i=0; i<stack_.size(); i++ ) {
        TRACE( "pitch=%f state=%d unisonGroup=%d\n", stack_[i].pitch_, stack_[i].state_, stack_[i].unisonGroup_ );
    }
}


