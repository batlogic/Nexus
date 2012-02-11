
#ifndef MODULEINFO_H
#define MODULEINFO_H

#include "Definitions.h"
#include "XML/ticpp.h"
using namespace ticpp;

class ModuleInfo;



enum Catalog 
{
	// system modules
	MASTER	   = 0,
	MIDI_GATE  = 1,
	MIDI_PITCH = 2,
    MIDI_INPUT = 3,
    // others
	SINE_OSCIL = 10,
	ADSR_ENV   = 11,
	DELAY      = 12,
};


enum BoxStyle 
{
	BS_NOTHING = 0,
	BS_VISIBLE = 1,
	BS_LABEL   = 2
};

enum RenderType 
{
	RENDER_EVENT   = 1,
    RENDER_CONTROL = 2,
    RENDER_AUDIO   = 4
	
};

enum PolyphonyType 
{
    MONOPHONIC = 0,
    POLYPHONIC = 1
};

enum PortAlignment 
{
	IN_PORT  = 0,
	OUT_PORT = 1
};

enum ParamNumberFormat 
{ 
	NF_INT   = 0,
	NF_FLOAT = 1,
	NF_DB    = 2
};

enum CtrlStyle 
{
    CS_INVISIBLE = 0,
    CS_SLIDER    = 1,
    CS_BISLIDER  = 2,
    CS_CHECKBOX  = 3,
    CS_INTEDIT   = 4
};


//--------------------------------------------------
// class CtrlInfo
//--------------------------------------------------

class CtrlInfo 
{
public:
    CtrlInfo();

    CtrlInfo( const CtrlInfo& that )  
    {
        id_           = that.id_;
        style_        = that.style_;
        label_        = that.label_;
        unit_         = that.unit_;
        format_       = that.format_;
        value_        = that.value_;
        defaultValue_ = that.defaultValue_;
        min_          = that.min_;
        max_          = that.max_;
        logFactor_    = that.logFactor_;
        numSteps_     = that.numSteps_;
        resolution_   = that.resolution_;
    }
      
    UINT16 id_; 
    CtrlStyle style_;
	string label_;
	string unit_;
	ParamNumberFormat format_;
    FLOAT value_;
	FLOAT defaultValue_;
    FLOAT min_; 
    FLOAT max_;
    FLOAT logFactor_; 
    UINT16 numSteps_;
    FLOAT resolution_;

    FLOAT makeLogarithmic( FLOAT value ) const;
    FLOAT makeLinear( FLOAT value ) const;

protected:
    FLOAT denormalize( FLOAT value ) const;
    FLOAT normalize( FLOAT value ) const;
};

typedef vector< CtrlInfo > CtrlInfoList;


//--------------------------------------------------
// class PortInfo
//--------------------------------------------------

class PortInfo 
{
public:
    PortInfo() :
        id_( 0 ),
        style_( CS_INVISIBLE ),
        defaultValue_( 1.f )
    {};

	UINT16 id_;
	string label_;
    CtrlStyle style_;
    FLOAT defaultValue_;
};
typedef vector< PortInfo > PortInfoList;


//--------------------------------------------------
// class ModuleInfo
//--------------------------------------------------

typedef map< UINT16, ModuleInfo* > ModuleInfoMap;


class ModuleInfo
{
public:
    ModuleInfo();

	static void buildMap();
	static ModuleInfoMap* getMap() { return &map_; }
	static ModuleInfo* getInfo( Catalog catalog ) { return map_[ (UINT16)catalog ]; }

	static string& getModuleLabel( Catalog catalog );
	static RenderType getRenderType( Catalog catalog );
	static UINT16 getBoxStyle( Catalog catalog );

    static PortInfo* getInPortInfo( Catalog catalog, UINT16 portId );
    static PortInfo* getOutPortInfo( Catalog catalog, UINT16 portId );

	static CtrlInfo* getParamInfo( Catalog catalog, UINT16 paramId );
    static void getLinkInfo( Catalog catalog, UINT16 linkId, CtrlInfo* ctrlInfo );
	
	static void cleanup();

	Catalog catalog_;
	string label_;
	RenderType renderType_;
    PolyphonyType polyphony_;
	BoxStyle boxStyle_;
	PortInfoList inPorts_;
	PortInfoList outPorts_;
	CtrlInfoList ctrlInfos_;

	static ModuleInfoMap map_;
    static UINT16 instanceCounter_;

protected:
	static void readModuleInfo( const char* xml );
    static void readPortInfos( Element* module, ModuleInfo* info );
    static void readParamInfos( Element* module, ModuleInfo* info );

};


#endif MODULEINFO_H