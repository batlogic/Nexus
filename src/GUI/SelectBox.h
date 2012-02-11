
#ifndef SELECTBOX_H
#define SELECTBOX_H

#include "Editor.h"
#include "../VST/cscrollview.h"


typedef vector < string > SelectBoxList;


class SelectBox : public CControl, public CControlListener
{
public:
    SelectBox( const CRect& rcSize, CControlListener* listener, long tag );
    ~SelectBox();

    INT32 getIndex()    { return index_; }
    void getText( INT32 index, string& text );
    void setIndex( string s );
    void setIndex( INT32 idx );
    void takeFocus();
    void looseFocus();

	void valueChanged( CControl* pControl );
    CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseUp( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseMoved( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseEntered( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseExited( CPoint& pos, const long& buttons );
    long onKeyDown( VstKeyCode& keyCode );
    bool onWheel( const CPoint& pos, const CMouseWheelAxis& axis, const float& distance, const long& buttons );

    void draw( CDrawContext* pContext );

    enum DropDirection { Dropdown, Dropup };

    void setDropDirection( DropDirection dir )  { dropDir_ = dir; }
    void setList( const SelectBoxList& list, INT32 numVisible );
    void setViewSize( CRect& rcSize, bool invalid );
    void setFont( CFontRef font );
    void setTextColor( const CColor& col );
    void setSelectionColor( const CColor& col );
    void setBackgroundColor( const CColor& col );
    void setFrameColor( const CColor& col );
        
	CLASS_METHODS( SelectBox, CControl )

	SelectBoxList list_;

protected:
    enum State { Hover = 1, Dropped = 2, Focused = 4 };
    enum Metrics { WidthButton = 13, TextHeight = 15 };

    void calcMetrics();
    INT32 indexFromPosition( const CPoint& pos );
    INT32 scrollTo( INT32 index );
    void drawDropdown( CDrawContext* pContext );
    void setState( UINT16 state );
    void commit( bool commit );
    void getTextRect( CRect& rc, INT32 index );

    CScrollbar* scrollbar_;

    UINT16 state_;
    DropDirection dropDir_;
    CRect rcBox_, rcText_, rcArrow_, rcArrowSel_, rcClip_, rcDropdown_, rcScrollbar_, rcScrollsize_;
    INT32 index_, indexSel_, indexSaved_;
    INT32 maxVisible_;
    INT32 scrollOffset_;
    CColor colText_, colBkgnd_, colFrame_, colSel_;
    CFontRef font_;
};



#endif // SELECTBOX_H