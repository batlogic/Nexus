
#ifndef _MODULEKIT_H
#define _MODULEKIT_H

#include "Definitions.h"
#include "Module.h"

class ModuleData;
class Synth;


class ModuleKit : public map< UINT16, Module* >
{
public:
	ModuleKit() {}
	//~ModuleKit();
	//void clear();

	//Module* getModule( INT16 id );	
	//Module* createModule( ModuleData* data, Synth* synth );
	//void removeModule( UINT16 id );
	void compile( Synth* synth );

	//void setSampleRate( INT32 sampleRate, INT32 oldRate );

	//void suspend();
	//void resume();
};


#endif // MODULEKIT_H
