
#include "VstParameters.h"
#include "Module.h"
#include "ModuleInfo.h"
#include "ModuleData.h"



void VstParameters::clear()
{
	currentId_ = 0;
	modules_.clear();
	params_.clear();
}


void VstParameters::setParameter( UINT32 index, FLOAT value )
{
	Module* module = getModule( index );
	if( module ) {
		UINT16 paramId = getParamId( index );
		module->setParameter( paramId, value );
	}
}


FLOAT VstParameters::getParameter( UINT32 index )
{
	Module* module = getModule( index );
	if( module ) {
		UINT16 paramId = getParamId( index );
		return module->getParameter( paramId );
	}
	return 0.0f;
}


void VstParameters::getParameterName( UINT32 index, char* s )
{
	ParamInfo* info = getParamInfo( index );
	if( info ) {
		string& label = info->getLabel();
		strncpy_s( s, MAX_PARAM_STR_LEN, label.c_str(), _TRUNCATE );
	}
}


void VstParameters::getParameterDisplay( UINT32 index, char* s )
{
	ParamInfo* info = getParamInfo( index );
	Module* module  = getModule( index );
	if( info && module ) 
	{
		FLOAT value = module->getParameter( index );
		info->getValue( value, s );
	}
}


void VstParameters::getParameterLabel( UINT32 index, char* s )
{
	ParamInfo* info = getParamInfo( index );
	if( info ) {
		string& unit = info->getUnit();
		strncpy_s( s, MAX_PARAM_STR_LEN, unit.c_str(), _TRUNCATE );
	}
}


void VstParameters::attach( Module* module ) 
{
	ModuleMap::iterator pos = modules_.find( currentId_ );
	VERIFY( pos == modules_.end() );

	ModuleInfo* info = ModuleInfo::getInfo( module->getData()->catalog_ );
	ParamList* list  = &info->params_;

	for( ParamList::iterator it = list->begin(); it!=list->end(); it++ ) 
	{
		params_.insert( std::make_pair( currentId_, it->id_ ));
		modules_.insert( std::make_pair( currentId_, module ));
		currentId_++;
	}
}


Module* VstParameters::getModule( UINT32 vstId )
{
	ModuleMap::iterator pos = modules_.find( vstId );
	if( pos != modules_.end() ) {
		return pos->second;
	}
	return NULL;
}


INT16 VstParameters::getModuleId( UINT32 vstId )
{
	Module* module = getModule( vstId );
	return module ? module->getData()->id_ : -1;
}


INT16 VstParameters::getParamId( UINT32 vstId )
{
	ParamMap::iterator pos = params_.find( vstId );
	if( pos != params_.end() ) {
		return pos->second;
	}
	return -1;
}


ParamInfo* VstParameters::getParamInfo( UINT32 index )
{
	Module* module = getModule( index );
	if( module ) {
		UINT16 catalog = module->getData()->catalog_;
		UINT16 paramId = getParamId( index );
		return ModuleInfo::getParamInfo( catalog, paramId );
	}
	return NULL;
}

