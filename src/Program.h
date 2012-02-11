
#ifndef PROGRAM_H
#define PROGRAM_H

#include "Definitions.h"
#include "ModuleData.h"

class Synth;
class Module;
class OutPort;


class Program : public ModuleDataList
{
public:
	Program();
    void operator=( const Program& ref );
	~Program();

	void clear( bool withDelete );
    void clearModules( bool withDelete );
    void compile( Synth* synth );
        	
	void addModule( ModuleData* data, Synth* synth );
    void addModule( ModuleData* data );
    void removeModule( ModuleData* data, Synth* synth );
    void addLink( ModuleData* targetData, LinkData& linkData, Synth* synth );
    void removeLinks( LinkDataList& list, Synth* synth );
    void removeLink( LinkData& linkData, Synth* synth, bool doCompile );
    void paste( Program* target, Synth* synth );
    
    void setPolyphony( ModuleData* data, PolyphonyType poly, Synth* synth );
    void setSampleRate( INT32 newRate, INT32 oldRate );
    void setNumVoices( UINT16 numVoices, Synth* synth );
    void setNumUnison( UINT16 numUnison )           { numUnison_ = numUnison; }
    void setUnisonSpread( UINT16 unisonSpread )     { unisonSpread_ = unisonSpread; }
    void setHold( bool hold )                       { hold_ = hold; }
    void setRetrigger( bool retrigger )             { retrigger_ = retrigger; }
    void setLegato( bool legato )                   { legato_ = legato; }
    void setRunning( bool running );

    ModuleData* find( INT32 moduleId );
    bool hasMaster()    { return find( MASTER ) != NULL; }
    void getName( char* name );

	string name_;
    UINT16 numVoices_;
    UINT16 numUnison_;
    UINT16 unisonSpread_;
    bool hold_;
    bool retrigger_;
    bool legato_;
    string category_;
    string comment_;
    bool ready_;

protected:
    void compileSink( Synth* synth );
    bool checkSentinel( ModuleData* data );
    
    void createModule( ModuleData* data, Synth* synth );
    void connectModule( ModuleData* targetData );
    void connectPort( ModuleData* sourceData, ModuleData* targetData, LinkData* linkData );

	void initModuleData( ModuleData* data );
	UINT32 createId( Catalog catalog );
    
    OutPort* getOutPort( ModuleData* data, UINT16 outputId );
    void getTargetModules( ModuleData* sourceData, ModuleDataList* list );
};


#endif // PROGRAM_H