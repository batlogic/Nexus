
#include "Synth.h"
#include "GUI/Editor.h"
#include "ModuleInfo.h"
#include "SoundDevices.h"
#include "AppOptions.h"
#include "Modules/Master.h"



Synth::Synth( Editor* editor ) : 
	editor_( editor ),
    devices_( NULL ),
	program_( NULL ),
	running_( false ),
	ready_( false ),
    sampleRate_( 0 ),
    tuning_( 0.f ),
	frameCounter_( 0 ),
    dummyMaster_( 0 ),
    master_( &dummyMaster_ )
{
	ModuleInfo::buildMap();
    setSampleRate( SAMPLERATE );

    if( editor_ ) {
        editor_->setSynth( this );
        
        if( editor_->isStandalone() ) 
        {
    	    devices_ = editor->getDevices(),
            setSampleRate( devices_->getSampleRate() );
            devices_->setSynth( this );    
        }
        setTuning( editor_->getOptions()->tuning_ );
    }
    polyphony_.setSynth( this );
}


void Synth::open()
{
    loadBank();
}


void Synth::close()
{
    if( editor_ )
        editor_->closeBank();
    else
        bank_.clear();

    program_ = NULL;
}


void Synth::cleanup()
{
	ModuleList::clear();
    master_ = &dummyMaster_;
}

    
//------------------------------------------------------------------------------
// Processing
//------------------------------------------------------------------------------

void Synth::suspend()
{
    if( running_ )
    {
        if( devices_ ) devices_->suspend();
        setRunning( false );
        checkAppState();
    }
}


void Synth::resume()
{
    if( running_ == false )
    {
        setRunning( true );
        if( devices_ ) devices_->resume();
        checkAppState();
    }
}


void Synth::setRunning( bool running ) 
{ 
    running_ = ready_ ? running : false; 

    if( ready_ && program_ ) {
        program_->setRunning( running_ );
    }
}



void Synth::processEvent( const MidiEvent& evt )
{
	if( ready_ == false || running_ == false ) return;

    UINT8 type   = evt.getData( 0 );
	UINT8 value1 = evt.getData( 1 );
	UINT8 value2 = evt.getData( 2 );

	try {
		switch( type ) 
		{
		case MidiEvent::NoteOn:      // velocity is zero: really a NoteOff, otherwise NoteOn
            value2 == 0 ? polyphony_.noteOff( value1 ) : polyphony_.noteOn( value1, value2 );
			break;

		case MidiEvent::NoteOff:
			polyphony_.noteOff( value1 ); // ignore note-off velocity
			break;

		case MidiEvent::ControlChange:
			polyphony_.notifyControlChange( (UINT16)value1, value2 );
			break;

		case MidiEvent::AfterTouch:
			polyphony_.notifyControlChange( 128, value1 );
            break;

		case MidiEvent::PitchWheel:
            polyphony_.notifyPitchbend( (UINT16)value1, (UINT16)value2 );
			break;

		case MidiEvent::Clock:
			break;
		}
	}
	catch( const exception& e ) {
		TRACE( e.what() );
        suspend();
        editor_->checkAppState();
	}
}


//-----------------------------------------------------------------
// Program Handling
//-----------------------------------------------------------------

void Synth::loadBank( const string& path )
{
	try {
        suspend();

        INT32 programNum = 0;
        program_ = NULL;

        if( editor_ ) {
            programNum = editor_->loadBank( path );
        } else {
            bank_.generateDefaultBank();
        }
        setProgram( programNum );
	}
	catch( const exception& e ) 
    {
        ready_ = false;
        checkAppState();
		TRACE( e.what() );
	}
}


void Synth::newBank( const string& path, const string& name, bool autosave )
{
	if( editor_ == NULL ) 
        return;

    try {
        suspend();

        program_ = NULL;
        INT32 programNum = editor_->newBank( path, name, autosave );

        setProgram( programNum );
	}
	catch( const exception& e ) 
    {
        ready_ = false;
        checkAppState();
		TRACE( e.what() );
	}
}


