
#include "ModuleProxy.h"



//-----------------------------------------------
// ModuleProxy
//-----------------------------------------------

void ModuleProxy::clearModules() 
{
	for( UINT16 i=0; i<getNumVoices(); i++ ) {
		delete getModule( i );
	}
	modules_.clear();
}


void ModuleProxy::createModules( UINT16 numVoices ) 
{
	for( UINT16 i=0; i<numVoices; i++ )	
	{
		Module* m = createModule();
		m->init();
		modules_.push_back( m );
	}
	numModules_ = numVoices;
}


void ModuleProxy::setParameter( UINT16 localId, FLOAT value ) 
{
	for( UINT16 i=0; i<getNumVoices(); i++ ) {
		getModule( i )->setParameter( localId, value );
	}
}


FLOAT ModuleProxy::getParameter( UINT16 localId )
{
	Module* m = getModule( 0 );
	return m == NULL ? 0.0f : m->getParameter( localId );
}


void ModuleProxy::connectModules()
{
	for( UINT16 i=0; i<links_.size(); i++ )	
	{
		Link& link = links_[i];
		UINT16 numVoices = link.source_->getNumVoices();

		for( UINT16 j=0; j<numVoices; j++ ) 
		{
			Module* targetModule = getModule( j );
			Module* sourceModule = link.source_->getModule( j );
			try {
				targetModule->connect( sourceModule, link );
			}
			catch( const runtime_error& e ) {
				cerr << __FUNCTION__ << e.what() << endl;
			}
		}
	}
}


void ModuleProxy::soundVoice( UINT16 voice, bool sounding )
{
	if( voice < modules_.size() ) {
		modules_[ voice ]->setSounding( sounding );
	}
}


//-----------------------------------------------
// MonoProxy
//-----------------------------------------------

void MonoProxy::createModules( UINT16 numVoices ) 
{
	ModuleProxy::createModules( 1 );
}


