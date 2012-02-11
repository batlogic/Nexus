
#ifndef MODULEPORT_H
#define MODULEPORT_H

#include "Editor.h"
#include "../ModuleData.h"

class PortInfo;
class ModuleBox;


class ModulePort : public CView
{
public:
	ModulePort( const CRect& size, PortInfo* info, PortAlignment align, ModuleBox* owner );

	CMouseEventResult onMouseEntered( CPoint& pos, const long& buttons );
	CMouseEventResult onMouseExited( CPoint& pos, const long& buttons );
	CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );
	CMouseEventResult onMouseMoved( CPoint& pos, const long& buttons );
	
	void draw( CDrawContext *pContext );

	bool testDocking( const CPoint& pos );
    bool startDocking( CPoint& pos );
    void endDocking( CPoint& pos );
	void connect( CPoint& pos, const LinkInfo& linkInfo );
	void disconnect( const LinkInfo& linkInfo );
    void getDockPosition( CPoint& pos );

	CLASS_METHODS( ModulePort, CView )

	enum State {
		Idle	  = 0,
		Hover     = 1,
        Docking   = 2,
        Connected = 4
	};

	PortInfo* info_;
	ModuleBox* owner_;
	UINT16 state_;
    vector< LinkInfo > links_;
    PortAlignment align_;

protected:
	void patchToPort( CPoint& pos );
	void portToPatch( CPoint& pos );

    bool testHover( const CPoint& pos );
    bool setHover( const CPoint& pos );

	CRect rcText_, rcSquare_, rcHover_, rcIndicator_, rcInvalid_;
};



#endif // MODULEPORT_H