
#include "Program.h"
#include "Synth.h"
#include "ModuleInfo.h"
#include "Modules/Envelopes.h"




Program::Program() : 
    numVoices_( 1 ), 
    numUnison_( 1 ),
    unisonSpread_( 5 ),
    hold_( false ),
    retrigger_( false ),
    legato_( false ),
    ready_( false ),
    name_( "Default" )
{
    int i=0;
}


void Program::operator=( const Program& ref )
{
	clear( true );

	name_         = ref.name_;
    numVoices_    = ref.numVoices_;
    numUnison_    = ref.numUnison_;
    unisonSpread_ = ref.unisonSpread_;
    hold_         = ref.hold_;
    retrigger_    = ref.retrigger_;
    legato_       = ref.legato_;
    category_     = ref.category_;
    comment_      = ref.comment_;
    ready_        = ref.ready_;

    for( const_iterator it=ref.begin(); it!=ref.end(); it++ )	
    {
        ModuleData* newData = new ModuleData( **it );
        addModule( newData );
	}
}


Program::~Program()
{
    clear( true );
}


void Program::clear( bool withDelete )
{
	for( iterator it=begin(); it!=end(); it++ )	
    {
        ModuleData* data = *it;
        if( withDelete )
		    delete data;
	}
    ModuleDataList::clear();
}


void Program::clearModules( bool withDelete ) 
{
	for( iterator it=begin(); it!=end(); it++ )	{
		(*it)->clearModule( withDelete );
	}
    ready_ = false;
}


void Program::compile( Synth* synth )
{
	clearModules( true );

    for( iterator it=begin(); it!=end(); it++ ) {
		createModule( *it, synth );
	}

	for( iterator it=begin(); it!=end(); it++ )	{
        connectModule( *it );
	}

	compileSink( synth );
    ready_ = true;
}


void Program::compileSink( Synth* synth )
{
	synth->cleanup();

    bool sentinel      = false;
    ModuleData* master = find( MASTER );
    ModuleData* data   = master;
    if( master == NULL )    // nothing to do
        return;

    queue< ModuleData* > queue;
	queue.push( data );
	
	while( queue.empty() == false )
	{
		data           = queue.front();
        Module* module = data->owner_;

        if( sentinel == false ) {
            sentinel = checkSentinel( data );
        }

        if( module->renderType_ & (RENDER_AUDIO | RENDER_CONTROL) )
		{
			if( synth->containsModule( module ) == false )
                synth->push_back( module );

            for( UINT16 i=0; i<data->links_.size(); i++ )
            {
                LinkData& linkData = data->links_.at( i );
                ModuleData* next   = find( linkData.sourceId_ );
                if( next && next != data )
                    queue.push( next );                 // enqueue sources of current node
            }
		}
		queue.pop();
	}
	synth->compile( master->owner_ );
}


bool Program::checkSentinel( ModuleData* data )
{
    if( data->catalog_ == ADSR_ENV ) 
    {
        ADSREnv* adsr = dynamic_cast< ADSREnv* >( data->owner_ );
        adsr->setSentinel( true );
        return true;
    }
    return false;
}
    

void Program::createModule( ModuleData* data, Synth* synth ) 
{
	Module* module = data->createModule();
	module->init( data, synth );
}


void Program::addModule( ModuleData* data )
{
	ASSERT( data->id_ >= 0 );
    push_back( data );
}


void Program::addModule( ModuleData* data, Synth* synth )
{
    initModuleData( data );
    createModule( data, synth );
	addModule( data );
}


void Program::removeModule( ModuleData* data, Synth* synth )
{
    ModuleDataList targets;
    getTargetModules( data, &targets );

    for( INT16 i=0; i<(INT16)targets.size(); i++ )        // remove the links at the targets
    {
        ModuleData* next = targets.at( i );
        for( UINT16 j=0; j<next->links_.size(); )
	    {
	    	LinkData& linkData = next->getLink( j );
            if( linkData.sourceId_ == data->id_ ) {
                next->links_.remove( linkData );
            }
            else j++;
	    }
    }
    
    for( INT16 i=0; i<(INT16)data->links_.size(); )     // disconnect ports of own links
	{
	    LinkData& linkData = data->getLink( i );
        if( linkData.targetId_ == data->id_ ) 
        {
            data->links_.remove( linkData );
        }
        else i++; 
    }

    for( iterator it=begin(); it!=end(); it++ )         // remove and delete data
    {
        if( (*it)->id_ == data->id_ ) {
	        delete *it;
	        erase( it );
            break;
        }
    }
    compile( synth );
}


void Program::connectModule( ModuleData* targetData )
{
	for( UINT16 i=0; i<targetData->links_.size(); i++ )
	{
		LinkData& linkData     = targetData->getLink( i );
        ModuleData* sourceData = find( linkData.sourceId_ );
        connectPort( sourceData, targetData, &linkData );
	}
}


void Program::addLink( ModuleData* targetData, LinkData& linkData, Synth* synth )
{
    if( targetData->oldLinks_.contains( linkData ) ) {      // see if this link existed before
        linkData = targetData->oldLinks_.get( linkData );  
    }
    targetData->addLink( linkData );                       // the LinkData are stored with the target
	
    compile( synth );
}


void Program::removeLinks( LinkDataList& list, Synth* synth )
{
    for( UINT16 i=0; i<list.size(); i++ ) {
        removeLink( list.at( i ), synth, false );
    }
    compile( synth );
}


