
#include "ModuleInfo.h"
#include "Utils.h"


ModuleInfoMap ModuleInfo::map_;
UINT16 ModuleInfo::instanceCounter_ = 0;

//-------------------------------------------------------
// XML Module Definitions
//-------------------------------------------------------

const char* masterXml =
	"<Module catalog=0 label=\"Master\" render=4 poly=0 boxstyle=3>"
		"<InPort id=0 label=\"In\" style=0 />"
		"<Param id=0 style=1 label=\"Volume\" dflt=0.35 steps=100 fmt=2 unit=\"db\" />"
	"</Module>";
	
const char* midiGateXml =
	"<Module catalog=1 label=\"Gate\" render=1 boxstyle=3>"
		"<OutPort id=0 label=\"Gate\" />"
	"</Module>";

const char* midiPitchXml =
	"<Module catalog=2 label=\"Freq\" render=3 boxstyle=3>"
		"<OutPort id=0 label=\"Freq\" />"
        "<Param id=0 style=2 label=\"Bend Range\" dflt=2 min=-48 max=48 steps=96 />"
        "<Param id=1 style=1 label=\"Portamento Time\" dflt=0 min=0 max=2000 steps=100 log=24 unit=\"msec\" fmt=1 />"
        "<Param id=2 style=3 label=\"Portamento Auto\" dflt=0 min=0 max=1 steps=1 />"
	"</Module>";

const char* midiInputXml =
	"<Module catalog=3 label=\"Input\" render=3 boxstyle=3>"
		"<OutPort id=0 label=\"Freq\" />"
        "<OutPort id=1 label=\"Gate\" />"
        "<Param id=0 style=2 label=\"Bend Range\" dflt=2 min=-48 max=48 steps=96 />"
        "<Param id=1 style=1 label=\"Portamento Time\" dflt=0 min=0 max=2000 steps=100 log=24 unit=\"msec\" fmt=1 />"
        "<Param id=2 style=3 label=\"Portamento Auto\" dflt=0 min=0 max=1 steps=1 />"
	"</Module>";

const char* sineWaveXml =
	"<Module catalog=10 label=\"Sine\" render=4 boxstyle=3>"
		"<InPort id=0 label=\"Freq\" style=1 />"
		"<InPort id=1 label=\"Amp\" style=1 />"
		"<OutPort id=0 label=\"Out\" />"
        "<Param id=0 style=0 label=\"Freq\"/>"
        "<Param id=1 style=0 label=\"Amp\"/>"
		"<Param id=2 style=2 label=\"Tune\" dflt=0 min=-48 max=48 steps=96 />"
		"<Param id=3 style=2 label=\"Finetune\" dflt=0 min=-1 max=1 steps=200 />"
	"</Module>";

const char* adsrXml =
	"<Module catalog=11 label=\"ADSR\" render=6 boxstyle=3>"
		"<InPort id=0 label=\"In\" style=1 />"
		"<InPort id=1 label=\"Gate\" style=1 />"
		//"<InPort id=2 label=\"Attack\" style=1 />"
		//"<InPort id=3 label=\"Decay\" style=1 />"
		//"<InPort id=4 label=\"Sustain\" style=1 />"
		//"<InPort id=5 label=\"Release\" style=1 />"
		"<OutPort id=0 label=\"Out\" />"
        "<Param id=0 style=0 label=\"In\"/>"
        "<Param id=1 style=0 label=\"Gate\"/>"
		"<Param id=2 style=1 label=\"Attack\"  min=0 max=1.2 dflt=0.1 steps=120 log=6 unit=\"sec\" fmt=1 />"
		"<Param id=3 style=1 label=\"Decay\"   min=0 max=1 dflt=0.0  steps=100 log=6 unit=\"sec\" fmt=1 />"
		"<Param id=4 style=1 label=\"Sustain\" min=0 max=1  dflt=0.75 steps=100 log=6 unit=\"db\" fmt=2 />"
		"<Param id=5 style=1 label=\"Release\" min=0 max=5 dflt=1  steps=100 log=6 unit=\"sec\" fmt=1 />"
	"</Module>";

