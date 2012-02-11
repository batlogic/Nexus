
#ifndef SINK_H
#define SINK_H


#include "Definitions.h"
#include "Module.h"


//-------------------------------------------
// class Sink
//-------------------------------------------

class Sink : public ModuleList
{
public:
	Sink();

	void cleanup();
	void compile( Module* master );
    bool contains( Module* module );
    void setSampleRate( INT32 sampleRate );
	FLOAT process();

	FLOAT* output_;
	UINT16 numModules_;

protected:
	UINT16 controlRateDiv_;
	INT16 counter_;
    FLOAT dummyMaster_;
};


__forceinline FLOAT Sink::process()
{
    if( --counter_ <= 0 ) {
		counter_ = controlRateDiv_;
	}
    for( Module** m = _Myfirst; m != _Mylast; m++ )
	{
		if( counter_ == controlRateDiv_ ) 
        {
			if( (*m)->renderType_ & RENDER_CONTROL )
                (*m)->processControl();
		}

        if( (*m)->renderType_ & RENDER_AUDIO )
            ((*m)->*(*m)->ptrProcessAudio_)();
	}
    return *output_;
}


#endif // SINK_H