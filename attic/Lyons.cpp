

#include "Lyons.h"

#include <stdio.h>
#include <math.h>



Lyons::Lyons (audioMasterCallback audioMaster )
	: AudioEffectX( audioMaster, 0, 1 )
{
	//sineOscil1_ = new SineWave();
	//sineOscil1_->setFrequency( 440 );

	//sineOscil2_ = new SineWave();
	//sineOscil2_->setFrequency( 440 * 3 );
}


Lyons::~Lyons()
{
	delete sineOscil1_;
	delete sineOscil2_;
}


void Lyons::processReplacing( float** inputs, float** outputs, VstInt32 numFrames )
{
	//float* out1 = outputs[0];
	//float* out2 = outputs[1];
	//register float tick;

	//for( VstInt32 i=0; i<numFrames; i++ ) 
	//{
	//	tick = (float)( -( sineOscil1_->process() + sineOscil2_->process() ) / 2.0 );

	//	*out1++ = tick;
	//	*out2++ = tick;
	//}
}