const char* delayXml =
	"<Module catalog=12 label=\"Delay\" render=4 boxstyle=3 poly=0 >"
		"<InPort id=0 label=\"In\" style=0 />"
		"<InPort id=1 label=\"Time\" style=1 />"
		"<InPort id=2 label=\"Repeats\" style=1 />"
		"<InPort id=3 label=\"Gain\" style=1 />"
		"<OutPort id=0 label=\"Out\" />"
        "<Param id=1 style=1 label=\"Time\" dflt=0.5 />"
		"<Param id=2 style=1 label=\"Repeats\" dflt=0.5 />"
		"<Param id=3 style=1 label=\"Gain\" min=0 max=1 dflt=0.5 steps=100 log=6 unit=\"db\" fmt=2 />"
	"</Module>";


//-------------------------------------------------------
// class CtrlInfo
//-------------------------------------------------------

CtrlInfo::CtrlInfo() :
    id_( 0 ),
    style_( CS_SLIDER ),
    format_( NF_FLOAT ),
    value_( 1.0f ),
    defaultValue_( 1.0f ),
    min_( 0.0f ),
    max_( 1.0f ),
    logFactor_( 0.0f ),
    numSteps_( 100 ),
    resolution_( 1.0f )
{}


FLOAT CtrlInfo::makeLogarithmic( FLOAT value ) const
{
    value = normalize( value );
    value = value * pow( 1.122f, (1 - value) * -logFactor_ );
    value = denormalize( value );

    return value;
}


FLOAT CtrlInfo::makeLinear( FLOAT value ) const
{
    if( logFactor_ == 0.f ) 
        return value;

    FLOAT v    = normalize( value );
    FLOAT base = pow( 1.122f, -logFactor_ );
    FLOAT inc  = 0.1f;
    FLOAT x, y = 0.f;

    // This algorithm is probably crude, but I found no other way to inverse
    // the equation used in makeLogarithmic()
    //
    while( true )
    {
        x = y * pow( base, 1-y );

        if( x == v ) break;
        if( x < v ) {
            y += inc;
        }
        if( x > v ) {
            FLOAT oldInc = inc;
            inc *= 0.1f;
            if( inc < 0.000001f ) break;
            y -= oldInc;
        }
    }
	// test
	//FLOAT xTest = 1 - log( 1.122f ) / (-logFactor_ * log( v ));
	//TRACE( "x=%f xTest=%f\n", x, xTest );

    return denormalize( y );
}


FLOAT CtrlInfo::denormalize( FLOAT value ) const
{
    FLOAT range  = max_ - min_;
    FLOAT result = value * range + min_;

    return result;
}


FLOAT CtrlInfo::normalize( FLOAT value ) const
{
    FLOAT range  = max_ - min_;
    //FLOAT result = value / range - min_ / range;
    FLOAT result = value / range;

    return result;
}




//-------------------------------------------------------
// class ModuleInfo
//-------------------------------------------------------

ModuleInfo::ModuleInfo() :
	catalog_( MASTER ),
	renderType_( RENDER_AUDIO ),
    polyphony_( POLYPHONIC ),
	boxStyle_( (BoxStyle)(BS_VISIBLE | BS_LABEL) )
    {}
    

void ModuleInfo::buildMap()
{
	if( instanceCounter_++ == 0 )
    {
	    readModuleInfo( masterXml );
	    readModuleInfo( midiGateXml );
        readModuleInfo( midiPitchXml );
        readModuleInfo( midiInputXml );
	    readModuleInfo( sineWaveXml );
	    readModuleInfo( adsrXml );
	    readModuleInfo( delayXml );
    }
}


void ModuleInfo::cleanup()
{
	for( ModuleInfoMap::iterator it = map_.begin(); it!=map_.end(); it++  ) {
		delete it->second;
	}
	map_.clear();
}


string& ModuleInfo::getModuleLabel( Catalog catalog )
{
	ModuleInfo* info = getInfo( catalog );
	return info->label_;
}


RenderType ModuleInfo::getRenderType( Catalog catalog )
{
	ModuleInfo* info = getInfo( catalog );
	return info->renderType_;
}


UINT16 ModuleInfo::getBoxStyle( Catalog catalog )
{
	ModuleInfo* info = getInfo( catalog );
	return info->boxStyle_;
}


PortInfo* ModuleInfo::getInPortInfo( Catalog catalog, UINT16 portId )
{
    ModuleInfo* info   = getInfo( catalog );
    PortInfoList* list = &info->inPorts_;
    ASSERT( portId < list->size() );

    return &list->at( portId );
}


