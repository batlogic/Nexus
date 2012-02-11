#ifndef _PARAMETERS_H
#define _PARAMETERS_H

#include "Definitions.h"

class Module;
class ModuleData;
class ParamInfo;



//-----------------------------------------------------------------
// VstParameters
// Matches between VstParameters and Modules.
//-----------------------------------------------------------------

class VstParameters
{
public:
	VstParameters() : currentId_( 0 ) {}

	void clear();
	void attach( Module* module );
	Module* getModule( UINT32 index );

	void setParameter( UINT32 index , FLOAT value );
	FLOAT getParameter( UINT32 index );
	
	void getParameterDisplay( UINT32 index, char* s );
	void getParameterName( UINT32 index, char* s );
	void getParameterLabel( UINT32 index, char* s ) ;

	INT16 getModuleId( UINT32 vstId );
	INT16 getParamId( UINT32 vstId );

protected:
	ParamInfo* getParamInfo( UINT32 index );

	typedef map< UINT32, Module* > ModuleMap;
	ModuleMap modules_;
	typedef map< UINT16, UINT16 > ParamMap;
	ParamMap params_;
	UINT16 currentId_;
};


#endif // PARAMETERS_H
