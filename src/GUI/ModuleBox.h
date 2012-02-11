
#ifndef MODULEBOX_H
#define MODULEBOX_H

#include "Editor.h"
#include "Wire.h"
#include "../ModuleData.h"

class ProgramView;
class ModulePort;
class Wire;
class Label;


class ModuleBox : public CViewContainer
{
public:
	ModuleBox::ModuleBox( ProgramView* owner, ModuleData* data );
	~ModuleBox();

    CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );

    void realize();
    void focusAndSelect( bool focus, bool select );
	void moveTo( CRect& r );
	ModulePort* getPort( UINT16 portId, PortAlignment portAlign );
    void getDockPosition( UINT16 portId, PortAlignment portAlign, CPoint& pos );
    void collapseOrExpand( bool collapse );
    bool isCollapsed();

	void createWires();
	void removeWires( UINT16 targetId );
    void removeWire( const LinkInfo& linkInfo, PortAlignment portAlign );
	void positionWires( ModuleBox* movedBox );
	void drawWires( CDrawContext* pContext );
	void addWire( const LinkInfo& linkInfo, Wire* wire );
    void hitTestWires( const CRect& rc, WireMap& map );
    Wire* getWire( const LinkInfo& linkInfo );
	
	void setColors();
    void drawBackgroundRect( CDrawContext *pContext, CRect& updateRect );
	
	CLASS_METHODS( ModuleBox, CViewContainer )

   	ModuleData* data_;
    Label* label_;
	WireMap wires_;
  	bool selected_;
    bool focused_;
    CColor colBkgnd_, colFrame_, colPort_, colText_;

protected:
	void calculateSize();
    void setPortVisibility();
    void deleteWire( const LinkInfo& linkInfo );
	
	ProgramView* owner_;
    const CCoord portHeight_;
};





#endif // MODULEBOX_H