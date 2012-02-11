
#include "Voice.h"
#include "SystemModules.h"


void Voice::update( bool active, UINT16 pitch, UINT16 group, UINT32 tag )
{
	active_ = active;
	pitch_  = pitch;
	group_  = group;
	tag_    = tag;
}

//void Voice::noteOn( FLOAT frequency, FLOAT amplitude )
//{
//	onPitch( frequency );
//	onGate( amplitude );
//}
//
//
//void Voice::noteOff( FLOAT amplitude )
//{
//	onGate( amplitude );
//}


//void Voice::onGate( FLOAT amplitude )
//{
//	MidiGate* gate = dynamic_cast<MidiGate*>(Synth::getModule( ModuleKit::MIDI_GATE ));
//	gate->process( amplitude, voiceNum_ );
//}
//
//
//void Voice::onPitch( FLOAT frequency )
//{
//	MidiPitch* pitch = dynamic_cast<MidiPitch*>(Synth::getModule( ModuleKit::MIDI_PITCH ));
//	pitch->process( frequency, voiceNum_ );
//
//}




