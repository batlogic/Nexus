
#include "ModuleData.h"
#include "ModuleInfo.h"

#include "Module.h"
#include "Modules/Master.h"
#include "Modules/MidiModules.h"
#include "Modules/Envelopes.h"
#include "Modules/SineOscil.h"
#include "Modules/Delay.h"




//----------------------------------------------------------------
// class ParamData
//----------------------------------------------------------------

ParamData::ParamData() : CtrlInfo()
{
    init();
}


ParamData::ParamData( const CtrlInfo& ctrlInfo ) : CtrlInfo( ctrlInfo )
{
    init();
}


void ParamData::init()
{
    veloSens_           = 0.0f;
    keyTrack_           = 0.0f;
    controller_         = -1;
    controllerMin_      = 0.0f;
    controllerMax_      = 127.0f;
    controllerSoft_     = true;
    controllerAccepted_ = false;
}


bool ParamData::scaleController( INT16 controller, FLOAT& value )
{
    if( controller_ == -1 || controller != controller_ )
        return false;
    
    if( controllerMax_ < controllerMin_ )                   // reverse range if min > max
        value = controllerMin_ - value;

    FLOAT range = abs( controllerMax_ - controllerMin_ );
    value /= range;                                         // normalize to 1
            
    if( max_ < min_ ) 
        value = min_ - value;

    range        = abs( max_ - min_ );
    FLOAT raster = range / numSteps_; 
    value       *= range;
    value        = makeLogarithmic( value );                // apply logarithmic scaling factor

    if( numSteps_ > 0 ) {                                   // rasterize
        value = (INT16)(value / raster) * raster;
    }

    if( controllerSoft_ && controllerAccepted_ == false )   // soft takeover
    {
        FLOAT diff = abs( value_ - value );
        if( diff > raster*2 ) {
            return false;
        }
        controllerAccepted_ = true;
    }
    return true;
}


void ParamData::resetController() 
{ 
    controllerAccepted_ = false; 
}



//----------------------------------------------------------------
// class ParamDataMap
//----------------------------------------------------------------

void ParamDataMap::add( const ParamData& paramData )	
{ 
    insert( make_pair( paramData.id_, paramData )); 

    ParamData& data = at( paramData.id_ );
    data.value_ = data.defaultValue_;
}


void ParamDataMap::set( ParamData& data )	
{ 
	iterator pos = find( data.id_ );
	if( pos != end() ) {
		pos->second = data;
	}
	else {
		TRACE( "Error: parameter [%d] not found [in %s, %s, line %d]\n", data.id_, __FILE_ONLY__, __FUNCTION__, __LINE__ );
	}
}


void ParamDataMap::set( UINT16 id, FLOAT value )
{
	iterator pos = find( id );
	if( pos != end() ) {
		pos->second.value_ = value;
	}
	else {
		TRACE( "Error: parameter [%d] not found [in %s, %s, line %d]\n", id, __FILE_ONLY__, __FUNCTION__, __LINE__ );
	}
}


ParamData* ParamDataMap::get( UINT16 id )				
{ 
	iterator pos = find( id );
	return pos != end() ? &pos->second : NULL;
}


void ParamDataMap::makeDefaults( Catalog catalog )
{
	ModuleInfo* info = ModuleInfo::getInfo( catalog );

	for( UINT16 i=0; i<info->ctrlInfos_.size(); i++ )
	{
        const ParamData& paramData = info->ctrlInfos_.at( i );
        add( paramData );
	}
}




//----------------------------------------------------------------
// class LinkInfo
//----------------------------------------------------------------

LinkInfo::LinkInfo() :
    sourceId_( 0 ),
    targetId_( 0 ),
    outputId_( 0 ),
    inputId_( 0 )
{}


bool LinkInfo::operator==( const LinkInfo& ref ) const 
{
	return 
        ref.sourceId_ == sourceId_ &&
		ref.targetId_ == targetId_ &&
		ref.outputId_ == outputId_ &&
		ref.inputId_  == inputId_;
}


bool LinkInfo::operator==( LinkInfo* ptr ) const 
{
    return operator==( *ptr );
}


