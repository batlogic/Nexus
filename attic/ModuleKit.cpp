
#include <queue>

#include "ModuleKit.h"
#include "ModuleData.h"
#include "ModuleInfo.h"
#include "Synth.h"



//ModuleKit::~ModuleKit() {
//	clear();
//}


//Module* ModuleKit::getModule( INT16 id )
//{
//	iterator pos = find( id );
//	VERIFY( pos != end() );
//
//	return pos->second;
//}
//
//
//Module* ModuleKit::createModule( ModuleData* data, Synth* synth ) 
//{
//	Module* module = data->createModule();
//	module->init( data, synth );
//	insert( std::make_pair( data->id_, module ));
//
//	return module;
//}


//void ModuleKit::removeModule( UINT16 id )
//{
//	iterator pos = find( id );
//	ASSERT( pos != end() );
//	delete pos->second;
//	erase( pos );
//}
//


void ModuleKit::compile( Synth* synth )
{
/*
	Sink* sink = synth->getSink();
	sink->cleanup();

	ModuleList sources;
    Module* master = getModule( MASTER );
	Module* module = master;

	queue< Module* > queue;
	queue.push( module );
	
	while( queue.empty() == false )
	{
		module = queue.front();

		if( module->getType() == AUDIO_MODULE )
		{
			sink->push_back( module );
			module->getAudioSources( &sources );

			for( UINT16 i=0; i<sources.size(); i++ )	// enqueue children of current node
			{
				queue.push( sources.at( i ));
			}
		}
		queue.pop();
	}
	sink->compile( master );
*/
}


//void ModuleKit::clear() 
//{
//	for( iterator it=begin(); it!=end(); it++ )	
//	{
//		UINT16 id = it->first;
//		delete it->second;
//	}
//	map< UINT16, Module* >::clear();
//}


//void ModuleKit::setSampleRate( INT32 sampleRate, INT32 oldRate )
//{
//	for( iterator it=begin(); it!=end(); it++ ) {
//		Module* m = it->second;
//		m->setSampleRate( sampleRate, oldRate );
//	}
//}
//
//
//void ModuleKit::suspend()
//{
//	for( iterator it=begin(); it!=end(); it++ ) {
//		Module* m = it->second;
//		m->suspend();
//	}
//}
//
//
//void ModuleKit::resume()
//{
//	for( iterator it=begin(); it!=end(); it++ ) {
//		Module* m = it->second;
//		m->resume();
//	}
//}