void Program::removeLink( LinkData& linkData, Synth* synth, bool doCompile )
{
    ModuleData* targetData = find( linkData.targetId_ );
    VERIFY( targetData );

    targetData->removeLink( linkData );

    if( doCompile ) 
        compile( synth );
}


void Program::setNumVoices( UINT16 numVoices, Synth* synth )
{
	numVoices_ = numVoices;

    for( iterator it=begin(); it!=end(); it++ ) {
        (*it)->owner_->setNumVoices( numVoices_ );
	}
    compile( synth );
}


void Program::setRunning( bool running )
{
    if( ready_ ) {
	    for( iterator it=begin(); it!=end(); it++ ) 
        {
		    Module* module = (*it)->owner_;
		    running ? module->resume() : module->suspend();
	    }
    }
}


void Program::setPolyphony( ModuleData* data, PolyphonyType poly, Synth* synth )
{
    data->polyphony_ = poly;
    compile( synth );
}


void Program::setSampleRate( INT32 newRate, INT32 oldRate )
{
	for( iterator it=begin(); it!=end(); it++ ) 
    {
		Module* module = (*it)->owner_;
		module->setSampleRate( newRate, oldRate );
	}
}


void Program::connectPort( ModuleData* sourceData, ModuleData* targetData, LinkData* linkData )
{
    if( sourceData && targetData )
    {
        Module* target = targetData->owner_;
        OutPort* port  = getOutPort( sourceData, linkData->outputId_ );
        ASSERT( port );

        if( port ) 
        {
            PortAdapterType adapter = NO_ADAPTER;
            if( sourceData->polyphony_ != targetData->polyphony_ ) {
                adapter = (sourceData->polyphony_ == MONOPHONIC) ? MONO_TO_POLY : POLY_TO_MONO;
            }
            port->connect( linkData, target, adapter );
        }
    }
}


void Program::initModuleData( ModuleData* data )
{
	ModuleInfo* info = ModuleInfo::getInfo( data->catalog_ );

    data->id_        = createId( data->catalog_ );
    data->label_     = info->label_;
    data->polyphony_ = info->polyphony_;
    
	for( UINT16 i=0; i<info->ctrlInfos_.size(); i++ ) 
	{
        const ParamData& paramData = info->ctrlInfos_.at( i );
        data->params_.add( paramData );
	}
}


void Program::paste( Program* target, Synth* synth )
{
    UINT32 firstId = target->size();
    for( iterator it = target->begin(); it != target->end(); it++ ) {   // find a unique id
        firstId = max( firstId, (UINT32)(*it)->id_+1 );
    }
    
    for( iterator it = begin(); it != end(); it++ ) 
    {
        ModuleData* data = *it;
	    Module* module   = data->createModule();
	    module->init( data, synth );

        UINT32 newId = firstId++;
        if( data->catalog_ == MASTER ) {
            if( target->hasMaster() == false ) {
                newId = MASTER;
            }
        }

        ModuleDataList targets;
        getTargetModules( data, &targets );

        for( INT16 i1=0; i1<(INT16)targets.size(); i1++ )        // adjust outgoing links to the new id
        {
            ModuleData* target = targets.at( i1 );
            for( UINT16 i2=0; i2<target->links_.size(); i2++ )
	         {
	    	    LinkData& linkData = target->getLink( i2 );
                if( linkData.sourceId_ == data->id_  ) {
                    linkData.sourceId_ = newId;
                }
	         }
        }
        
        for( INT16 i=0; i<(INT16)data->links_.size(); )    
        {
            LinkData& linkData     = data->links_.at( i );
            ModuleData* sourceData = find( linkData.sourceId_ );

            if( sourceData == NULL ) {               // erase links that go to modules not within the pasted modules
                data->links_.remove( linkData );
            }
            else {                                   // adjust incoming links to the new id
                linkData.targetId_ = newId;
                i++;
            }
        }
        data->id_ = newId;
    }

    for( iterator it = begin(); it != end(); it++ )   // connect the modules and copy them to target program
    {
        ModuleData* data = *it;
        connectModule( data );
        target->addModule( data );
    }
}


UINT32 Program::createId( Catalog catalog )
{
	if( catalog == MASTER ) {
        if( hasMaster() == false ) {
            return 0;
        }
    }

    UINT32 result = size();
    ModuleData* dummy = find( result );

    if( dummy != 0 )
    {
        result = 1;
        for( result=1; result<size(); result++ )
        {
            dummy = find( result );
            if( dummy == NULL ) 
                break;
        }
    }
    ASSERT( result <= size() );
    return result;
}


void Program::getName( char* name )	
{ 
	strcpy_s( name, MAX_PROG_NAME_LEN, name_.c_str() );	
}


ModuleData* Program::find( INT32 moduleId )
{
    for( iterator it = begin(); it != end(); it++ ) 
    {
        if( (*it)->id_ == moduleId ) {
            return *it;
        }
    }
    return NULL;
}


OutPort* Program::getOutPort( ModuleData* data, UINT16 outputId )
{
    return data ? data->owner_->outPorts_.at( outputId ) : NULL;
}


void Program::getTargetModules( ModuleData* sourceData, ModuleDataList* list )
{
    list->clear();

	for( iterator it=begin(); it!=end(); it++ )	{
        if( *it != sourceData ) 
        {
            ModuleData* next = *it;
            if( next->links_.hasSourceId( sourceData->id_ )) {
                list->push_back( next );
            }
        }
    }
}


