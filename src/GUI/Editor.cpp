

#include "Editor.h"
#include "../Synth.h"
#include "../SoundDevices.h"
#include "../Utils.h"
#include "Fonts.h"
#include "Images.h"
#include "Colors.h"

#include "ChildFrame.h"
#include "Widgets.h"
#include "TabView.h"
#include "ProgramView.h"
#include "CtrlView.h"
#include "BankView.h"
#include "SystemView.h"

const CColor colTransparent	         = { 255, 255, 255,   0 };
const CColor colBlack		         = {   0,   0,   0, 255 };
const CColor colWhite		         = { 255, 255, 255, 255 };
const CColor colGrey		         = {  41,  57,  67, 255 };
const CColor colOrange	             = { 209, 128,  46, 255 };
const CColor colRed		             = { 183,  46,   0, 255 };
const CColor colGreen	             = {  42, 148,  50, 255 };
const CColor colYellow               = { 221, 190,   4, 255 };


//-----------------------------------------------
// class Editor
//-----------------------------------------------

Editor::Editor() :
	childFrame_( NULL ),
	programView_( NULL ),
	ctrlView_( NULL ),
    bankView_( NULL ),
    systemView_( NULL ),
    tabView_( NULL ),
    appState_( Stopped )
{
	
    Fonts::loadFonts();
}


Editor::~Editor()
{
    Fonts::cleanup();
}


bool Editor::open( void* systemWindow )
{
    createFrame( systemWindow );
    initGui( systemWindow );

    return true;
}


void Editor::initGui( void* systemWindow )
{
	CRect rc      = childFrame_->getViewSize();
    CCoord middle = rc.width() - 276;
    CCoord bottom = rc.height() - 55;

    CRect rcTabView( rc.left+8, rc.top+8, rc.right-8, rc.bottom-8 );
	CRect rcTabButtons( -3, 0, 100, 64 );	            // size of a TabButton (double height)
	CRect rcDisplay( 0, 0, rc.right-16, bottom );	    // area without tab bar
	CRect rcProgramView( 0, 0, middle, bottom );
	CRect rcCtrlView( middle, 0, rc.right-8, bottom );
    CRect rcBankView( 0, 0, rc.right-8, bottom );
    CRect rcSystemView( 0, 0, rc.right-16, bottom );

	tabView_     = new TabView( rcTabView, rcTabButtons, this );
   	ctrlView_    = new CtrlView( rcCtrlView, this );
	programView_ = new ProgramView( rcProgramView, this, ctrlView_ );
    bankView_    = new BankView( rcBankView, this );
    
	CViewContainer* programTab = new CViewContainer( rcDisplay, childFrame_ );
    programTab->setAutosizeFlags( kAutosizeAll );
    programTab->setTransparency( true );
    programTab->addView( programView_->getScrollView() );
	programTab->addView( ctrlView_ );

    tabView_->addTab( programTab, new TabButton( "Program" ));
	tabView_->addTab( bankView_, new TabButton( "Bank" ) );
	
    if( isStandalone() )
    {
        systemView_ = new SystemView( rcSystemView, this );
        tabView_->addTab( systemView_, new TabButton( "System" ) );
    }

    tabView_->init();	
    childFrame_->addView( tabView_ );
    checkAppState();
    setColorScheme();
}


void Editor::close()
{
}


void Editor::closeFrame()
{
    if( childFrame_ ) 
    { 
		if( childFrame_->close() )
        {
            childFrame_->forget();
		    childFrame_  = NULL;
        	programView_ = NULL;
	        ctrlView_    = NULL;
            bankView_    = NULL;
            systemView_  = NULL;
            tabView_     = NULL;
        }
	}
}


void Editor::setSize( const CRect& rcSize )
{
    if( childFrame_ ) 
    {
        CCoord middle = rcSize.width() - 276;
        CCoord bottom = rcSize.height() - 55;

	    CRect rcLeft( 0, 0, middle, bottom );
	    CRect rcRight( middle, 0, rcSize.right-8, bottom );
        
        programView_->setPreferredSize( rcLeft );
        ctrlView_->setPreferredSize( rcRight );
    }
}


void Editor::setColorScheme()
{
    for( UINT16 i=0; i<gColorSchemes.size(); i++ )
    {
        ColorScheme& scheme = gColorSchemes[i];
        if( scheme.name_ == options_.colorScheme_ )
        {
            colors = scheme;
            childFrame_->setBackgroundColor( colors.fill2 );
            tabView_->setColors();
            programView_->setColors();
            bankView_->setColors();
            if( systemView_ ) systemView_->setColors();
            childFrame_->invalid();
        }
    }
}


