
#ifndef MODULEDATA_H
#define MODULEDATA_H

#include "Definitions.h"
#include "ModuleInfo.h"
#include "Buffer.h"

class Module;



//-------------------------------------------
// class ParamData
//-------------------------------------------

class ParamData : public CtrlInfo
{
public:
    ParamData();
    ParamData( const CtrlInfo& ctrlInfo );
    void init();
    bool scaleController( INT16 controller, FLOAT& value );
    void resetController();

    FLOAT veloSens_;
    FLOAT keyTrack_;
    INT16 controller_;
    FLOAT controllerMin_;
    FLOAT controllerMax_;
    bool controllerSoft_;

protected:
    bool controllerAccepted_;
};



//-------------------------------------------
// class ParamDataMap
//-------------------------------------------

class ParamDataMap : public map< UINT16, ParamData >
{
public:
    void add( const ParamData& paramData );
    void set( UINT16 id, FLOAT value );
	void set( ParamData& data );
	ParamData* get( UINT16 id );

	void makeDefaults( Catalog catalog );
};



//-------------------------------------------
// class LinkInfo
//-------------------------------------------

class LinkInfo
{
public:
    LinkInfo();

	bool operator==( const LinkInfo& ref ) const;
    bool operator==( LinkInfo* ptr ) const;
	bool operator<( const LinkInfo& ref ) const;

    UINT32 sourceId_;
    UINT32 targetId_;
	UINT16 outputId_;
	UINT16 inputId_;
};



//-------------------------------------------
// class LinkData
//-------------------------------------------

class LinkData : public LinkInfo, public ParamData
{
public:
    LinkData();
    LinkData( const LinkInfo& linkInfo );
};


class LinkDataList : public vector< LinkData > 
{
public:
    LinkData& get( const LinkData& linkData );
    void set( const LinkData& linkData );
    void remove( const LinkData& linkData );
    INT16 getIndex( const LinkData& linkData );
    bool contains( const LinkData& linkData );
    bool hasSourceId( UINT32 sourceId );
};



//-------------------------------------------
// class ModuleData
//-------------------------------------------

class ModuleData 
{
public:
	ModuleData( Catalog catalog=MASTER );
    ~ModuleData();

	Module* createModule();
    void clearModule( bool erase );
	ModuleData* createData();

	void addLink( LinkData& linkData );
    LinkData& getLink( UINT16 idx );
    void removeLink( LinkData& linkData );

	void setPosition( long x, long y )		    { xPos_ = x; yPos_ = y; }
	void setParameters( ParamDataMap& params )	{ params_ = params; }
	void setParameter( UINT16 id, FLOAT value ) { params_.set( id, value ); }

	string label_;
    INT32 id_;
    Module* owner_;
    Catalog catalog_;
    PolyphonyType polyphony_;

	ParamDataMap params_;
	LinkDataList links_;
    LinkDataList oldLinks_;
    
	long xPos_;
	long yPos_;
    bool collapsed_;
};

typedef vector< ModuleData* > ModuleDataList;



#endif // MODULEDATA_H