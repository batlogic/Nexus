

#include "VST/audioeffectx.h"
#include "SineWave.h"



class Lyons : public AudioEffectX
{
public:
	Lyons( audioMasterCallback audioMaster );
	~Lyons();
	virtual void processReplacing( float** inputs, float** outputs, VstInt32 sampleFrames );

private:
	SineWave* sineOscil1_;
	SineWave* sineOscil2_;
};

