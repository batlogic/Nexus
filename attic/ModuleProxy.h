
#ifndef _MODULEPROXY_H
#define _MODULEPROXY_H

#include "BaseDef.h"
#include "Module.h"
#include "Parameters.h"
#include "ModuleLink.h"
#include "ModuleIO.h"


class ModuleProxy : public ParameterObserver
{
public:
	ModuleProxy() : numModules_( 0 ) {}
	virtual ~ModuleProxy()	{ clearModules(); }

	void process( void );

	virtual void clearModules();
	virtual void createModules( UINT16 numVoices );
	virtual Module* createModule() = 0;
	ModuleList* getModules()				{ return &modules_; }
	virtual Module* getModule( UINT16 idx ) { return modules_.at( idx ); }
	void connectModules();

	virtual bool isAudio() const    { return true; }

	void addLink( Link link )		{ links_.push_back( link ); }
	Link* getLink( INT16 idx )		{ return &links_.at( idx ); }
	INT16 numLinks()    			{ return links_.size(); }

	virtual void setParameter( UINT16 localId, FLOAT value ); 
	virtual FLOAT getParameter( UINT16 localId );

	void soundVoice( UINT16 voice, bool sounding );

protected:
	UINT16 getNumVoices()		   	{ return modules_.size(); }

	ModuleList modules_;				// list of realized modules
	vector<Link> links_;				// links to other modules
	UINT16 numModules_;					// number of realized modules=number of voices
};



inline void ModuleProxy::process( void )
{
	for( UINT16 i=0; i<numModules_; i++ )
	{
		Module* m = modules_[i];
		m->process();
	}
}



class PolyProxy : public ModuleProxy
{
};


class MonoProxy : public ModuleProxy
{
public:
	void createModules( UINT16 numVoices );
	Module* getModule( UINT16 idx ) { return modules_.at( 0 ); }
};


class EventProxy : public PolyProxy
{
public:
	bool isAudio() const { return false; }
};




#endif // MODULEPROXY_H
