
#ifndef MODULELINK_H
#define MODULELINK_H

#include "BaseDef.h"

class Module;
class ModuleProxy;


struct Link
{
	Link( ModuleProxy* source, UINT16 inputId, UINT16 outputId ) :
		source_( source ),
		inputId_( inputId ),
		outputId_( outputId )
		{}

	ModuleProxy* source_;
	UINT16 inputId_, outputId_;
};







#endif // MODULELINK_H