void Synth::setProgram( INT32 programNum )
{
    try {
		suspend();
        ready_   = false;

        program_ = editor_ ? editor_->setProgram( programNum ) : bank_.setProgram( programNum, false );

        polyphony_.setProgram( program_ );
		program_->compile( this );

		if( editor_ ) {
            ready_ = editor_->programMessage( Editor::ProgramChanged, NULL );
        }
        else ready_ = true;
		resume();
	}
	catch( const exception& e ) {
		ready_ = false;
        checkAppState();
		TRACE( e.what() );
	}
}


void Synth::setProgramName( char* name )
{
    if( program_ )
        program_->name_ = name;
}


void Synth::getProgramName( char* name )
{
	getProgramNameIndexed( -1, getProgramNumber(), name );
}


bool Synth::getProgramNameIndexed( INT32 category, INT32 index, char* name )
{
    bank_.getProgramName( index, name );
    return true;
}


INT32 Synth::getProgramNumber()					
{ 
    return bank_.programNum_; 
}


bool Synth::beginSetProgram()  
{ 
    return ready_; 
}


bool Synth::endSetProgram()    
{ 
    return ready_; 
}



//------------------------------------------------
// Module Handling
//------------------------------------------------

void Synth::addModule( ModuleData* data )
{
	try {
        program_->addModule( data, this );
	}
	catch( const exception& e ) {
        TRACE( e.what() );
        suspend();
        checkAppState();
	}
}


void Synth::removeModule( ModuleData* data )
{
	suspend();
	try {
        program_->removeModule( data, this );
	}
	catch( const exception& e ) {
        TRACE( e.what() );
        checkAppState();
        return;
	}
	resume();
}


bool Synth::addLink( ModuleData* data, LinkData& linkData )
{
	suspend();
	try {
        program_->addLink( data, linkData, this );
	}
	catch( const exception& e ) {
        TRACE( e.what() );
        checkAppState();
		return false;
	}
	resume();
	return true;
}


void Synth::removeLink( LinkData& linkData )
{
	suspend();
	try {
        program_->removeLink( linkData, this, true );
	}
	catch( const exception& e ) {
        TRACE( e.what() );
        checkAppState();
        return;
	}
	resume();
}


void Synth::removeLinks( LinkDataList& list )
{
	suspend();
	try {
        program_->removeLinks( list, this );
	}
	catch( const exception& e ) {
        TRACE( e.what() );
        checkAppState();
        return;
	}
	resume();
}


void Synth::setPolyphony( ModuleData* data, PolyphonyType poly )
{
	suspend();
	try {
        program_->setPolyphony( data, poly, this );
	}
	catch( const exception& e ) {
        TRACE( e.what() );
        checkAppState();
        return;
	}
	resume();
}


void Synth::setSampleRate( INT32 sampleRate )
{
    if( sampleRate > 0 && sampleRate != sampleRate_ ) 
	{
		INT32 oldRate = sampleRate_;
		sampleRate_   = (INT32)sampleRate;

		if( program_ )
        {
            suspend();
            program_->setSampleRate( sampleRate_, oldRate );
		    resume();
        }
	}
    controlRateDiv_ = sampleRate_ / CONTROLRATE;
}


void Synth::setNumVoices( UINT16 numVoices )	
{ 
    if( numVoices > 0 && program_ && numVoices != program_->numVoices_ )
    {
	    suspend();
	    try {
            polyphony_.setNumVoices( numVoices );
            program_->setNumVoices( numVoices, this );
	    }
	    catch( const exception& e ) {
            TRACE( e.what() );
            checkAppState();
            return;
	    }
	    resume();
    }
}


void Synth::setMonitor( INT16 numSounding, FLOAT pitch, FLOAT gate, UINT16 flags )
{
    if( editor_ ) 
        editor_->setMonitor( numSounding, pitch, gate, flags );
}


void Synth::checkAppState()
{
    if( editor_ ) 
        editor_->checkAppState();
}


bool Synth::containsModule( Module* module )
{
    for( UINT16 i=0; i<size(); i++ ) {
        if( operator[](i) == module )
            return true;
    }
    return false;
}


void Synth::compile( Module* master )
{
	reverse( begin(), end() );

	master_ = master ? &(dynamic_cast< Master* >(master)->value_) : &dummyMaster_;
}












