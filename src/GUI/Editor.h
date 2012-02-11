
#ifndef EDITOR_H
#define EDITOR_H

#include "../Definitions.h"
#include "../AppOptions.h"
#include "../Bank.h"
#include "../VST/aeffguieditor.h"
#include "../VST/ctabview.h"
#include "ChildFrame.h"
#include "Clipboard.h"
#include "Colors.h"


class Synth;
class Program;
class TabView;
class ProgramView;
class CtrlView;
class BankView;
class SystemView;
class ChildFrame;
class CScrollViewEx;
class SoundDevices;

extern const CColor colTransparent;
extern const CColor colBlack;
extern const CColor colWhite;
extern const CColor colGrey;
extern const CColor colOrange;
extern const CColor colRed;
extern const CColor colGreen;
extern const CColor colYellow;


extern void TraceRect( const CRect& r, const string s );
extern void TracePoint( const CPoint& p, const string s );

#define ATTR_SELECTED 10000
#define ATTR_FOCUSED 10001



class Editor 
{
friend class ChildFrame;
public:
    Editor();
	virtual ~Editor();

    bool open( void* systemWindow );
    virtual void close();
    
    INT32 loadBank( const string& path="" );
    INT32 newBank( const string& path , const string& name, bool autosave );
    void saveBank( const string& path="" );
    void closeBank();
    Program* getCurrentProgram();
    Program* setProgram( INT32 programNum );
    INT32 getProgramNumber();

    enum ProgramMessage { BankChanged, BankEdited, ProgramChanged, ProgramEdited };
    bool programMessage( ProgramMessage msg, CView* sender ); 

	ChildFrame* getChildFrame()	         { return childFrame_; }
    virtual void* getMainFrameHandle()   { return NULL; }
    AppOptions* getOptions()             { return &options_; }
	void setSynth( Synth* synth )	     { synth_ = synth; }
	Synth* getSynth()				     { return synth_; }
    SoundDevices* getDevices()           { return &devices_; }
    Bank* getBank();
    ProgramView* getProgramView()        { return programView_; }
    BankView* getBankView()              { return bankView_; }
    CtrlView* getCtrlView()              { return ctrlView_; }

    void copyText( ostringstream& os );
    bool pasteText( istringstream& is );
    bool isTextAvailable();
    bool onDrop( CDragContainer* drag, const CPoint& pos );

    enum AppState { Running, Suspended, Stopped, NotReady };

    void setAppState( AppState state );
    AppState getAppState()  { return appState_; }
    void checkAppState();
    void setMonitor( INT16 numSounding, FLOAT pitch, FLOAT gate, UINT16 flags );

    long onKeyDown( VstKeyCode& keyCode );
    bool isStandalone() { return devices_.isStandalone(); }

    virtual void setColorScheme();
    
protected:
    virtual void createFrame( void* systemWindow ) = 0;
    void closeFrame();
    virtual void setSize( const CRect& size );
    void initGui( void* systemWindow );

    AppOptions options_;
    SoundDevices devices_;
    ChildFrame* childFrame_;
    TabView* tabView_;
	Synth* synth_;
	ProgramView* programView_;
	CtrlView* ctrlView_;
    BankView* bankView_;
    SystemView* systemView_;
    AppState appState_;
};


#ifdef _RT_AUDIO

#include "MainFrame.h"


class RtEditor : public Editor, public VSTGUIEditorInterface
{
public:
    RtEditor( HINSTANCE hInst );
    bool open();
    void close();
    
    void* getMainFrameHandle() { return mainFrame_.hwnd_; }
    void setColorScheme();
    
protected:
    void createFrame( void* systemWindow );

    MainFrame mainFrame_;
};
#else

class VstEditor : public Editor, public AEffGUIEditor
{
public:
    VstEditor( void* effect );
    ~VstEditor();

    bool open( void* systemWindow );
    void close();
    void doIdleStuff();

protected:
    void createFrame( void* systemWindow );
    void setSize( const CRect& rcSize );
};

#endif _RT_AUDIO



#endif // EDITOR_H