Bank* Editor::getBank()                      
{ 
    return synth_->getBank(); 
}


INT32 Editor::loadBank( const string& path )
{
    Bank* bank = synth_->getBank();
    bank->close( options_.autosave_ );

    string p = path;
    p.empty() ? p = options_.bankPath_ : options_.bankPath_ = p;

    p.empty() ? bank->newBank( options_.appPath_ + "New Bank.nexus", "New Bank", false ) : bank->load( p );
    programMessage( BankChanged, NULL );
    return bank->programNum_;
}


INT32 Editor::newBank( const string& path, const string& name, bool autosave )
{
    bool save = autosave ? options_.autosave_ : false;
    
    Bank* bank = synth_->getBank();
    bank->newBank( path, name, save );
    programMessage( BankChanged, NULL );
    return bank->programNum_;
}


void Editor::saveBank( const string& path )
{
    Bank* bank = synth_->getBank();
    bank->save( path, true );
    options_.bankPath_ = bank->path_;
}


void Editor::closeBank()
{
    getBank()->close( options_.autosave_ );
}


Program* Editor::getCurrentProgram()
{
    return getBank()->getCurrentProgram();
}


Program* Editor::setProgram( INT32 programNum )
{
    return getBank()->setProgram( programNum, options_.autosave_ );
}


INT32 Editor::getProgramNumber()             
{ 
    return getBank()->programNum_; 
}


bool Editor::programMessage( ProgramMessage msg, CView* sender )
{
    if( childFrame_ ) try
    {
        switch( msg ) 
        {
        case BankChanged:
        case BankEdited:
            tabView_->programMessage( msg, sender );
            bankView_->programMessage( msg, sender );
            break;
        case ProgramChanged: 
            programView_->programMessage( msg, sender );
            bankView_->programMessage( msg, sender );
            tabView_->programMessage( msg, sender );
            break;
        case ProgramEdited: 
            programView_->programMessage( msg, sender );
            break;
        }
	}
	catch( const exception& e ) {
		TRACE( e.what() );
		return false;
	}
	return true;

}


void Editor::copyText( ostringstream& os )   
{ 
    Clipboard::copyText( (HWND)childFrame_->getSystemWindow(), os ); 
}


bool Editor::pasteText( istringstream& is )  
{ 
    return Clipboard::pasteText((HWND)childFrame_->getSystemWindow(), is ); 
}


bool Editor::isTextAvailable()               
{ 
    return Clipboard::isTextAvailable(); 
}     


bool Editor::onDrop( CDragContainer* drag, const CPoint& pos )
{
    if( bankView_->isAttached() ) {
        return bankView_->onDrop( drag, pos );
    }
    return false;
}


void Editor::setAppState( AppState state )
{
    appState_ = state;

    if( appState_ != Running ) 
        setMonitor( 0, -1, -1, MonitorAll );

    if( tabView_ ) 
        tabView_->setAppState( appState_ );

    if( childFrame_ )
        childFrame_->updateWindow();
}


void Editor::checkAppState()
{
    if( devices_.getState() == SoundDevices::NotReady ) {
        setAppState( Stopped );
        return;
    }
    if( synth_->isReady() == false ) {
        setAppState( NotReady );
        return;
    }
    if( synth_->isRunning() == false ) {
        setAppState( Suspended );
        return;
    }
    setAppState( Running );
}


void Editor::setMonitor( INT16 numSounding, FLOAT pitch, FLOAT gate, UINT16 flags )
{
    if( tabView_ ) 
        tabView_->setMonitor( numSounding, pitch, gate, flags );
}


long Editor::onKeyDown( VstKeyCode& keyCode )
{
    if( keyCode.virt == 0 && keyCode.modifier == MODIFIER_CONTROL )
    {
        switch( keyCode.character ) {
#ifdef _RT_AUDIO
        case 101:            // Ctrl-e
            if( systemView_ )
                systemView_->onEngineRun( !options_.audioRun_ );
            return 1;
#endif
        case 111:            // Ctrl-o
            if( bankView_ ) 
                bankView_->onOpenBank(); 
            return 1;
        case 115:            // Ctrl-s
            if( bankView_ ) 
                bankView_->onSaveBank(); 
            return 1;   
        }
    }
    return 0;
}


#ifdef _RT_AUDIO

