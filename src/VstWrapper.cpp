
#include "Definitions.h"
#include "VstWrapper.h"
#include "GUI/Editor.h"



AudioEffect* createEffectInstance( audioMasterCallback audioMaster )
{
	return new VstWrapper( audioMaster );
	//return new mdaJX10( audioMaster );
}



VstWrapper::VstWrapper( audioMasterCallback audioMaster )
	: AudioEffectX( audioMaster, NUMPROGRAMS, 0 ),
    editor_( NULL ),
    synth_( NULL )
{
    editor_ = new VstEditor( this );
	synth_  = new Synth( editor_ );
	setEditor( editor_ );

	setNumInputs( NUMINPUTS ); 
	setNumOutputs( NUMOUTPUTS ); 

	canProcessReplacing();
	isSynth();
    programsAreChunks();
	setUniqueID( 170760 );
}


VstWrapper::~VstWrapper()
{
    delete synth_;
    synth_ = NULL;
}


void VstWrapper::open()
{
    synth_->open();
}


void VstWrapper::close() 
{ 
    synth_->close(); 
}


void VstWrapper::suspend()
{
    synth_->suspend();
    AudioEffectX::suspend();
}


void VstWrapper::resume() 
{ 
    synth_->resume(); 
    AudioEffectX::resume(); 
}


void VstWrapper::process( float** inputs, float** outputs, VstInt32 numFrames )
{
   	synth_->process( inputs, outputs, numFrames );
}


void VstWrapper::processReplacing( float** inputs, float** outputs, VstInt32 numFrames )
{
   	synth_->processReplacing( inputs, outputs, numFrames );
}


VstInt32 VstWrapper::processEvents( VstEvents* events )
{
    for( VstInt32 i = 0; i < events->numEvents; i ++)
	{
		if( events->events[i]->type != kVstMidiType ) {
			continue ;
		}
        VstMidiEvent* evt = reinterpret_cast< VstMidiEvent* >( events->events[i] );
        if( evt != NULL)
        {
			MidiEvent evt( evt->midiData, evt->deltaFrames );
			synth_->processEvent( evt );
        }
	}
	return 1;
}


void VstWrapper::setSampleRate( FLOAT sampleRate )
{
    AudioEffectX::setSampleRate( sampleRate );
    synth_->setSampleRate( (UINT32)sampleRate );
}


VstInt32 VstWrapper::canDo( char* text )
{
	if( !strcmp( text, "receiveVstEvents" )) return 1;
	if( !strcmp( text, "receiveVstMidiEvent" ))	return 1;
	return -1;
}



bool VstWrapper::getVendorString( char* text )  
{ 
    strcpy_s( text, kVstMaxVendorStrLen, "Tom Büttner" );
    return true; 
}


bool VstWrapper::getProductString( char* text ) 
{ 
    strcpy_s( text, kVstMaxProductStrLen, "Nexus Modular Synth" );
    return true; 
}



bool VstWrapper::getEffectName( char* name )
{
    strcpy_s( name, kVstMaxEffectNameLen, "Nexus" );
    return  true;
}


bool VstWrapper::updateDisplay()
{
	bool result = false;
    if (audioMaster) {
		result = (audioMaster (&cEffect, audioMasterUpdateDisplay, 0, 0, 0, 0)) ? true : false;
    }
	return result;
}


VstIntPtr VstWrapper::dispatcher(  	
		VstInt32	opcode,
		VstInt32	index,
		VstIntPtr	value,
		void*		ptr,
		float  		opt )
{
    //switch( opcode ) {
    //case effEditKeyDown: return 0;
    //}
    return AudioEffectX::dispatcher( opcode, index, value, ptr, opt );
}




