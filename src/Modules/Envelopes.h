#ifndef ENVELOPES_H
#define ENVELOPES_H

#include "Definitions.h"
#include "Module.h"
#include "Buffer.h"



class ADSREnv : public Module
{
public:
	enum ParamId {
		PARAM_GATE     = 1,
		PARAM_ATTACK   = 2,
		PARAM_DECAY    = 3,
		PARAM_SUSTAIN  = 4,
		PARAM_RELEASE  = 5,
		PARAM_NUMPARAMS
	};

	ADSREnv();
	void initData();
    void initInPorts();

	void processAudio() throw();
	void processControl() throw();

	void setParameter( UINT16 paramId, FLOAT value, FLOAT modulation=0.f, INT16 voice=-1 );
    void setSentinel( bool sentinel ) { sentinel_ = sentinel; }

protected:  
	void keyOn( FLOAT amplitude, UINT16 voice );
	void keyOff( UINT16 voice );

	void setSampleRate( INT32 newRate, INT32 oldRate );

	// ranges for all times: 0-1, 1=10sec
	void setAttackRate( FLOAT time, INT16 voice=-1 );
	void setDecayRate( FLOAT time, INT16 voice=-1 );
	void setSustainLevel( FLOAT level, INT16 voice=-1 );
	void setReleaseRate( FLOAT time, INT16 voice=-1 );
	
	FLOAT calcRate( FLOAT time );

	Buffer<FLOAT> bufValue_, bufTarget_, bufDelta_, bufVelocity_;
	FLOAT *value_, *target_, *delta_, *velocity_;

    Buffer<FLOAT> bufAttackRate_, bufDecayRate_, bufSustainLevel_, bufReleaseRate_;
    FLOAT *attackRate_, *decayRate_, *sustainLevel_, *releaseRate_;

    Buffer<INT16> bufState_;
    INT16* state_;

	bool sentinel_;

	AudioIn audioIn_;
	AudioOut audioOut_;

    FLOAT* ptrAudioIn_;

	enum State {
		StateAttack,
		StateDecay,
		StateSustain,
		StateRelease,
		StateDone    
	};
};


#endif // ENVELOPES_H
