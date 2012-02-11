#ifndef _VSTWRAPPER_H
#define _VSTWRAPPER_H

#include "VST/audioeffectx.h"
#include "Synth.h"

class VstEditor;



class VstWrapper : public AudioEffectX
{
public:
	VstWrapper( audioMasterCallback audioMaster );
	~VstWrapper();

	void process( float** inputs, float** outputs, VstInt32 numFrames );
	void processReplacing( float** inputs, float** outputs, VstInt32 numFrames );
	VstInt32 processEvents( VstEvents* events );

	void open();
	void close();
	void suspend();
	void resume();

	bool beginSetProgram()										{ return synth_->beginSetProgram(); }
	bool endSetProgram()										{ return synth_->endSetProgram(); }

	VstInt32 getProgram()										{ return synth_->getProgramNumber(); }
	void setProgram( VstInt32 program )							{ synth_->setProgram( program ); }
	void setProgramName( char* name )							{ synth_->setProgramName( name ); }
	void getProgramName( char* name )							{ synth_->getProgramName( name ); }

	void setSampleRate( float sampleRate );

	VstInt32 canDo( char* text );
	bool getVendorString( char* text );
    bool getEffectName( char* name );
	bool getProductString( char* text );

    bool updateDisplay();

	VstIntPtr dispatcher(  	
			VstInt32	opcode,
			VstInt32	index,
			VstIntPtr	value,
			void*		ptr,
			float  		opt );

protected:
	Synth* synth_;
	VstEditor* editor_;
};

#endif // VSTWRAPPER_H
