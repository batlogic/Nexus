
#ifndef CHILDFRAME_H
#define CHILDFRAME_H


#include "Editor.h"

class Editor;


enum CursorType
{
	CursorDefault = 0,			// arrow cursor
	CursorWait,					// wait cursor
	CursorHSize,				// horizontal size cursor
	CursorVSize,				// vertical size cursor
	CursorSizeAll,				// size all cursor
	CursorNESWSize,				// northeast and southwest size cursor
	CursorNWSESize,				// northwest and southeast size cursor
	CursorCopy,					// copy cursor (mainly for drag&drop operations)
	CursorNotAllowed,			// not allowed cursor (mainly for drag&drop operations)
	CursorHand,					// hand cursor
    CursorBeam                  // text edit cursor
};


class ChildFrame : public CFrame   
{
public:
	ChildFrame( const CRect& rcSize, void* systemWindow, VSTGUIEditorInterface* editor );
    bool close();

	void resetDanglingPointers();
    long setCursor();
    void setCursor( CursorType type );

    void drawRect( CDrawContext* pContext, const CRect& updateRect );
    void redraw();
    void updateWindow();
    bool setSize( CCoord width, CCoord height );

    long onKeyDown( VstKeyCode& keyCode );
    bool onWheel( const CPoint& pos, const CMouseWheelAxis& axis, const float& distance, const long& buttons );
    CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseMoved( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseUp( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseExited( CPoint& pos, const long& buttons );

    void setMouseCaptureView (CView* view ) { mouseCaptureView_ = view; }
    static void makeVstKeyCode( UINT nChar, VstKeyCode& keyCode );

    Editor* editor_;

protected:
    CView* mouseCaptureView_;
    HCURSOR hCursor_;
};





#endif // CHILDFRAME_H