

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


Voice::Voice( INT16 id, UINT16 state, FLOAT pitch, FLOAT gate, INT32 tag, INT32 unisonGroup )
{
    init( id, state, pitch, gate, tag );
    unisonGroup_ = unisonGroup;
}


void Voice::reset()
{
    id_          = -1;
    state_       = Silent;
    pitch_       = -1;
    gate_        = -1;
    tag_         = -1;
    unisonGroup_ = -1;
}
        
        
void Voice::init( INT16 id, UINT16 state, FLOAT pitch, FLOAT gate, INT32 tag )
{
    id_     = id;
    state_  = state;
	pitch_  = pitch;
    gate_   = gate;
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
    hold_( false ),
    sustain_( false ),
    retrigger_( false ),
    legato_( false ),
    lastPitch_( -1 )
{
	setNumVoices( 1 );
    stack_.reserve( 100 );
}


Polyphony::~Polyphony()
{}


void Polyphony::noteOn( INT16 pitch, INT16 gate )
{
    monitorNoteEvent( -1, pitch, gate, MonitorPitch|MonitorGate );

    FLOAT g = gate * ONE_OVER_128;
    g /= ((numUnison_ > 1) ? (numUnison_*0.66f) : 1);
    INT32 unisonGroup = tags_;

    FLOAT basePitch = (FLOAT)pitch + synth_->getTuning();

    if( hold_ && numActive_ == 0 && !sustain_ ) {
        allNotesOff( false );
    }

    for( UINT16 i=0; i<numUnison_; i++ )                    // trigger numUnison voices
    {
        UINT16 voice = getUnusedVoice();
        if( voice >= 0 ) 
        {
            FLOAT p               = basePitch + i * unisonSpread_;
            Voice* current        = &voices_[ voice ];
            current->unisonGroup_ = unisonGroup;
            current->pitch_       = p;

            if( i == 0 ) {                                  // put the played note on the stack
                stack_.push_back( Voice( voice, NoteOn, p, g, tags_, unisonGroup ));
            }
            startVoice( voice, p, g );
        }
    }
    combineVoices( basePitch, g );                            
    lastPitch_ = basePitch;
}


void Polyphony::noteOff( INT16 pitch )
{
    monitorNoteEvent( numSounding_, -1, -1, MonitorAll );
    FLOAT basePitch = (FLOAT)pitch + synth_->getTuning();

    INT16 idxStack  = -1;
    Voice* offVoice = NULL; 

    for( idxStack=stack_.size()-1; idxStack>=0; idxStack-- )    // find voice
    {
        if( stack_[idxStack].pitch_ == basePitch ) {
            offVoice = &stack_[idxStack];
            break;
        } 
    }
    if( offVoice == NULL ) {
        ASSERT( offVoice );
        return;
    }

    if( retrigger_ && stack_.size() >= 2 )                      // retrigger remaining voices
    {
        Voice* prev = &stack_.at( stack_.size()-2 );

        if( prev->tag_ != offVoice->tag_ )
        {
            lastPitch_ = offVoice->pitch_;
            combineVoices( prev->pitch_, -1 );
            lastPitch_ = prev->pitch_;
        }
    }

    for( UINT16 i=0; i<numVoices_; i++ )                       // set state to NoteOff for the voice and all assigned unisonVoices
    {
        Voice* next = &voices_[i];
        if( next->unisonGroup_ == offVoice->unisonGroup_ && next->state_ > Silent )
        {
            next->state_ &= ~NoteOn;
            next->state_ |= NoteOff;
            numActive_--;
            ASSERT( numActive_ >= 0 );

            if( !hold_ && !sustain_ || (hold_ && retrigger_ && !sustain_ && numActive_ >= numUnison_ ))  {
                notifyGate( 0, next->id_ );
                notifyNote( next->pitch_, 0, next->id_ );
            }
        }
    }
    stack_.erase( stack_.begin()+idxStack );                    // remove note from stack
}


void Polyphony::startVoice( UINT16 voice, FLOAT pitch, FLOAT gate )
{
    if( numSounding_ == 0 ) 
        synth_->beginProfiling();
	
    UINT16 state = NoteOn | (hold_ ? NoteHold : 0);
    voices_[voice].init( voice, state, pitch, gate, tags_ );

    notifyPitch( pitch, voice );
    notifyGate( gate, voice );
    notifyNote( pitch, gate, voice );

	tags_++;
    numSounding_++;
    numActive_++;
	updateSoundingVoices();
	monitorNoteEvent( numSounding_, -1, -1, MonitorVoices );
    ASSERT( numSounding_ <= (INT16)numVoices_ );
}


void Polyphony::endVoice( UINT16 voice )
{
    Voice& v = voices_[ voice ];
	if( v.state_ ) 
	{
		numSounding_--;
        monitorNoteEvent( numSounding_, -1, -1, MonitorVoices );

        v.reset();
		updateSoundingVoices();
	}
	ASSERT( numSounding_ >= 0 );
    if( numSounding_ == 0 )
        synth_->endProfiling();
}


void Polyphony::allNotesOff( bool reset )
{
    for( UINT16 i=0; i<voices_.size(); i++ ) 
    {
        Voice* next = &voices_[i];
        if( reset ) {
            next->reset();
        } 
        else if( next->state_ > Silent ) 
        {
            next->state_ = NoteOff;
            notifyGate( 0, next->id_ );
            notifyNote( next->pitch_, 0, next->id_ );
        }
    }
    stack_.clear();

    if( reset ) numSounding_ = 0;
    numActive_ = 0;
    updateSoundingVoices();
    monitorNoteEvent( numSounding_, -1, -1, MonitorAll );
}


INT16 Polyphony::getUnusedVoice()
{
	INT16 idxKill = -1;

    for( UINT16 i=0; i<numVoices_; i++ ) 
	{
		if( voices_[i].state_ == Silent ) {         // voices available?
			return i;
		}
	}

	for( UINT16 i=0; i<numVoices_; i++ )		// all voices sounding, so interrupt the oldest.
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


void Polyphony::combineVoices( FLOAT basePitch, FLOAT gate )
{
    if( retrigger_ || legato_ )
    {
        gate = legato_ ? -1.f : gate;

        for( UINT16 i=0; i<stack_.size(); i++ )                 // iterate over all played voices
        {
            Voice* first   = &stack_[i];
            UINT16 counter = 0;
            for( UINT16 j=0; j<numVoices_ && counter < numUnison_; j++ )    // iterate over unison voices
            {
                Voice* next = &voices_[j];
                if( next->unisonGroup_ == first->unisonGroup_ && next->state_ > Silent )
                {
                    FLOAT pitch = basePitch + counter * unisonSpread_;
                    
                    if( next->pitch_ != pitch ) 
                    {
                        next->pitch_ = pitch;
                        notifyNote( pitch, gate, j );       
                    }
                    counter++;
                }
            }
        }
    }
}


void Polyphony::updateSoundingVoices()
{
	UINT16 counter = 0;

	for( UINT16 i=0; i<numVoices_; i++ )
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
    if( eventNum == 64 ) {   // Sustain
        setSustain( value >= 64 );
    }
    
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
    numVoices_      = numVoices;
    soundingVoices_ = bufSoundingVoices_.resize( numVoices );
	updateSoundingVoices();
}


void Polyphony::setNumUnison( UINT16 numUnison )
{
    numUnison_ = min( numVoices_, numUnison );
}


void Polyphony::setUnisonSpread( UINT16 cent )
{
    unisonSpread_ = cent / 100.f;
}


void Polyphony::setHold( bool hold )
{
    hold_ = hold;
    if( hold_ == false ) 
        allNotesOff( false );
}


void Polyphony::setSustain( bool sustain )
{
    sustain_ = sustain;
    if( sustain == false )
    {
        for( UINT16 i=0; i<numVoices_; i++ )
        {
            Voice* next = &voices_[i];
            if( next->state_ == NoteOff ) 
            {
                notifyGate( 0, next->id_ );
                notifyNote( next->pitch_, 0, next->id_ );
            }
        }
    }
}


void Polyphony::setLegato( bool legato )       
{ 
    legato_ = legato; 
}


void Polyphony::setProgram( Program* program )
{
    setNumVoices( program->numVoices_ );
    setNumUnison( program->numUnison_ );
    setUnisonSpread( program->unisonSpread_ );
    setHold( program->hold_ );
    setRetrigger( program->retrigger_ );
    setLegato( program->legato_ );
}


FLOAT Polyphony::getPreviousPitch( UINT16 voice )
{
    if( lastPitch_ >= 0 && voice >= 0 && voice < voices_.size() )
    {
        Voice& current = voices_[ voice ];
        INT32 diff     = current.tag_ - current.unisonGroup_;

        ASSERT( diff >= 0 );
        return lastPitch_ + diff * unisonSpread_;
    }
    return -1;
}


void Polyphony::monitorNoteEvent( INT16 numSounding, FLOAT pitch, FLOAT gate, UINT16 flags ) const
{
    synth_->setMonitor( numSounding, pitch, gate, flags );
   
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


