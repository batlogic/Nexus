
#include "TabView.h"
#include "Fonts.h"
#include "SelectBox.h"
#include "Widgets.h"
#include "BankView.h"



//--------------------------------------------------------------
// class TabButton
//--------------------------------------------------------------

TabButton::TabButton( const char* label )
: COnOffButton( CRect( 0, 0, 0, 0 ), NULL, 0, NULL, kPostListenerUpdate ),
	label_( label )
{
	setTransparency( false );
	setDirty( true );
    setAutosizeFlags( kAutosizeBottom );
}


void TabButton::draw( CDrawContext* pContext )
{
	CRect r = size;
	r.inset( 3, 4 );

	pContext->setFillColor( colors.control1 );
	value == 1.f ? pContext->setFrameColor( colors.selection ) : pContext->setFrameColor( colTransparent );
	pContext->setLineWidth( 1 );
	pContext->drawRect( r, kDrawFilledAndStroked );

	pContext->setFont( fontPirulen9, 9 );
	pContext->setFontColor( colors.control2 );
	pContext->drawStringUTF8( label_.c_str(), r, kCenterText, true );

    setDirty( false );
}



//--------------------------------------------------------------
// class TabView
//--------------------------------------------------------------

TabView::TabView( const CRect& rcSize, const CRect& rcTabSize, Editor* editor ) :
    CTabView( rcSize, editor->getChildFrame(), rcTabSize, NULL, kPositionBottom ),
    editor_( editor ),
    sbProgram_( NULL ),
    lblPitch_( NULL ),
    lblGate_( NULL ), 
    lblVoices_( NULL ),
    lblState_( NULL )
{
    setTransparency( true );
    setAutosizeFlags( kAutosizeAll );
}


TabView::~TabView()
{
    if( isChild( sbProgram_ ) == false )
        delete sbProgram_;
}


void TabView::init()
{
    CRect rcBottom( size.left, size.bottom-39, size.right, size.bottom );
    
    CRect rcProgram( rcBottom.right-250, rcBottom.top+3, rcBottom.right-118, rcBottom.top+21 );
    CRect rcVoices( rcBottom.right-115, rcBottom.top+3, rcBottom.right-87, rcBottom.top+21 );
    CRect rcPitch( rcBottom.right-86, rcBottom.top+3, rcBottom.right-57, rcBottom.top+21 );
    CRect rcGate( rcBottom.right-56, rcBottom.top+3, rcBottom.right-27, rcBottom.top+21 );
    CRect rcState( rcBottom.right-26, rcBottom.top+3, rcBottom.right-8, rcBottom.top+21 );

    sbProgram_ = new SelectBox( rcProgram, editor_->getBankView(), BankView::CmdLoadProgram );
    sbProgram_->setFont( fontArial9 );
    sbProgram_->setDropDirection( SelectBox::Dropup );
    sbProgram_->setAutosizeFlags( kAutosizeRight|kAutosizeBottom );
    addView( sbProgram_ );

    lblVoices_            = new Label( rcVoices, "0" );
    lblVoices_->font_     = fontArial9;
    lblVoices_->hAlign_   = kCenterText;
    lblVoices_->setAutosizeFlags( kAutosizeRight|kAutosizeBottom );
    addView( lblVoices_ );

    lblPitch_            = new Label( rcPitch, "" );
    lblPitch_->font_     = fontArial9;
    lblPitch_->hAlign_   = kCenterText;
    lblPitch_->indent_   = CPoint( 1, 0 );
    lblPitch_->setAutosizeFlags( kAutosizeRight|kAutosizeBottom );
    addView( lblPitch_ );

    lblGate_            = new Label( rcGate, "" );
    lblGate_->font_     = fontArial9;
    lblGate_->hAlign_   = kCenterText;
    lblGate_->setAutosizeFlags( kAutosizeRight|kAutosizeBottom );
    addView( lblGate_ );

    lblState_            = new Label( rcState, "" );
    lblState_->colBkgnd_ = colGrey;
    lblState_->colText_  = colTransparent;
    lblState_->setAutosizeFlags( kAutosizeRight|kAutosizeBottom );
    addView( lblState_ );

    updateProgramBox();
    setColors();
	alignTabs( CTabView::kAlignLeft );
    selectTab( 0 );
}