PortInfo* ModuleInfo::getOutPortInfo( Catalog catalog, UINT16 portId )
{
    ModuleInfo* info   = getInfo( catalog );
    PortInfoList* list = &info->outPorts_;
    ASSERT( portId < list->size() );

    return &list->at( portId );
}


CtrlInfo* ModuleInfo::getParamInfo( Catalog catalog, UINT16 paramId )
{
	ModuleInfo* info = getInfo( catalog );

	for( UINT i=0; i<info->ctrlInfos_.size(); i++ ) 
    {
        CtrlInfo* next = &info->ctrlInfos_[i];
        if( next->id_ == paramId )
            return next;
    }
	return NULL;
}


void ModuleInfo::getLinkInfo( Catalog catalog, UINT16 linkId, CtrlInfo* ctrlInfo )
{
    PortInfo* portInfo = getInPortInfo( catalog, linkId ); 
    ctrlInfo->value_   = portInfo->defaultValue_;
    ctrlInfo->style_   = portInfo->style_;
    ctrlInfo->label_   = portInfo->label_;
}


void ModuleInfo::readModuleInfo( const char* xml )
{
	ModuleInfo* info = new ModuleInfo();
	try
	{
    	Document doc = Document( "" );
	    doc.Parse( xml, false );

        Element* element = doc.FirstChildElement( "Module", true );
	    element->GetAttribute( "catalog",  (int*)&info->catalog_ );
	    element->GetAttribute( "label",     &info->label_ );
	    element->GetAttribute( "render",   (int*)&info->renderType_ );
        element->GetAttribute( "poly",     (int*)&info->polyphony_, false );
	    element->GetAttribute( "boxstyle", (int*)&info->boxStyle_ );

	    readPortInfos( element, info );
	    readParamInfos( element, info );
	}
	catch( const exception& e ) {
		TRACE( e.what() );
	}
	map_.insert( make_pair( info->catalog_, info ));
}


void ModuleInfo::readPortInfos( Element* module, ModuleInfo* info )
{
	info->inPorts_.clear();
    info->outPorts_.clear();

	Iterator< Element > itIn( "InPort" );
	for( itIn = itIn.begin( module ); itIn != itIn.end(); itIn++ )
	{
		PortInfo portInfo;
		itIn->GetAttribute( "id",    &portInfo.id_ );
		itIn->GetAttribute( "label", &portInfo.label_ );
        itIn->GetAttribute( "style", (int*)&portInfo.style_ );
        itIn->GetAttribute( "dflt",  &portInfo.defaultValue_, false );
                
        info->inPorts_.push_back( portInfo );
	}

	Iterator< Element > itOut( "OutPort" );
	for( itOut = itOut.begin( module ); itOut != itOut.end(); itOut++ )
	{
		PortInfo portInfo;
		itOut->GetAttribute( "id",    &portInfo.id_ );
		itOut->GetAttribute( "label", &portInfo.label_ );
        
        info->outPorts_.push_back( portInfo );
	}
}


void ModuleInfo::readParamInfos( Element* module, ModuleInfo* info )
{
	info->ctrlInfos_.clear();

	Iterator< Element > it( "Param" );
	for( it = it.begin( module ); it != it.end(); it++ )
	{
		CtrlInfo ctrlInfo;
        it->GetAttribute( "id",    (int*)&ctrlInfo.id_, false );
        it->GetAttribute( "value", &ctrlInfo.value_, false );
        it->GetAttribute( "style", (int*)&ctrlInfo.style_, false );
        it->GetAttribute( "label", &ctrlInfo.label_, false );
        it->GetAttribute( "unit",  &ctrlInfo.unit_, false );
        it->GetAttribute( "fmt",   (int*)&ctrlInfo.format_, false );
        it->GetAttribute( "dflt",  &ctrlInfo.defaultValue_, false );
        it->GetAttribute( "min",   &ctrlInfo.min_, false );
        it->GetAttribute( "max",   &ctrlInfo.max_, false );
        it->GetAttribute( "log",   &ctrlInfo.logFactor_, false );
        it->GetAttribute( "steps", &ctrlInfo.numSteps_, false );
        it->GetAttribute( "res",   &ctrlInfo.resolution_, false );

		info->ctrlInfos_.push_back( ctrlInfo );
	}
}


