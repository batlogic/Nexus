
#ifndef WIDGETS_H
#define WIDGETS_H

#include "Editor.h"
#include "../VST/cscrollview.h"



//--------------------------------------------------
// class ScrollView
//--------------------------------------------------

class ScrollView : public CScrollView
{
public:
    ScrollView( const CRect& size, const CRect& containerSize, CFrame* frame, long style );

    void enableScrollbars( bool horizontal, bool vertical );
    void setScrollOffset( CPoint& offset );
    CRect& getContainerSize()   { return containerSize; }

    CLASS_METHODS( ScrollView, CScrollView )
};



//--------------------------------------------------
// class CMenuItemEx
//--------------------------------------------------

class CMenuItemEx : public CMenuItem
{
public:
	CMenuItemEx( const char* title, UINT16 tag, long flags=kNoFlags ) :
	  CMenuItem( title, flags ),
	  tag_( tag )
	  {}

	UINT32 tag_;
};



//--------------------------------------------------
// class ModalDialog
//--------------------------------------------------

class ModalDialog : public CViewContainer
{
public:

    ModalDialog( CView* owner );

    virtual void show( CPoint& pos, bool centerPosition );
    virtual void close( UINT16 cmd );
    virtual void valueChanged( CControl *pControl );
    void looseFocus() {}
    void center();

    long onKeyDown( WPARAM wParam );
    CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseUp( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseMoved( CPoint& pos, const long& buttons );

    CLASS_METHODS( ModalDialog, CViewContainer )

    enum State { Running, Committed, Canceled };
    State state_;

protected:
    void moveTo( const CPoint& pos );
    void advanceFocus( bool forward );
    CView* getNextFocusView( CView* view, bool forward );
    CCView* getCCView( CView* view );
    CControl* getControlByTag( UINT16 tag );

    CView* owner_;
    CView* mouseOverView_;
    CRect rcTop_, rcBottom_, rcClient_;
    
    enum Command { CmdNothing, CmdCommit, CmdCancel };

    struct TrackData
	{
		TrackData() : tracking_( false ) {}
		bool tracking_;
		CPoint offset_;
	};
	TrackData trackData_;
};



//--------------------------------------------------
// class Label
//--------------------------------------------------

class Label : public CControl
{
public:
	Label( const CRect& rcSize, const string& text, CControlListener* listener=NULL, long tag=0 );

	CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );
    void draw( CDrawContext *pContext );
    void setText( const string& text );
    void setColors( const CColor& colText, const CColor& colTextSel, const CColor& colTextFocus, const CColor& colBkgnd, const CColor& colFrame );
    bool isSelected();
    bool isFocused();

	CLASS_METHODS( Label, CControl )

    enum MouseBehavior { OnMouseNothing=0, OnMouseLeftDown=1 };

	string text_;
	CColor colText_, colTextSel_, colTextFocus_, colBkgnd_, colFrame_;
	CHoriTxtAlign hAlign_;
    CPoint indent_;
	CFontDesc* font_;
    void* data_;
    MouseBehavior onMouse_;
};



//--------------------------------------------------
// class Checkbox
//--------------------------------------------------

class Checkbox : public CControl
{
public:
	Checkbox( CRect& rcSize, CControlListener* listener, long tag, float state );
    CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );
	void draw( CDrawContext *pContext );
    bool isChecked()    { return value == 0.0f ? false : true; }
    void setState( bool state );
    void setValue( float value );

	CLASS_METHODS( Checkbox, CControl )

    CColor colTick_, colFrame_, colBkgnd_;
    CPoint tickSize_;
    void* data_;
};



//--------------------------------------------------
// class Slider
//--------------------------------------------------

class Slider : public CControl
{
public:
    enum Style { Monopolar, Bipolar };

	Slider( const CRect& rcSize, CControlListener* listener, long tag );

    //float getValue() const { return value_; }
    float getValue() const;
    void setValue( float value );
    void setResolution( float resolution );
    void takeFocus();
    void looseFocus();
    
    CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );
	CMouseEventResult onMouseUp( CPoint& pos, const long& buttons );
	CMouseEventResult onMouseMoved( CPoint& pos, const long& buttons );
    long Slider::onKeyDown( VstKeyCode& keyCode );
	
	void draw( CDrawContext *pContext );

	CColor colHandle_;
	CColor colBkgnd_;
    UINT16 style_;
    void* data_;
    float defaultValue_, minValue_, maxValue_;
    UINT16 numSteps_;
    float resolution_;
    float finetuning_;

    CLASS_METHODS( Slider, CControl )

protected:
    enum Orientation { Horizontal, Vertical };

    float offset2Value( const CPoint& offset ) const;
    void value2Pos( const float value, CPoint& pos ) const;
    void setHandle( float value );
    float rasterize( float value ); 
    void notifyListener();

    float value_, coef_;
    Orientation orientation_;
    CPoint pos_;
    CRect rcHandle_, rcFocus_;
    bool focus_;

	bool tracking_;
    float valueAnchor_;
    CPoint posAnchor_;
};


//--------------------------------------------------
// class KickButton
//--------------------------------------------------

class KickButton : public CKickButton
{
public:
    KickButton( const CRect& rcSize, const string& label, CControlListener* listener, long tag );
    void draw( CDrawContext *pContext );
    void setColors( const CColor& colBkgnd, const CColor& colText, const CColor& colFrame, const CColor& colFrameKicked );

    string label_;
    CColor colBkgnd_, colFrame_, colFrameKicked_, colText_;
    CFontRef font_;
};


//-------------------------------------------
// class Caret
//-------------------------------------------

class Caret
{
public:
    Caret();

    void show( CView* parent, CCoord height );
    void hide();
    void setPosition( const CPoint& pos );

    CPoint position_;
    CCoord height_;

protected:
    CView* parent_;
    HWND hwnd_;
};


//--------------------------------------------------
// class Rubberband
//--------------------------------------------------

class Rubberband : public CView
{
public:
    Rubberband();

    void draw( CDrawContext* pContext );
    void begin( const CPoint& pos, const CPoint& offset );
    void stretch( const CPoint& pos );
    void hide();

    CColor colLine_;
    CLineStyle lineStyle_;

protected:
    CPoint anchor_;
    CPoint offset_; // scroll offset
};


//--------------------------------------------------
// class DrawHelper
//--------------------------------------------------

class DrawHelper
{
public:
    static void drawTriangle( CDrawContext* pContext, const CRect& rcSize, CColor color, bool upwards );
};



#endif // WIDGETS_H