//------------------------------------------------------
// class RtEditor
//------------------------------------------------------

void* hInstance = NULL;

#include "direct.h"

RtEditor::RtEditor( HINSTANCE hInst ) : 
    Editor()
{
    hInstance = hInst;

    char buffer[_MAX_PATH];
    getcwd( buffer, MAX_PATH );
    string appPath = buffer + string( "\\" );

    options_.open( __argc, __targv, appPath, "nexus_standalone.options" );
    devices_.setOptions( &options_ );
    colors.init( appPath, colors );
}


bool RtEditor::open()
{
    if( mainFrame_.create( (HINSTANCE)hInstance, &options_ )) 
    {
        if( options_.audioRun_ )
            devices_.open();

        Editor::open( mainFrame_.hwnd_ );
        mainFrame_.show( childFrame_ );
    }
    close();
    return true;
}


void RtEditor::close()
{
    if( childFrame_ )
    {
        devices_.close();
        synth_->close(); 

        bankView_->getColumnMetrics( options_.columnMetrics_ );
        bankView_->getSortInfo( options_.sortColumn_, options_.sortType_ );
        options_.close();

        closeFrame();
    }
}


void RtEditor::createFrame( void* systemWindow )
{
    CRect rcSize;
    mainFrame_.getClientRect( rcSize );
    setSize( rcSize );
    
    childFrame_ = new ChildFrame( rcSize, systemWindow, this );
    childFrame_->setBackgroundColor( colors.fill2 );
    childFrame_->useOffscreen( true );
    this->frame = childFrame_;
    childFrame_->editor_ = this;
}


void RtEditor::setColorScheme()
{
    Editor::setColorScheme();
    mainFrame_.onNcPaint();
}

#else // _RT_AUDIO

extern void* hInstance;

//------------------------------------------------------
// class VstEditor
//------------------------------------------------------


VstEditor::VstEditor( void* effect ) :
    Editor(),
	AEffGUIEditor( effect )
{
    LPTSTR lpDllPath = new TCHAR[ _MAX_PATH ];
    GetModuleFileName( (HMODULE)hInstance, lpDllPath, _MAX_PATH );  // get the file path of the dll

    string dllPath = Utils::tchar2string( lpDllPath );
    dllPath = dllPath.substr( 0, dllPath.find_last_of( '\\' )+1 );
    
    options_.open( dllPath, "nexus_plugin.options" );
    colors.init( dllPath, colors );

    CRect rcSize( options_.windowLeft_, options_.windowTop_, options_.windowRight_, options_.windowBottom_ );
    rcSize.offset( -rcSize.left, -rcSize.top );
    setSize( rcSize );

    delete[] lpDllPath;
}


VstEditor::~VstEditor()
{
    options_.close();
}


bool VstEditor::open( void* systemWindow )
{
    AEffGUIEditor::open( systemWindow );
    return Editor::open( systemWindow );
}


void VstEditor::close()
{
    AEffGUIEditor::close();

   	bankView_->getColumnMetrics( options_.columnMetrics_ );
    bankView_->getSortInfo( options_.sortColumn_, options_.sortType_ );

    closeFrame();
}


void VstEditor::createFrame( void* systemWindow )
{
    childFrame_ = new ChildFrame( CRect( rect.left, rect.top, rect.right, rect.bottom ), systemWindow, this );
	childFrame_->setBackgroundColor( colors.fill2 ); 
    childFrame_->useOffscreen( true );
	this->frame = childFrame_;
    childFrame_->editor_ = this;
}


void VstEditor::doIdleStuff()
{
    if( tabView_ ) {
        tabView_->updateMonitor();
    }
    AEffGUIEditor::doIdleStuff();
}


void VstEditor::setSize( const CRect& rcSize )
{
	rect.top    = (int)rcSize.top;
	rect.left   = (int)rcSize.left;
	rect.right  = (int)rcSize.right;
	rect.bottom = (int)rcSize.bottom;	

    Editor::setSize( rcSize );
}

#endif // _RT_AUDIO


//-------------------------------------------------
// DebugHelpers
//-------------------------------------------------
//
void TraceRect( const CRect& r, const string s )
{
    TRACE( "%s : left=%d top=%d right=%d bottom=%d width=%d height=%d\n", s.c_str(), r.left, r.top, r.right, r.bottom, r.getWidth(), r.getHeight() );
}


void TracePoint( const CPoint& p, const string s )
{
    TRACE( "%s : x=%d y=%d\n", s.c_str(), p.x, p.y );
}


