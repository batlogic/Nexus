
#ifndef MASTER_H
#define MASTER_H

#include "Module.h"
#include "Ports.h"


class Master : public Module
{
public:
	Master();
    void initInPorts();
	void processAudio() throw();

	void attach();
	void setParameter( UINT16 paramId, FLOAT value, FLOAT modulation=0.f, INT16 voice=-1 );

	enum {
		PARAM_VOLUME
	};
    FLOAT value_;

protected:
	AudioIn audioIn_; 
	AudioOut audioOut_;

	FLOAT volume_;
    FLOAT* ptrAudioIn_;
};


#endif // MASTER_H