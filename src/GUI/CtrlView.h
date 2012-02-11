
#ifndef CTRLVIEW_H
#define CTRLVIEW_H

#include "Editor.h"
#include "Widgets.h"
#include "../ModuleInfo.h"
#include "../ModuleData.h"
#include "../Polyphony.h"

class ModuleData;
class CtrlInfo;
class NumericEdit;
class CtrlPanel;



//--------------------------------------------------
// class CtrlView
//--------------------------------------------------

class CtrlView : public CViewContainer, public CControlListener
{
public:
	CtrlView( const CRect& rcSize, Editor* editor );

    bool removed( CView* parent );
    CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );
    void valueChanged( CControl* pControl );
    void setPreferredSize( const CRect& rcSize );

    void updateModule( ModuleData* data );
    void updateProgram();

    CLASS_METHODS( CtrlView, CViewContainer )

    enum Command { 
        CmdProgramName, CmdProgramCategory, CmdProgramComment, 
        CmdProgramHold, CmdProgramRetrigger, CmdProgramLegato,
        CmdProgramNumVoices, CmdProgramNumUnison, CmdProgramSpread,
        CmdModuleLabel, CmdPanic };

protected:
    bool removeAll (const bool &withForget = true);

	Editor* editor_;
    CtrlPanel* ctrlPanel_;
};



//-----------------------------------------------------------------------------
// class ScrollbarDrawer
//-----------------------------------------------------------------------------

class ScrollbarDrawer : public IScrollbarDrawer
{
public:
	void drawScrollbarBackground( CDrawContext* pContext, const CRect& size, CScrollbar::ScrollbarDirection direction, CScrollbar* bar);
	void drawScrollbarScroller( CDrawContext* pContext, const CRect& size, CScrollbar::ScrollbarDirection direction, CScrollbar* bar);
};



//--------------------------------------------------
// class CtrlPanel
//--------------------------------------------------

class CtrlPanel : public CViewContainer
{
public:
    CtrlPanel( const CRect& rcSize, Editor* editor, CtrlView* parent, ModuleData* data );
    ScrollView* getScrollView() { return scrollView_; }
    void setPreferredSize( const CRect& rcSize );
    CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );
    //void setViewSize( CRect& rect, bool invalid );

    CLASS_METHODS( CtrlPanel, CViewContainer )

protected:
    void rescale();

    Editor* editor_;
    CtrlView* parent_;
    ScrollView* scrollView_;
    ScrollbarDrawer drawer_;
    CCoord clientHeight_;
    CRect rcPreferred_;
};


//--------------------------------------------------
// class ParamStrip
//--------------------------------------------------

class ParamStrip : public CViewContainer, public CControlListener, public MidiListener
{
public:
	ParamStrip( const CRect& rcSize, Editor* editor, ModuleData* data, ParamData* paramData );

    CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );
    void valueChanged( CControl* pControl );
    void attachOrDetach( ParamData* paramData );
    void onController( UINT16 eventNum, FLOAT value );

    CLASS_METHODS( ParamStrip, CViewContainer )

protected:
    void addControl( const CRect& rc, UINT16 id, ParamData* paramData );
    CMouseEventResult showCtrlDialog( const CPoint& pos );
    
    enum IdOffset { ParamOffset=0, LinkOffset=1000 };

    Editor* editor_;
    ModuleData* data_;
};



//--------------------------------------------------------------
// class CtrlDialog
//--------------------------------------------------------------

class CtrlDialog : public ModalDialog, public CControlListener
{
public:
    CtrlDialog( CView* owner, ParamStrip* paramStrip, ParamData* paramData );

    void valueChanged( CControl *pControl );
    void drawBackgroundRect( CDrawContext* pContext, CRect& rcUpdate );

    CLASS_METHODS( CtrlDialog, ModalDialog )

    ParamData paramData_;

protected:
    enum Command { 
        CmdLabel      = 3,
        CmdVeloSens   = 4,
        CmdDefault    = 5,
        CmdKeyTrack   = 6,
        CmdBipolar    = 7,
        CmdParamMin   = 8, 
        CmdParamMax   = 9,
        CmdParamSteps = 10,
        CmdParamLog   = 11,
        CmdParamRes   = 12,
        CmdParamUnit  = 13,
        CmdCC         = 14,
        CmdCCNum      = 15,
        CmdCCMin      = 16,
        CmdCCMax      = 17,
        CmdCCSteps    = 18,
        CmdCCSoft     = 19
    };

    Label* lblCaption_;
    CRect rcBtnOk_, rcBtnCancel_;
    ParamStrip* paramStrip_;
};



//-------------------------------------------------------
// class TextSlider
//-------------------------------------------------------

class TextSlider : public CControl, public CControlListener
{
public:
    TextSlider( const CRect& rcSize, CControlListener* listener, long tag, ParamData* paramData );
    ~TextSlider();
        
    void init();
    bool attached( CView* parent );
    bool removed( CView* parent );
    void looseFocus();
    void parentSizeChanged();
    bool verifyView( CView* view ) const;
    void valueChanged( CControl* pControl );
    float getValue() const;
    void setValue( FLOAT value );
    void draw( CDrawContext* pContext );

    CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseMoved( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseUp( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseEntered( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseExited( CPoint& pos, const long& buttons );
    long onKeyDown( VstKeyCode& keyCode );

    CLASS_METHODS( TextSlider, CControl )

    ParamData* paramData_;

protected:
    CView* getViewAt( const CPoint& pos ) const;

    Slider* slider_;
    NumericEdit* numEdit_;
};



#endif // CTRLVIEW_H