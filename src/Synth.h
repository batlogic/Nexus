#ifndef SYNTH_H
#define SYNTH_H


#include "MidiEvent.h"
#include "Module.h"
#include "Ports.h"
#include "Polyphony.h"
#include "Program.h"
#include "Bank.h"

#include "Profiler.h"
#include "Trace.h"


class Editor;
class SoundDevices;


typedef vector< Module* > ModuleList;

class Synth : public ModuleList
{
public:
	Synth( Editor* editor=NULL );
	
	void open();
	void close();
	void cleanup();

	void process( FLOAT** inputs, FLOAT** outputs, INT32 numFrames );
    void processReplacing( FLOAT** inputs, FLOAT** outputs, INT32 numFrames ) throw();
	void processEvent( const MidiEvent& evt );
	void suspend();
	void resume();

	INT32 getProgramNumber();
	Program* getProgram()					{ return program_; }
	void setProgram( INT32 programNum );
	void setProgramName( char* name );
	void getProgramName( char* name );
	bool getProgramNameIndexed( INT32 category, INT32 index, char* name );

    Bank* getBank() { return &bank_; }
    void loadBank( const string& path="" );
    void newBank( const string& path, const string& name, bool autosave );

    bool beginSetProgram();
    bool endSetProgram();

	void addModule( ModuleData* data );
	void removeModule( ModuleData* data );
	bool addLink( ModuleData* data, LinkData& linkData );
    void removeLink( LinkData& linkData );
    void removeLinks( LinkDataList& list );

    void setPolyphony( ModuleData* data, PolyphonyType poly );

	Polyphony* getPolyphony()				{ return &polyphony_; }

	INT32 getSampleRate()					{ return sampleRate_; }
	void setSampleRate( INT32 sampleRate);

	UINT16 getNumVoices()					{ return program_ ? program_->numVoices_ : 0; }
	void setNumVoices( UINT16 numVoices );

    FLOAT getTuning() const                 { return tuning_; }
    void setTuning( INT16 tuningInCent )    { tuning_ = tuningInCent / 100.f; }

    void setMonitor( INT16 numSounding, FLOAT pitch, FLOAT gate, UINT16 flags );
    bool isReady()      { return ready_; }
    bool isRunning()    { return running_; }

	void compile( Module* master );
    bool containsModule( Module* module );

    void beginProfiling()   { perfAverage_.resize( 100000 );	}
    void endProfiling()     { perfAverage_.average(); }

protected:
	void setRunning( bool ready );
    void checkAppState();
    	
    Bank bank_;
    Polyphony polyphony_;
    SoundDevices* devices_;
	Editor* editor_;
	Program* program_;

	INT32 sampleRate_;
    FLOAT tuning_;
	bool ready_, running_;

	UINT16 controlRateDiv_;
	INT16 frameCounter_;
    FLOAT dummyMaster_;
    FLOAT* master_;

	//PerformanceCounter perfCounter_;
	TimeStampCounter perfCounter_;
	PerformanceAverage perfAverage_;
};


//-------------------------------------------------------------
// Main audio processing loop. Existing data are overwritten. The input
// and the output is a block of data. Each block is processed samplewise 
// by the currently loaded modules.
//
// FLOAT** inputs	Array of pointers to the data
// FLOAT** outputs	Array of pointers to where the datacan be written to
// INT32 numFrames	Number of sample frames to be processed
//-------------------------------------------------------------
__forceinline void Synth::processReplacing( FLOAT** inputs, FLOAT** outputs, INT32 numFrames ) throw()
{
	FLOAT* out1 = outputs[0];	
	FLOAT* out2 = outputs[1];
	perfCounter_.begin();

	for( INT32 i=0; i<numFrames && ready_ && running_; i++ ) 
	{
        if( --frameCounter_ <= 0 ) {
		    frameCounter_ = controlRateDiv_;
	    }
        for( Module** m = _Myfirst; m != _Mylast; m++ )
	    {
		    if( frameCounter_ == controlRateDiv_ ) 
            {
			    if( (*m)->renderType_ & RENDER_CONTROL )
                    (*m)->processControl();
		    }

            if( (*m)->ptrProcessAudio_ != NULL )
                ((*m)->*(*m)->ptrProcessAudio_)();
	    }
		*out1++ = *master_; 
		*out2++ = *master_;
	}
	
    double timePerFrame = perfCounter_.end() / (double)numFrames;
    perfAverage_.add( timePerFrame );
}

//-------------------------------------------------------------
// Main audio processing loop. After processing the data are accumulated to the data.
// The input and the output is a block of data. Each block is processed samplewise
// by the currently loaded modules.
//
// FLOAT** inputs	Array of pointers to the data
// FLOAT** outputs	Array of pointers to where the datacan be written to
// INT32 numFrames	Number of sample frames to be processed
//-------------------------------------------------------------
__forceinline void Synth::process( FLOAT** inputs, FLOAT** outputs, INT32 numFrames )
{
	FLOAT* out1 = outputs[0];	
	FLOAT* out2 = outputs[1];

	for( INT32 i=0; i<numFrames && ready_ && running_; i++ ) 
	{
        if( --frameCounter_ <= 0 ) {
		    frameCounter_ = controlRateDiv_;
	    }
        for( Module** m = _Myfirst; m != _Mylast; m++ )
	    {
		    if( frameCounter_ == controlRateDiv_ ) 
            {
			    if( (*m)->renderType_ & RENDER_CONTROL )
                    (*m)->processControl();
		    }

            if( (*m)->renderType_ & RENDER_AUDIO )
                ((*m)->*(*m)->ptrProcessAudio_)();
	    }
		*out1++ += *master_;
		*out2++ += *master_;
	}
}







#endif // SYNTH_H