bool LinkInfo::operator<( const LinkInfo& ref ) const
{
	if( ref.targetId_ != targetId_ ) return ref.targetId_ < targetId_;
    if( ref.sourceId_ != sourceId_ ) return ref.sourceId_ < sourceId_;
    if( ref.inputId_  != inputId_ )  return ref.inputId_  < inputId_;
    if( ref.outputId_ != outputId_ ) return ref.outputId_ < outputId_;

    return false;
}



//----------------------------------------------------------------
// class LinkData
//----------------------------------------------------------------

LinkData::LinkData() : 
    LinkInfo(), 
    ParamData()
    {}


LinkData::LinkData( const LinkInfo& linkInfo ) :
    LinkInfo( linkInfo ),
    ParamData()
    {}



//----------------------------------------------------------------
// class LinkDataList
//----------------------------------------------------------------

LinkData& LinkDataList::get( const LinkData& linkData )
{
    iterator pos = find( begin(), end(), linkData );
    VERIFY( pos != end() );
    return *pos;
}


void LinkDataList::set( const LinkData& linkData )
{
    iterator pos = find( begin(), end(), linkData );
    VERIFY( pos != end() );
    *pos = linkData;
}


void LinkDataList::remove( const LinkData& linkData )
{
    LinkDataList::iterator pos = find( begin(), end(), linkData );
    ASSERT( pos != end() );
    if( pos != end() ) {
        erase( pos );
    }
}


INT16 LinkDataList::getIndex( const LinkData& linkData )
{
    for( UINT16 i=0; i<size(); i++ ) {
        if( at( i ) == linkData ) {
            return i;
        }
    }
    return -1;
}


bool LinkDataList::contains( const LinkData& linkData )
{
    iterator pos = find( begin(), end(), linkData );
    return pos != end();
}


bool LinkDataList::hasSourceId( UINT32 sourceId )
{
    for( UINT32 i=0; i<size(); i++ )
    {
        LinkData& linkData = at( i );
        if( linkData.sourceId_ == sourceId ) {
            return true;
        }
    }
    return false;
}



//----------------------------------------------------------------
// class ModuleData
//----------------------------------------------------------------

ModuleData::ModuleData( Catalog catalog ) : 
    owner_( NULL ),
	catalog_( catalog ), 
	id_( -1 ),
	polyphony_( POLYPHONIC ),
	xPos_( 0 ),
	yPos_( 0 ),
    collapsed_( false )
    {}


ModuleData::~ModuleData()
{
    clearModule( true );
}


Module* ModuleData::createModule()
{
	Module* module = NULL;
    switch( catalog_ ) 
	{
	case MASTER:	 module = new Master(); break;
	case MIDI_GATE:	 module = new MidiGate(); break; 
	case MIDI_PITCH: module = new MidiPitch(); break;
    case MIDI_INPUT: module = new MidiInput(); break;

	case SINE_OSCIL: module = new SineOscil(); break; 
	case ADSR_ENV:	 module = new ADSREnv(); break;
	case DELAY:		 module = new Delay(); break;
    default:         EXCEPTION( runtime_error, "catalog %d does not exist", catalog_ );
	}

    module->renderType_ = ModuleInfo::getRenderType( catalog_ );
	owner_ = module;

    return module;
}


void ModuleData::clearModule( bool withDelete )
{
    if( owner_ ) {
        if( withDelete )
            delete owner_;
        owner_ = NULL;
    }
}


LinkData& ModuleData::getLink( UINT16 idx )
{
    ASSERT( idx >= 0 && idx < links_.size() );
    return links_.at( idx );
}


void ModuleData::addLink( LinkData& linkData )				
{ 
    linkData.targetId_ = id_;
    links_.push_back( linkData );
    linkData = links_.back();
}


void ModuleData::removeLink( LinkData& linkData )
{
    linkData = links_.get( linkData );
    links_.remove( linkData );

    if( oldLinks_.contains( linkData )) {
        oldLinks_.remove( linkData );
    }
    oldLinks_.push_back( linkData );
}




