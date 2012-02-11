
#ifndef SCROLLVIEW_H
#define SCROLLVIEW_H

#include "Editor.h"


class Scrollbar : public CControl
{
public:
	enum ScrollbarDirection {
		kHorizontal,
		kVertical,
	};

	Scrollbar( const CRect& size, CControlListener* listener, long tag, ScrollbarDirection style, const CRect& scrollSize );
	~Scrollbar();
	
	void setScrollSize( const CRect& rcScrollSize );
	void setStep( float newStep ) { stepValue = newStep; }

	CRect& getScrollSize( CRect& rect ) const { rect = scrollSize; return rect; }
	float getStep() const { return stepValue; }

	// overwrite
	void draw( CDrawContext* pContext );
	bool onWheel( const CPoint &where, const CMouseWheelAxis &axis, const float &distance, const long &buttons );
	CMouseEventResult onMouseDown( CPoint &where, const long& buttons );
	CMouseEventResult onMouseUp( CPoint &where, const long& buttons );
	CMouseEventResult onMouseMoved( CPoint &where, const long& buttons) ;
	CMessageResult notify( CBaseObject* sender, const char* message );
	void setViewSize( CRect &newSize, bool invalid );

	CLASS_METHODS( Scrollbar, CControl )

  	CColor frameColor_;
	CColor scrollerColor_;
	CColor backgroundColor_;

protected:
	void drawBackground( CDrawContext* pContext );
	void drawScroller( CDrawContext* pContext, const CRect& size );

	void calculateScrollerLength();
	CRect getScrollerRect();
	void doStepping();

	ScrollbarDirection direction;
	CRect scrollSize;
	CRect scrollerArea;

	float stepValue;	
	CCoord scrollerLength;

private:
	CVSTGUITimer* timer;
	CPoint startPoint;
	CRect scrollerRect;
	bool scrolling;
	float startValue;
};






#endif // SCROLLVIEW_H