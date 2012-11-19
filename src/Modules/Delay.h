#ifndef DELAY_H
#define DELAY_H

#include "Definitions.h"
#include "Module.h"
#include "Buffer.h"


class Delay : public Module
{
public:
	Delay();
	void initData();
    void initInPorts();
	
	void processAudio() throw();
	void resume();
	void setParameter( UINT16 paramId, FLOAT value, FLOAT modulation=0.f, INT16 voice=-1 );

	enum ParamId {
		PARAM_DELAYTIME = 1,
		PARAM_FEEDBACK  = 2,
		PARAM_GAIN      = 3
	};

protected:
	UINT32 delayTime_;
	FLOAT feedback_;
	FLOAT gain_;
	UINT32 bufferSize_;

	Buffer<UINT32> bufCursor_;
	UINT32* cursor_;
	
	Buffer< FLOAT > bufDelay_;
    FLOAT* buffer_;

protected:
	AudioIn audioIn_; 
	AudioOut audioOut_;
    FLOAT* ptrAudioIn_;
};



#endif // DELAY_H