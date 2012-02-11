
#ifndef SYSTEMVIEW_H
#define SYSTEMVIEW_H

#include "Editor.h"
#include "Widgets.h"
#include "GridBox.h"

class AppOptions;
class SelectBox;


#ifdef _RT_AUDIO

class AudioOutPanel : public CViewContainer, public CControlListener
{
public:
    AudioOutPanel( const CRect& rcSize, Editor* editor );

    bool attached( CView* parent );
    void update();
    void setColors();
    void valueChanged( CControl* pControl );
    void onEngineRun( bool run );
    CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );
    void drawRect( CDrawContext* pContext, const CRect& updateRect );

protected:
    void setCursor( CursorType type );

    enum { CmdAsio, CmdRun, CmdDevice, CmdChannel, CmdBuffer, CmdRate };

    Editor* editor_;
    SelectBox *sbDevice_, *sbChannel_, *sbBuffer_, *sbRate_;
    Checkbox *cbAsio_, *cbRun_;
    Label *lbLatency_, *lbBuffer_, *lbRate_, *lblCaption_, *lblAsio_, *lblRun_;
};


class MidiBoxItem : public GridBoxItem
{
public:
    MidiBoxItem( const string& text, bool state );
    bool getState()     { return checkbox_->isChecked(); }
    string getText()    { return label_->text_; }  
    void setColors();

protected:
    Label* label_;
    Checkbox* checkbox_;
};


class MidiInPanel : public CViewContainer, public GridBoxListener
{
public:
    MidiInPanel( const CRect& rcSize, Editor* editor );

    void valueChanged( INT32 row, INT32 col ); 
    void setColors();
    void drawRect( CDrawContext* pContext, const CRect& updateRect );

protected:
    Editor* editor_;
    GridBox* gbPorts_;
    Label* lblCaption_;
};

#endif // _RT_AUDIO



class SystemView : public CViewContainer
{
public:
	SystemView( const CRect& size, Editor* editor );
    void setColors();
    void onEngineRun( bool run );

    CLASS_METHODS( SystemView, CViewContainer )
    
protected:
    Editor* editor_;

#ifdef _RT_AUDIO
    AudioOutPanel* audioOutPanel_;
    MidiInPanel* midiInPanel_;
#endif
};



#endif // SYSTEMVIEW_H