void TabView::setColors()
{
    sbProgram_->setTextColor( colors.text3 );
    sbProgram_->setBackgroundColor( colors.fill2 );
    sbProgram_->setFrameColor( colors.fill3 );
    sbProgram_->setSelectionColor( colors.selection );

    lblVoices_->colBkgnd_ = colors.text3;
    lblVoices_->colText_  = colors.fill2;
    lblVoices_->colFrame_ = colTransparent;

    lblPitch_->colBkgnd_ = colors.text3;
    lblPitch_->colText_  = colors.fill2;
    lblPitch_->colFrame_ = colors.fill3;

    lblGate_->colBkgnd_ = colors.text3;
    lblGate_->colText_  = colors.fill2;
    lblGate_->colFrame_ = colors.fill3;

    lblState_->colFrame_ = colTransparent;
}


void TabView::updateProgramBox()
{
    Bank* bank = editor_->getBank();
    INT32 programNum = editor_->getProgramNumber();
    SelectBoxList programNames;

    for( UINT16 i=0; i<bank->size(); i++ )
    {
        Program* program = bank->getProgram( i );
        programNames.push_back( program->name_ );
    }
    sbProgram_->setList( programNames, 20 );
    sbProgram_->setIndex( programNum );
}


bool TabView::programMessage( Editor::ProgramMessage msg, CView* sender )
{
    updateProgramBox();
    return true;
}


bool TabView::addTab( CView* view, CControl* button )
{
    bool result = CTabView::addTab( view, button );

    CViewContainer* tabContainer = dynamic_cast< CViewContainer* >( getView( 0 ));
    if( tabContainer ) {
        tabContainer->setAutosizeFlags( kAutosizeBottom );
    }
    return result;
}


bool TabView::selectTab( long index )
{
    bool result = CTabView::selectTab( index );

    if( index == 0 ) {
        if( isChild( sbProgram_ ) == false )     // sbProgram must be last child, because it's dropdown must draw over the other views
        {
            sbProgram_->setVisible( true );
            addView( sbProgram_ );
        }
    } else {
        sbProgram_->setVisible( false );
        removeView( sbProgram_, false );
    }
    return result;
}


bool TabView::onDrop( CDragContainer* drag, const CPoint& pos )
{
	return editor_->onDrop( drag, pos );
}


void TabView::setAppState( Editor::AppState state )
{
    if( lblState_ )
    {
        switch( state ) {
        case Editor::Running:   lblState_->colBkgnd_ = colGreen;     break;
        case Editor::Suspended: lblState_->colBkgnd_ = colYellow;    break;
        case Editor::Stopped:   lblState_->colBkgnd_ = colGrey;      break;
        case Editor::NotReady:  lblState_->colBkgnd_ = colRed;       break;
        }
        lblState_->invalid();
    }
}


void TabView::setMonitor( INT16 numSounding, FLOAT pitch, FLOAT gate, UINT16 flags )
{
    if( flags & MonitorVoices )
    {
        ostringstream osVoices;
        osVoices << numSounding;
        lblVoices_->text_ = osVoices.str();
    }
    if( flags & MonitorPitch )
    {
        if( pitch == -1 ) lblPitch_->text_ = "";
        else {
            ostringstream osPitch;
            osPitch << setprecision( 0 ) << fixed << (INT16)pitch;
            lblPitch_->text_ = osPitch.str();
        }
    }
    if( flags & MonitorGate )
    {
        if( gate == -1 ) lblGate_->text_ = "";
        else {
            ostringstream osGate;
            osGate << setprecision( 0 ) << fixed << (INT16)gate;
            lblGate_->text_ = osGate.str();        
        }
    } 
    lblVoices_->setDirty();
    lblPitch_->setDirty();
    lblGate_->setDirty();

    if( editor_->isStandalone() )
        updateMonitor();
}


void TabView::updateMonitor()
{
    if( lblPitch_->isDirty() )
        lblPitch_->invalid();
    if( lblGate_->isDirty() )
        lblGate_->invalid();
    if( lblVoices_->isDirty() )
        lblVoices_->invalid();
}


