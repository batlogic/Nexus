
#ifndef TABVIEW_H
#define TABVIEW_H

#include "Editor.h"
#include "Widgets.h"
#include "../VST/ctabview.h"

class SelectBox;
class Label;


//--------------------------------------------------
// class TabButton
//--------------------------------------------------

class TabButton : public COnOffButton
{
public:
	TabButton( const char* label );
	void draw( CDrawContext* pContext );

    CLASS_METHODS( TabButton, COnOffButton )

protected:
	string label_;
};



//--------------------------------------------------
// class TabView
//--------------------------------------------------

class TabView : public CTabView
{
public:
    TabView( const CRect& rcSize, const CRect& rcTabSize, Editor* editor );
    ~TabView();

    void init();
    void setColors();
    bool programMessage( Editor::ProgramMessage msg, CView* sender );
    void setAppState( Editor::AppState state );
    void setMonitor( INT16 numSounding, FLOAT pitch, FLOAT gate, UINT16 flags );
    void updateMonitor();

    bool addTab( CView* view, CControl* button );
    bool selectTab( long index );
    bool onDrop( CDragContainer* drag, const CPoint& pos );

    CLASS_METHODS( TabView, CTabView )

protected:
    void updateProgramBox();

    Editor* editor_;
    SelectBox* sbProgram_;
    Label *lblPitch_, *lblGate_, *lblVoices_, *lblState_;
};


#endif // TABVIEW_H
