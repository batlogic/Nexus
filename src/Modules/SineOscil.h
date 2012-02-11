#ifndef SINEOSCIL_H
#define SINEOSCIL_H


#include "Definitions.h"
#include "Buffer.h"
#include "Module.h"



class SineOscil : public Module
{
public:
	SineOscil();
    ~SineOscil();

	void initData();
    void initInPorts();
    void checkPorts();
	void setParameter( UINT16 paramId, FLOAT value, FLOAT modulation=0.f, INT16 voice=-1 );

	void processAudio() throw();
    void processAudioFm();
    void processAudioAm();
    void processAudioFmAm();

	enum ParamId {
        PARAM_FREQUENCY   = 0,
        PARAM_AMPLITUDE   = 1,
		PARAM_TUNING      = 2,
		PARAM_FINETUNING  = 3,
	};

protected:
	void addPhase( FLOAT phase );
	void addPhaseOffset( FLOAT phaseOffset );

	void setSampleRate( INT32 newRate, INT32 oldRate );
    void setTuning( FLOAT paramValue );
    void setFineTuning( FLOAT paramValue );
    void setRate( UINT16 voice );

	void makeWaveTable();

	Buffer<FLOAT> bufRate_, bufTime_, bufAmplitude_, bufFreq_;
	FLOAT *time_, *amplitude_, *rate_, *freq_;
	
	FLOAT phaseOffset_;

	double tuning_;
	double fineTuning_;

    AudioIn  fmInput_;
    AudioIn  amInput_;
	AudioOut audioOut_;
    FLOAT* ptrFmInput_;
    FLOAT* ptrAmInput_;
    
	static Buffer<FLOAT> bufTable_;
	static FLOAT* table_;
	static UINT16 tableSize_; // = 2048;
};



inline void SineOscil::setRate( UINT16 voice )
{
    rate_[ voice ] = (tableSize_ * freq_[ voice ] * (FLOAT)tuning_ * (FLOAT)fineTuning_) / sampleRate_;
}


#endif  // SINEOSCIL

