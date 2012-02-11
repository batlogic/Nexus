
#include "CtrlView.h"
#include "ProgramView.h"
#include "TextEdit.h"
#include "Fonts.h"
#include "Images.h"
#include "../Program.h"
#include "../Synth.h"
#include "../ModuleData.h"
#include "../ModuleInfo.h"



//--------------------------------------------------------------
// class CtrlView
//--------------------------------------------------------------

CtrlView::CtrlView( const CRect& rcSize, Editor* editor ) : 
    CViewContainer( rcSize, editor->getChildFrame() ),
	editor_( editor ),
    ctrlPanel_( NULL )
{
	setMouseableArea( rcSize );
	setTransparency( true );
    setWantsFocus( false );
    setAutosizeFlags( kAutosizeRight|kAutosizeTop|kAutosizeBottom );
}


bool CtrlView::removed( CView* parent )
{
    getFrame()->setFocusView( NULL );   // set active TextEdits to idle
    return CViewContainer::removed( parent );
}


void CtrlView::updateModule( ModuleData* data )
{
	removeAll();

    if( data ) 
    {
	    CRect rcLabel( 20, 0, size.width()-18, 10+18 );
        CRect rcPanel( 10, 45, size.width()-8, size.bottom );
        	
        TextEdit* textEdit = new TextEdit( rcLabel, this, CmdModuleLabel, data->label_ );
        textEdit->font_    = fontPirulen14;
        textEdit->style_   = TextEdit::NeedsReturn | TextEdit::SelectOnOpen; 
        textEdit->openOn_  = TextEdit::DblClick;
        textEdit->data_    = data;
        textEdit->setTextColors( colors.text3, colBlack, colors.text3, colBlack );
	    addView( textEdit );

        ctrlPanel_ = new CtrlPanel( rcPanel, editor_, this, data );
        addView( ctrlPanel_->getScrollView() );
    }
    invalid();
}


void CtrlView::updateProgram()
{
    removeAll();
    Program* program = editor_->getCurrentProgram();

    if( program )
    {
	    CCoord textHeight = fontArial11->getSize() + 8;

	    CRect rcLabel( 18, 0, size.width()-10, 18 );
        CRect rcNameLabel( 18, 40, 80, 40+16 );
        CRect rcNameEdit( 18, 55, size.width()-18, 55+textHeight );
        CRect rcCatLabel( 18, 85, 80, 85+16 );
        CRect rcCatEdit( 18, 100, size.width()-18, 100+textHeight );
        CRect rcCommentLabel( 18, 130, 80, 130+16 );
        CRect rcCommentEdit( 18, 145, size.width()-18, 145+textHeight );
        CRect rcVoicesLabel( 153, size.height()-32, 184, size.height()-20 );
        CRect rcVoicesEdit( 153, size.height()-18, 183, size.height() );

        CRect rcHoldLabel( 40, size.height()-129, 200, size.height()-113 );
        CRect rcHoldEdit( 18, size.height()-129, 34, size.height()-113 );
        CRect rcRetrigLabel( 40, size.height()-92, 200, size.height()-76 );
        CRect rcRetrigEdit( 18, size.height()-92, 34, size.height()-76 );
        CRect rcLegatoLabel( 40, size.height()-55, 200, size.height()-39 );
        CRect rcLegatoEdit( 18, size.height()-55, 34, size.height()-39 );
        CRect rcUnisonLabel( 182, size.height()-32, 215, size.height()-20 );
        CRect rcUnisonEdit( 182, size.height()-18, 214, size.height() );
        CRect rcSpreadLabel( 213, size.height()-32, 245, size.height()-20 );
        CRect rcSpreadEdit( 213, size.height()-18, 260, size.height() );
        CRect rcPanic( 18, size.height()-18, 150, size.height() );

        Label* label;
        TextEdit* textEdit;
        NumericEdit* numEdit;
        Checkbox* checkbox;

        label           = new Label( rcLabel, "Program" );
	    label->font_    = fontPirulen14;
	    label->colText_ = colors.text3;
	    addView( label );

	    label           = new Label( rcNameLabel, "Name:" );
	    label->font_    = fontArial9;
	    label->colText_ = colors.text3;
	    addView( label );
        
        textEdit         = new TextEdit( rcNameEdit, this, CmdProgramName, program->name_ );
        textEdit->style_ = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
        textEdit->font_  = fontArial11;
        textEdit->setTextColors( colors.text3, colBlack, colors.text3, colBlack );
        textEdit->setBkgndColors( colTransparent, colWhite, colors.selection );
        textEdit->setFrameColors( colors.fill3, colors.fill3 );
	    addView( textEdit );

        label           = new Label( rcCatLabel, "Category:" );
	    label->font_    = fontArial9;
	    label->colText_ = colors.text3;
	    addView( label );

        textEdit         = new TextEdit( rcCatEdit, this, CmdProgramCategory, program->category_ );
        textEdit->style_ = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
        textEdit->font_  = fontArial11;
        textEdit->setTextColors( colors.text3, colBlack, colors.text3, colBlack );
        textEdit->setBkgndColors( colTransparent, colWhite, colors.selection );
        textEdit->setFrameColors( colors.fill3, colors.fill3 );
	    addView( textEdit );

	    label           = new Label( rcCommentLabel, "Comment:" );
	    label->font_    = fontArial9;
	    label->colText_ = colors.text3;
	    addView( label );

        textEdit         = new TextEdit( rcCommentEdit, this, CmdProgramComment, program->comment_ );
        textEdit->style_ = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
        textEdit->font_  = fontArial11;
        textEdit->setTextColors( colors.text3, colBlack, colors.text3, colBlack );
        textEdit->setBkgndColors( colTransparent, colWhite, colors.selection );
        textEdit->setFrameColors( colors.fill3, colors.fill3 );
	    addView( textEdit );

	    label           = new Label( rcHoldLabel, "Hold" );
	    label->font_    = fontArial9;
	    label->colText_ = colors.text3;
        label->setAutosizeFlags( kAutosizeRight|kAutosizeBottom );
	    addView( label );

        checkbox            = new Checkbox( rcHoldEdit, this, CmdProgramHold, program->hold_ );
        checkbox->colTick_  = colors.control1;
        checkbox->colBkgnd_ = colors.control2;
        checkbox->setAutosizeFlags( kAutosizeRight|kAutosizeBottom );
        addView( checkbox );

        label           = new Label( rcRetrigLabel, "Mono/Retrigger" );
	    label->font_    = fontArial9;
	    label->colText_ = colors.text3;
        label->setAutosizeFlags( kAutosizeRight|kAutosizeBottom );
	    addView( label );

        checkbox            = new Checkbox( rcRetrigEdit, this, CmdProgramRetrigger, program->retrigger_ );
        checkbox->colTick_  = colors.control1;
        checkbox->colBkgnd_ = colors.control2;
        checkbox->setAutosizeFlags( kAutosizeRight|kAutosizeBottom );
        addView( checkbox );

	    label           = new Label( rcLegatoLabel, "Mono/Legato" );
	    label->font_    = fontArial9;
	    label->colText_ = colors.text3;
        label->setAutosizeFlags( kAutosizeRight|kAutosizeBottom );
	    addView( label );

        checkbox            = new Checkbox( rcLegatoEdit, this, CmdProgramLegato, program->legato_ );
        checkbox->colTick_  = colors.control1;
        checkbox->colBkgnd_ = colors.control2;
        checkbox->setAutosizeFlags( kAutosizeRight|kAutosizeBottom );
        addView( checkbox );

        label           = new Label( rcVoicesLabel, "Vcs" );
	    label->font_    = fontArial9;
	    label->colText_ = colors.text3;
        label->setAutosizeFlags( kAutosizeRight|kAutosizeBottom );
	    addView( label );

        numEdit            = new NumericEdit( rcVoicesEdit, this, CmdProgramNumVoices, program->numVoices_, NumericEdit::Integer );
        numEdit->style_    = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
        numEdit->minValue_ = 1;
        numEdit->maxValue_ = 64;
        numEdit->format_   = NumericEdit::Integer;
        numEdit->font_     = fontLucida11;
        numEdit->hAlign_   = kCenterText;
        numEdit->setTextColors( colors.text3, colBlack, colors.text3, colBlack );
        numEdit->setBkgndColors( colTransparent, colWhite, colors.selection );
        numEdit->setFrameColors( colors.fill3, colors.fill3 );
        numEdit->setAutosizeFlags( kAutosizeRight|kAutosizeBottom );
	    addView( numEdit );

        label           = new Label( rcUnisonLabel, "Uni" );
	    label->font_    = fontArial9;
	    label->colText_ = colors.text3;
        label->setAutosizeFlags( kAutosizeRight|kAutosizeBottom );
	    addView( label );

        numEdit            = new NumericEdit( rcUnisonEdit, this, CmdProgramNumUnison, program->numUnison_, NumericEdit::Integer );
        numEdit->style_    = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
        numEdit->minValue_ = 1;
        numEdit->maxValue_ = 8;
        numEdit->format_   = NumericEdit::Integer;
        numEdit->font_     = fontLucida11;
        numEdit->hAlign_   = kCenterText;
        numEdit->setTextColors( colors.text3, colBlack, colors.text3, colBlack );
        numEdit->setBkgndColors( colTransparent, colWhite, colors.selection );
        numEdit->setFrameColors( colors.fill3, colors.fill3 );
        numEdit->setAutosizeFlags( kAutosizeRight|kAutosizeBottom );
	    addView( numEdit );

        label           = new Label( rcSpreadLabel, "Sprd" );
	    label->font_    = fontArial9;
	    label->colText_ = colors.text3;
        label->setAutosizeFlags( kAutosizeRight|kAutosizeBottom );
	    addView( label );

        numEdit            = new NumericEdit( rcSpreadEdit, this, CmdProgramSpread, program->unisonSpread_, NumericEdit::Integer );
        numEdit->style_    = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
        numEdit->minValue_ = 0;
        numEdit->maxValue_ = 12000;
        numEdit->format_   = NumericEdit::Integer;
        numEdit->font_     = fontLucida11;
        numEdit->hAlign_   = kCenterText;
        numEdit->setTextColors( colors.text3, colBlack, colors.text3, colBlack );
        numEdit->setBkgndColors( colTransparent, colWhite, colors.selection );
        numEdit->setFrameColors( colors.fill3, colors.fill3 );
        numEdit->setAutosizeFlags( kAutosizeRight|kAutosizeBottom );
	    addView( numEdit );

        KickButton* button = new KickButton( rcPanic, "Panic", this, CmdPanic );
        button->font_ = fontPirulen9;
        button->setWantsFocus( false );
        button->setColors( colTransparent, colors.text3, colors.text3, colors.selection );
        button->setAutosizeFlags( kAutosizeRight|kAutosizeBottom );
	    addView( button );
    }
    invalid();
}



void CtrlView::valueChanged( CControl* pControl )
{
    Program* program = editor_->getCurrentProgram();
    if( pControl == NULL || program == NULL ) 
        return;
    
    TextEdit* textEdit   = NULL;
    Checkbox* checkbox   = NULL;
    NumericEdit* numEdit = NULL; 

    if( pControl->isTypeOf( "TextEdit" ) || pControl->isTypeOf( "NumericEdit" ))
        textEdit = dynamic_cast< TextEdit* >( pControl );
    if( pControl->isTypeOf( "Checkbox" ))
        checkbox = dynamic_cast< Checkbox* >( pControl );
    if( pControl->isTypeOf( "NumericEdit" ))
        numEdit = dynamic_cast< NumericEdit* >( pControl );
    
    long tag = pControl->getTag();
    switch( tag ) 
    {
    case CmdProgramName:     
        program->name_ = textEdit->getText(); 
        editor_->programMessage( Editor::ProgramEdited, this );
        break;
    case CmdProgramCategory: 
        program->category_ = textEdit->getText(); 
        editor_->programMessage( Editor::ProgramEdited, this );
        break;
    case CmdProgramComment:  
        program->comment_ = textEdit->getText(); 
        editor_->programMessage( Editor::ProgramEdited, this );
        break;
    case CmdProgramHold:
        program->setHold( checkbox->isChecked() );
        editor_->getSynth()->getPolyphony()->setHold( checkbox->isChecked() );
        break;
    case CmdProgramRetrigger:
        program->setRetrigger( checkbox->isChecked() );
        editor_->getSynth()->getPolyphony()->setRetrigger( checkbox->isChecked() );
        break;
    case CmdProgramLegato:
        program->setLegato( checkbox->isChecked() );
        editor_->getSynth()->getPolyphony()->setLegato( checkbox->isChecked() );
        break;
    case CmdProgramNumVoices: 
        editor_->getSynth()->setNumVoices( (UINT16)numEdit->getValue() );
        break;
    case CmdProgramNumUnison:
        program->setNumUnison( (UINT16)numEdit->getValue() );
        editor_->getSynth()->getPolyphony()->setNumUnison( (UINT16)numEdit->getValue() );
        break;
    case CmdProgramSpread:
        program->setUnisonSpread( (UINT16)numEdit->getValue() );
        editor_->getSynth()->getPolyphony()->setUnisonSpread( (UINT16)numEdit->getValue() );
        break;
    case CmdPanic:
        if( pControl->getValue() == 0 )
            editor_->getSynth()->getPolyphony()->allNotesOff();
        break;
    case CmdModuleLabel:   
    {
        ModuleData* data = (ModuleData*)textEdit->data_;
        data->label_     = textEdit->getText();

        editor_->getProgramView()->setBoxLabel( data->id_, data->label_ );
        break;
    }
    }
}


CMouseEventResult CtrlView::onMouseDown( CPoint& pos, const long& buttons )
{
    CView* mouseDownView = getViewAt( pos, true );
    if( mouseDownView == NULL ) {
        getFrame()->setFocusView( editor_->getProgramView() );
    }
    return CViewContainer::onMouseDown( pos, buttons );
}



bool CtrlView::removeAll( const bool &withForget )  
{ 
    mouseDownView = NULL; 
    ctrlPanel_    = NULL;
    return CViewContainer::removeAll( withForget ); 
}


void CtrlView::setPreferredSize( const CRect& rcSize )
{
    if( ctrlPanel_ )
        ctrlPanel_->setPreferredSize( rcSize );
}


//--------------------------------------------------------------
// class ScrollbarDrawer
//--------------------------------------------------------------

void ScrollbarDrawer::drawScrollbarBackground( CDrawContext* pContext, const CRect& size, CScrollbar::ScrollbarDirection direction, CScrollbar* bar)
{
	pContext->setFillColor( bar->getBackgroundColor() );
	pContext->drawRect( size, kDrawFilled );
}


void ScrollbarDrawer::drawScrollbarScroller( CDrawContext* pContext, const CRect& size, CScrollbar::ScrollbarDirection direction, CScrollbar* bar)
{
	CRect rc = size;
    rc.inset( -2, -2 );
    pContext->setFillColor( bar->getScrollerColor() );
	pContext->drawRect( rc, kDrawFilled );
}


//--------------------------------------------------------------
// class CtrlPanel
//--------------------------------------------------------------

CtrlPanel::CtrlPanel( const CRect& rcSize, Editor* editor, CtrlView* parent, ModuleData* data ) :
    CViewContainer( rcSize, editor->getChildFrame() ),
	editor_( editor ),
    parent_( parent )
{
    setMouseableArea( size );
    setTransparency( true );
    setAutosizeFlags( kAutosizeAll );
    
    CRect rcStrip( 0, 0, size.width()-16, 30 );
    size( 0, 0, size.width(), size.height() );
    rcPreferred_  = size;
    clientHeight_ = size.top;

    long style  = CScrollView::kVerticalScrollbar|CScrollView::kDontDrawFrame;
    scrollView_ = new ScrollView( rcSize, rcSize, editor_->getChildFrame(), style );
    scrollView_->setTransparency( true );
    scrollView_->enableScrollbars( false, false );
    scrollView_->setAutosizeFlags( kAutosizeAll );
    scrollView_->addView( this );

    CScrollbar* vsb = scrollView_->getVerticalScrollbar();
    vsb->setBackgroundColor( colors.control2 );
    vsb->setFrameColor( colTransparent );
    vsb->setScrollerColor( colors.control1 );
    vsb->setDrawer( &drawer_ );
    vsb->setWantsFocus( false );
    
    for( ParamDataMap::iterator it=data->params_.begin(); it!=data->params_.end(); it++)
	{
        ParamData* paramData = &it->second;
		if( paramData->style_ > CS_INVISIBLE || data->links_.empty() == false ) 
        {
            ParamStrip* strip = new ParamStrip( rcStrip, editor_, data, paramData );
		    addView( strip );
            clientHeight_ += strip->getHeight();
		    rcStrip.offset( 0, strip->getHeight() );
        }
	}
    rescale();
}


void CtrlPanel::setPreferredSize( const CRect& rcSize )   
{ 
    rcPreferred_( 0, 0, rcSize.width(), rcSize.height()-45 );
    rescale();
}


void CtrlPanel::rescale()
{
    CRect rcUsed( rcPreferred_.left, rcPreferred_.top, rcPreferred_.right, clientHeight_ );

    CScrollbar* bar = scrollView_->getVerticalScrollbar();
    CRect rcBar     = bar->getViewSize();
    CCoord height   = rcPreferred_.height() - 12;
    bar->setViewSize( CRect( rcBar.left, rcBar.top, rcBar.right, rcBar.top+height ), false );

    scrollView_->setContainerSize( rcUsed, true );
    scrollView_->enableScrollbars( false, clientHeight_ > rcPreferred_.height()-30 );

    size( size.left, size.top, size.right, size.top+rcUsed.height() );
    setMouseableArea( rcUsed );
}


CMouseEventResult CtrlPanel::onMouseDown( CPoint& pos, const long& buttons )
{
    CView* mouseDownView = getViewAt( pos, true );
    if( mouseDownView == NULL || mouseDownView->isTypeOf( "TextSlider") == false ) {
        getFrame()->setFocusView( editor_->getProgramView() );
    }
    return CViewContainer::onMouseDown( pos, buttons );
}


//--------------------------------------------------------------
// class ParamStrip
//--------------------------------------------------------------

ParamStrip::ParamStrip( const CRect& rcSize, Editor* editor, ModuleData* data, ParamData* paramData ) :
	CViewContainer( rcSize, editor->getChildFrame(), NULL ),
    editor_( editor ),
    data_( data )
{
	setTransparency( true );

    CRect rc( 0, 0, size.width(), size.height() );
    if( paramData->style_ > CS_INVISIBLE )
    {
        addControl( rc, paramData->id_+ParamOffset, paramData );
        rc.offset( 0, 30 );
    }

    for( UINT16 i=0; i<data_->links_.size(); i++ ) 
    {
        LinkData* linkData = &data_->links_.at( i );
        if( linkData->inputId_ == paramData->id_ && linkData->style_ > CS_INVISIBLE ) 
        {
            addControl( rc, i+LinkOffset, linkData );
            rc.offset( 0, 30 );
        }
    }

    rc( size.left, size.top, size.right, size.top+rc.bottom-30 );
    setViewSize( rc );
    setMouseableArea( rc );
}


void ParamStrip::addControl( const CRect& rc, UINT16 id, ParamData* paramData )
{
    CRect rcLabel( rc.right-85, rc.top, rc.right, rc.top+18 );
	CRect rcSlider( rc.left+10, rc.top, rc.right-95, rc.top+18 );
    CRect rcCheckbox( rc.left+10, rc.top, rc.left+28, rc.top+18 );
    CRect rcNumEdit( rc.left+10, rc.top, rc.left+50, rc.top+18 );

    Label* label    = new Label( rcLabel, paramData->label_ );
    label->font_    = fontArial9;
	label->colText_ = colors.text3;
	label->hAlign_  = kLeftText;
    label->setMouseableArea( rcLabel );
	addView( label );

    switch( paramData->style_ )
    {
    case CS_SLIDER:
    case CS_BISLIDER:
    {
        TextSlider* slider = new TextSlider( rcSlider, this, id, paramData );
	    addView( slider );
        label->data_ = slider;
        break;
    }
    case CS_CHECKBOX:
    {
        Checkbox* cb  = new Checkbox( rcCheckbox, this, id, paramData->value_ );
        cb->data_     = paramData;
        cb->colTick_  = colors.control1;
        cb->colBkgnd_ = colors.control2;
        addView( cb );
        break;
    }
    case CS_INTEDIT:
    {
        NumericEdit* edit = new NumericEdit( rcNumEdit, this, id, paramData->value_, NumericEdit::Integer );
        edit->data_       = paramData;
        edit->font_       = fontLucida11;
        edit->minValue_   = paramData->min_;
        edit->maxValue_   = paramData->max_;
        edit->style_      = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
        edit->setTextColors( colors.control1, colBlack, colBlack, colBlack );
        edit->setBkgndColors( colors.control2, colWhite, colors.selection );
        addView( edit );
        break;
    }
    }
    attachOrDetach( paramData );
}


CMouseEventResult ParamStrip::onMouseDown( CPoint& pos, const long& buttons )
{
    if( buttons & kDoubleClick ) {
        showCtrlDialog( pos );
    }
    return CViewContainer::onMouseDown( pos, buttons );
}


void ParamStrip::valueChanged( CControl* pControl )
{
    UINT16 id = (UINT16)pControl->getTag(); 

    if( pControl->isTypeOf( "Checkbox" )) {
        Checkbox* checkbox        = dynamic_cast< Checkbox* >( pControl );
        data_->params_[id].value_ = checkbox->isChecked() ? 1.f : 0.f;
    }
    if( pControl->isTypeOf( "NumericEdit" )) {
        NumericEdit* edit         = dynamic_cast< NumericEdit* >( pControl );
        data_->params_[id].value_ = edit->getValue();
    }

    if( id < LinkOffset )                         
    {
        ParamData& paramData = data_->params_.at( id );
        data_->owner_->setParameter( id, paramData.value_ );
    }
    else
    {
        id -= LinkOffset;
        LinkData& linkData = data_->links_.at( id );
        Program* program   = editor_->getCurrentProgram();
        ModuleData* source = program->find( linkData.sourceId_ );

        source->owner_->setModulation( &linkData );
    }
}


void ParamStrip::attachOrDetach( ParamData* paramData )    // TODO: MidiListener wäre vielleicht besser bei TextSlider aufgehoben
{
    Polyphony* polyphony = editor_->getSynth()->getPolyphony();

    if( paramData->controller_ >= 0 ) 
    {
        midiInterest_ |= MidiListener::Controller;
        polyphony->attachMidiListener( this );
    } 
    else {
        midiInterest_ &= ~paramData->controller_; 
        polyphony->detachMidiListener( this );
    }
}


void ParamStrip::onController( UINT16 controller, FLOAT value )
{
    for( CCView* pSv = pFirstView; pSv; pSv = pSv->pNext ) {
		if( pSv->pView->isTypeOf( "TextSlider" ))
        {
            TextSlider* slider   = dynamic_cast< TextSlider* >( pSv->pView );
            ParamData* paramData = slider->paramData_;

            if( paramData->scaleController( controller, value )) {
                slider->setValue( value );
            }
        }
	}
}


CMouseEventResult ParamStrip::showCtrlDialog( const CPoint& pos ) 
{
    CView* view = getViewAt( pos, true );
    if( view && view->isTypeOf( "Label" ))	
    {
		Label* label = dynamic_cast< Label* >( view );
        if( label && label->data_ )
        {
            TextSlider* slider   = (TextSlider*)label->data_;
            ParamData* paramData = slider->paramData_;

            CPoint dialogPos( pos );
	        localToFrame( dialogPos );

            CtrlDialog dialog( pParentView, this, paramData );
            dialog.show( dialogPos, true );

            if( dialog.state_ == ModalDialog::Committed )
            {
                *paramData = dialog.paramData_;    
                label->setText( paramData->label_ );
                slider->init();
                valueChanged( slider );
                attachOrDetach( paramData );
            }
            return kMouseEventHandled;
        }
	}
    return kMouseEventNotHandled;
}



//--------------------------------------------------------------
// class CtrlDialog
//--------------------------------------------------------------

CtrlDialog::CtrlDialog( CView* owner, ParamStrip* paramStrip, ParamData* paramData ) : 
    ModalDialog( owner ),
    paramStrip_( paramStrip )
{
	ASSERT( paramData );
    paramData_ = *paramData;

    CBitmap* imgBkgnd     = images.get( Images::bkgndCtrlDialog );
    CBitmap* imgBtnOk     = images.get( Images::btnOk );
    CBitmap* imgBtnCancel = images.get( Images::btnCancel );

    setTransparency( true );
    setBackground( imgBkgnd );
    size( 0, 0, imgBkgnd->getWidth(), imgBkgnd->getHeight() );

    CRect rcCaption( 5, 5, 200, 15 );
    CRect rcLblLabel( 30, 35, 30+80, 35+17 );
    CRect rcEdtLabel( 115, 35, 115+80, 35+17 );
    CRect rcLblVelo( 30, 62, 30+80, 62+17 );
    CRect rcEdtVelo( 115, 62, 115+80, 62+17 );
    CRect rcLblDefault( 235, 35, 235+80, 35+17 );
    CRect rcEdtDefault( 310, 35, 310+80, 35+17 );
    CRect rcLblKtrack( 235, 62, 235+80, 62+17 );
    CRect rcEdtKtrack( 310, 62, 310+80, 62+17 );

    CRect rcLblSlider( 30, 112, 30+80, 112+17 );
    CRect rcLblBipol( 115, 112, 115+80, 112+17 );
    CRect rcEdtBipol( 178, 112, 178+17, 112+17 );
    CRect rcLblSliderMin( 30, 139, 30+60, 139+17 );
    CRect rcEdtSliderMin( 115, 139, 115+80, 139+17 );
    CRect rcLblSliderMax( 30, 166, 30+80, 166+17 );
    CRect rcEdtSliderMax( 115, 166, 115+80, 166+17 );
    CRect rcLblSliderSteps( 30, 193, 30+80, 193+17 );
    CRect rcEdtSliderSteps( 115, 193, 115+80, 193+17 );
    CRect rcLblRes( 30, 205, 30+80, 205+40 );
    CRect rcEdtRes( 115, 220, 115+80, 220+17 );
    CRect rcLblLog( 30, 232, 30+80, 232+40 );
    CRect rcEdtLog( 115, 247, 115+80, 247+17 );
    CRect rcLblUnit( 30, 273, 30+80, 273+17 );
    CRect rcEdtUnit( 115, 273, 115+80, 273+17 );

    CRect rcLblCC( 235, 112, 235+80, 112+17 );
    CRect rcEdtCC( 310, 112, 310+17, 112+17 );
    CRect rcLblCCNum( 235, 166, 235+80, 166+17 );
    CRect rcEdtCCNum( 310, 166, 310+80, 166+17 );
    CRect rcLblCCMin( 235, 193, 235+80, 193+17 );
    CRect rcEdtCCMin( 310, 193, 310+80, 193+17 );
    CRect rcLblCCMax( 235, 220, 235+80, 220+17 );
    CRect rcEdtCCMax( 310, 220, 310+80, 220+17 );
    CRect rcLblSoft( 235, 258, 235+80, 258+40 );
    CRect rcEdtSoft( 310, 273, 310+17, 273+17 );

    rcBtnOk_( 371, 315, 371+20, 315+20 );
    rcBtnCancel_( 396, 315, 396+20, 315+20 );

    CKickButton* button;
    Label* label;
    TextEdit* textEdit;
    NumericEdit* numEdit;
    Checkbox* checkbox;

    button = new CKickButton( rcBtnOk_, this, CmdCommit, imgBtnOk->getHeight()/2, imgBtnOk );
    button->setWantsFocus( false );
	addView( button );
	
    button = new CKickButton( rcBtnCancel_, this, CmdCancel, imgBtnCancel->getHeight()/2, imgBtnCancel );
    button->setWantsFocus( false );
	addView( button );
        
    // Top

    lblCaption_          = new Label( rcCaption, "Properties: " + paramData_.label_ );
	lblCaption_->font_   = fontPirulen9;
    lblCaption_->colText_= colors.dlgText;
	addView( lblCaption_ );

    label              = new Label( rcLblLabel, "Label" );
	label->font_       = fontArial11;
	label->colText_    = colors.dlgText;
	addView( label );

    textEdit           = new TextEdit( rcEdtLabel, this, CmdLabel, paramData_.label_ );
    textEdit->font_    = fontArial10;
    textEdit->style_   = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
    textEdit->setBkgndColors( colors.dlgTextBkgnd, colWhite, colors.selection );
    textEdit->setTextColors( colors.dlgText, colBlack, colors.dlgText, colBlack );
	addView( textEdit );

    label              = new Label( rcLblVelo, "VeloSens" );
	label->font_       = fontArial11;
	label->colText_    = colors.dlgText;
	addView( label );

    numEdit            = new NumericEdit( rcEdtVelo, this, CmdVeloSens, paramData_.veloSens_, NumericEdit::Float );
    numEdit->minValue_ = -100.0f;
    numEdit->maxValue_ = 100.0f;
    numEdit->font_     = fontLucida11;
    numEdit->style_    = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
    numEdit->setBkgndColors( colors.dlgTextBkgnd, colWhite, colors.selection );
    numEdit->setTextColors( colors.dlgText, colBlack, colors.dlgText, colBlack );
	addView( numEdit );

    label              = new Label( rcLblDefault, "Default" );
	label->font_       = fontArial11;
	label->colText_    = colors.dlgText;
	addView( label );

    numEdit            = new NumericEdit( rcEdtDefault, this, CmdDefault, paramData_.defaultValue_, NumericEdit::Float );
    numEdit->font_     = fontLucida11;
    numEdit->style_    = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
    numEdit->setBkgndColors( colors.dlgTextBkgnd, colWhite, colors.selection );
    numEdit->setTextColors( colors.dlgText, colBlack, colors.dlgText, colBlack );
	addView( numEdit );

    label              = new Label( rcLblKtrack, "KeyTracking" );
	label->font_       = fontArial11;
	label->colText_    = colors.dlgText;
	addView( label );

    numEdit            = new NumericEdit( rcEdtKtrack, this, CmdKeyTrack, paramData_.keyTrack_, NumericEdit::Float );
    numEdit->minValue_ = -100.0f;
    numEdit->maxValue_ = 100.0f;
    numEdit->font_     = fontLucida11;
    numEdit->style_    = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
    numEdit->setBkgndColors( colors.dlgTextBkgnd, colWhite, colors.selection );
    numEdit->setTextColors( colors.dlgText, colBlack, colors.dlgText, colBlack );
	addView( numEdit );
    
    // Slider

    label              = new Label( rcLblSlider, "Slider" );
	label->font_       = fontArial11;
	label->colText_    = colors.dlgText;
	addView( label );

    label              = new Label( rcLblBipol, "Bipol" );
	label->font_       = fontArial11;
	label->colText_    = colors.dlgText;
	addView( label );

    checkbox           = new Checkbox( rcEdtBipol, this, CmdBipolar, paramData_.style_ == CS_BISLIDER );
    checkbox->colBkgnd_= colors.dlgTextBkgnd;
    checkbox->colFrame_= colTransparent;
    checkbox->colTick_ = colors.dlgText;
	addView( checkbox );

    label              = new Label( rcLblSliderMin, "Min" );
	label->font_       = fontArial11;
	label->colText_    = colors.dlgText;
	addView( label );

    numEdit            = new NumericEdit( rcEdtSliderMin, this, CmdParamMin, paramData_.min_, NumericEdit::Float );
    numEdit->font_     = fontLucida11;
    numEdit->style_    = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
    numEdit->setBkgndColors( colors.dlgTextBkgnd, colWhite, colors.selection );
    numEdit->setTextColors( colors.dlgText, colBlack, colors.dlgText, colBlack );
    addView( numEdit );

    label              = new Label( rcLblSliderMax, "Max" );
	label->font_       = fontArial11;
	label->colText_    = colors.dlgText;
	addView( label );

    numEdit            = new NumericEdit( rcEdtSliderMax, this, CmdParamMax, paramData_.max_, NumericEdit::Float );
    numEdit->font_     = fontLucida11;
    numEdit->style_    = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
    numEdit->setBkgndColors( colors.dlgTextBkgnd, colWhite, colors.selection );
    numEdit->setTextColors( colors.dlgText, colBlack, colors.dlgText, colBlack );
	addView( numEdit );

    label              = new Label( rcLblSliderSteps, "Steps" );
	label->font_       = fontArial11;
	label->colText_    = colors.dlgText;
	addView( label );

    numEdit            = new NumericEdit( rcEdtSliderSteps, this, CmdParamSteps, paramData_.numSteps_, NumericEdit::Integer );
    numEdit->minValue_ = 0.0f;
    numEdit->maxValue_ = SHRT_MAX;
    numEdit->font_     = fontLucida11;
    numEdit->style_    = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
    numEdit->setBkgndColors( colors.dlgTextBkgnd, colWhite, colors.selection );
    numEdit->setTextColors( colors.dlgText, colBlack, colors.dlgText, colBlack );
	addView( numEdit );

    label              = new Label( rcLblRes, "Mouse\nResolution" );
	label->font_       = fontArial11;
	label->colText_    = colors.dlgText;
	addView( label );

    numEdit            = new NumericEdit( rcEdtRes, this, CmdParamRes, paramData_.resolution_, NumericEdit::Float );
    numEdit->minValue_ = 0.1f;
    numEdit->maxValue_ = 10.0f;
    numEdit->font_     = fontLucida11;
    numEdit->style_    = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
    numEdit->setBkgndColors( colors.dlgTextBkgnd, colWhite, colors.selection );
    numEdit->setTextColors( colors.dlgText, colBlack, colors.dlgText, colBlack );
	addView( numEdit );

    label              = new Label( rcLblLog, "Logarithmic\nScaling" );
	label->font_       = fontArial11;
	label->colText_    = colors.dlgText;
	addView( label );

    numEdit            = new NumericEdit( rcEdtLog, this, CmdParamLog, paramData_.logFactor_, NumericEdit::Float );
    numEdit->minValue_ = 0.0f;
    numEdit->maxValue_ = 24.0f;
    numEdit->font_     = fontLucida11;
    numEdit->style_    = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
    numEdit->setBkgndColors( colors.dlgTextBkgnd, colWhite, colors.selection );
    numEdit->setTextColors( colors.dlgText, colBlack, colors.dlgText, colBlack );
	addView( numEdit );

    label              = new Label( rcLblUnit, "Unit" );
	label->font_       = fontArial11;
	label->colText_    = colors.dlgText;
	addView( label );

    textEdit           = new TextEdit( rcEdtUnit, this, CmdParamUnit, paramData_.unit_ );
    textEdit->style_   = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
    textEdit->setBkgndColors( colors.dlgTextBkgnd, colWhite, colors.selection );
    textEdit->setTextColors( colors.dlgText, colBlack, colors.dlgText, colBlack );
    textEdit->font_    = fontArial10;
	addView( textEdit );

    // CC

    label              = new Label( rcLblCC, "Controller" );
	label->font_       = fontArial11;
	label->colText_    = colors.dlgText;
	addView( label );

    checkbox           = new Checkbox( rcEdtCC, this, CmdCC, paramData_.controller_ > -1 );
    checkbox->colBkgnd_= colors.dlgTextBkgnd;
    checkbox->colFrame_= colTransparent;
    checkbox->colTick_ = colors.dlgText;
	addView( checkbox );

    label              = new Label( rcLblCCNum, "Number" );
	label->font_       = fontArial11;
	label->colText_    = colors.dlgText;
	addView( label );

    numEdit            = new NumericEdit( rcEdtCCNum, this, CmdCCNum, paramData_.controller_, NumericEdit::Integer );
    numEdit->minValue_ = -1.0f;
    numEdit->maxValue_ = 127.0f;
    numEdit->font_     = fontLucida11;
    numEdit->style_    = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
    numEdit->setBkgndColors( colors.dlgTextBkgnd, colWhite, colors.selection );
    numEdit->setTextColors( colors.dlgText, colBlack, colors.dlgText, colBlack );
	addView( numEdit );

    label              = new Label( rcLblCCMin, "Min" );
	label->font_       = fontArial11;
	label->colText_    = colors.dlgText;
	addView( label );

    numEdit            = new NumericEdit( rcEdtCCMin, this, CmdCCMin, paramData_.controllerMin_, NumericEdit::Float );
    numEdit->font_     = fontLucida11;
    numEdit->style_    = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
    numEdit->setBkgndColors( colors.dlgTextBkgnd, colWhite, colors.selection );
    numEdit->setTextColors( colors.dlgText, colBlack, colors.dlgText, colBlack );
    addView( numEdit );

    label              = new Label( rcLblCCMax, "Max" );
	label->font_       = fontArial11;
	label->colText_    = colors.dlgText;
	addView( label );

    numEdit            = new NumericEdit( rcEdtCCMax, this, CmdCCMax, paramData_.controllerMax_, NumericEdit::Float );
    numEdit->font_     = fontLucida11;
    numEdit->style_    = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
    numEdit->setBkgndColors( colors.dlgTextBkgnd, colWhite, colors.selection );
    numEdit->setTextColors( colors.dlgText, colBlack, colors.dlgText, colBlack );
	addView( numEdit );

    label              = new Label( rcLblSoft, "Soft\nTakeover" );
	label->font_       = fontArial11;
	label->colText_    = colors.dlgText;
	addView( label );

    checkbox           = new Checkbox( rcEdtSoft, this, CmdCCSoft, paramData_.controllerSoft_ );
    checkbox->colBkgnd_= colors.dlgTextBkgnd;
    checkbox->colFrame_= colTransparent;
    checkbox->colTick_ = colors.dlgText;
	addView( checkbox );
}


void CtrlDialog::valueChanged( CControl *pControl )
{
    Command cmd = (Command)pControl->getTag();
    float value = 0.0f;
    string text;

    if( pControl->isTypeOf( "NumericEdit" )) {
        NumericEdit* numEdit = dynamic_cast< NumericEdit* >( pControl );
        value = numEdit->getValue();
    }
    if( pControl->isTypeOf( "TextEdit" )) {
        TextEdit* textEdit = dynamic_cast< TextEdit* >( pControl );
        text = textEdit->getText();
    }
    if( pControl->isTypeOf( "Checkbox" )) {
        Checkbox* checkbox = dynamic_cast< Checkbox* >( pControl );
        value = checkbox->isChecked() ? 1.0f : 0.0f;
    }

    switch( cmd ) 
    {
    case CmdLabel:
    {
        paramData_.label_   = text;
        lblCaption_->setText( "Properties: " + text );
        break;
    }
    case CmdVeloSens:   paramData_.veloSens_     = value; break;
    case CmdDefault:    paramData_.defaultValue_ = value; break;
    case CmdKeyTrack:   paramData_.keyTrack_     = value; break;
    
    case CmdBipolar:    paramData_.style_        = value ? CS_BISLIDER : CS_SLIDER; break;
    case CmdParamMin:   paramData_.min_          = value; break;
    case CmdParamMax:   paramData_.max_          = value; break;
    case CmdParamSteps: paramData_.numSteps_     = (UINT16)value; break;
    case CmdParamLog:   paramData_.logFactor_    = value; break;
    case CmdParamRes:   paramData_.resolution_   = value; break;
    case CmdParamUnit:  paramData_.unit_         = text;  break;

    case CmdCC:    
    {
        paramData_.controller_ = value ? 0 : -1;
        CControl* control      = getControlByTag( CmdCCNum );
        control->setValue( paramData_.controller_ );
        break;
    }
    case CmdCCNum: 
    {
        paramData_.controller_ = (INT16)value; 
        CControl* control      = getControlByTag( CmdCC );
        float checked          = paramData_.controller_ > -1 ? 1.0f : 0.0f;
        control->setValue( checked );
        break;
    }
    case CmdCCMin:  paramData_.controllerMin_  = value; break;
    case CmdCCMax:  paramData_.controllerMax_  = value; break;
    case CmdCCSoft: paramData_.controllerSoft_ = value ? true : false; break;

    default: ModalDialog::valueChanged( pControl );
    }
}


void CtrlDialog::drawBackgroundRect( CDrawContext* pContext, CRect& rcUpdate )
{
    ModalDialog::drawBackgroundRect( pContext, rcUpdate );

    CRect rcClient( 0, 18, size.width()-10, size.height()-10 );
    CRect rcCaption( 0, 0, rcClient.right, 18 );
    CRect rcLeftFrame( 16, 97, 16+206, 97+205 );
    CRect rcRightFrame( 221, 97, 221+195, 97+205 );
    
    pContext->setFillColor( colors.dlgFill1 );
    pContext->drawRect( rcClient, kDrawFilled );

    pContext->setLineWidth( 1 );
    pContext->setFrameColor( colors.dlgFill1 );
    pContext->setFillColor( colors.dlgCaption );
    pContext->drawRect( rcCaption, kDrawFilledAndStroked );

    pContext->setFillColor( colTransparent );
    pContext->setFrameColor( colors.dlgTextBkgnd );
    pContext->drawRect( rcLeftFrame, kDrawFilledAndStroked );
    pContext->drawRect( rcRightFrame, kDrawFilledAndStroked );

    pContext->setFillColor( colors.dlgFill2 );
    pContext->drawRect( rcBtnOk_, kDrawFilled );
    pContext->drawRect( rcBtnCancel_, kDrawFilled );
}



//-------------------------------------------------------------------
// class TextSlider
//-------------------------------------------------------------------

TextSlider::TextSlider( const CRect& rcSize, CControlListener* listener, long tag, ParamData* paramData ) :
    CControl( rcSize, listener, tag ),
    slider_( NULL ),
    numEdit_( NULL ),
    paramData_( paramData )
{
    setWantsFocus( true );
    setAutosizeFlags( kAutosizeNone );
    
    CRect rcSlider = size;
    CRect rcEdit   = CRect( size.left+2, size.top+2, size.right-2, size.bottom-2 );
    
    slider_             = new Slider( rcSlider, this, tag );
    slider_->style_     = paramData_->style_ - 1;
	slider_->colHandle_ = colors.control1;
	slider_->colBkgnd_  = colors.control2;

    numEdit_          = new NumericEdit( rcEdit, this, tag, paramData_->value_, NumericEdit::Float );
    numEdit_->openOn_ = TextEdit::DblClick;
    numEdit_->style_  = TextEdit::SelectOnOpen | TextEdit::NeedsReturn;
    numEdit_->font_   = fontArial9;
    numEdit_->setTextColors( colors.text2, colBlack, colors.text2, colors.text2 );
    numEdit_->setBkgndColors( colTransparent, MakeCColor( colors.control2.red, colors.control2.green, colors.control2.blue, 99 ), colors.selection );
    numEdit_->setFrameColors( colTransparent, colTransparent );

    init();
}


TextSlider::~TextSlider()
{
    delete slider_;
    delete numEdit_;
}


void TextSlider::init()
{
    slider_->style_        = paramData_->style_ - 1;
    slider_->defaultValue_ = paramData_->defaultValue_;
    slider_->minValue_     = paramData_->min_;
    slider_->maxValue_     = paramData_->max_;
    slider_->numSteps_     = paramData_->numSteps_;
    slider_->data_         = paramData_;
    slider_->setResolution( paramData_->resolution_ );
    slider_->setValue( paramData_->makeLinear( paramData_->value_ ));

    numEdit_->unit_      = paramData_->unit_;
    numEdit_->minValue_  = paramData_->min_;
    numEdit_->maxValue_  = paramData_->max_;
    numEdit_->format_    = (NumericEdit::NumberFormat)paramData_->format_;
    numEdit_->precision_ = (int)max( 0, ceil( log10( (FLOAT)paramData_->numSteps_ ) / ( paramData_->max_-paramData_->min_ )));
    numEdit_->setValue( paramData_->value_ );
    numEdit_->setVisible( false );
}


bool TextSlider::attached( CView* parent )
{
	bool result = CControl::attached( parent );
    slider_->attached( parent );
    numEdit_->attached( parent );

	return result;
}


bool TextSlider::removed( CView* parent )
{
	slider_->removed( parent );
    numEdit_->removed( parent );

    return CControl::removed( parent );
}


void TextSlider::parentSizeChanged()
{
    numEdit_->parentSizeChanged();
}


CMouseEventResult TextSlider::onMouseDown( CPoint& pos, const long& buttons )
{
	getFrame()->setFocusView( this );
    
    CPoint p( pos );
	p.offset( -size.left, -size.top );
    CMouseEventResult result = kMouseEventNotHandled;
    
    if( (buttons & kLButton) && (buttons & kControl) )  // leftClick+Ctrl: set numEdit to editing
    {
        CView* view = getViewAt( pos );
        if( view == numEdit_ )              
        {
            slider_->looseFocus();
            numEdit_->setState( TextEdit::Editing );
            return kMouseEventHandled;
        }
    }

    if( numEdit_->isEditing() ) 
    {
        CView* view = getViewAt( pos );
        if( view == slider_ ) {
            numEdit_->looseFocus();
        }
        result = numEdit_->onMouseDown( p, buttons );
    }
    else 
    {
        numEdit_->setState( TextEdit::Idle );
        slider_->takeFocus();
        result = slider_->onMouseDown( p, buttons );
    }
    return result;
}


CMouseEventResult TextSlider::onMouseMoved( CPoint& pos, const long& buttons )
{
	CPoint p( pos );
	p.offset( -size.left, -size.top );
    CMouseEventResult result = kMouseEventNotHandled;

    if( numEdit_->isEditing() ) {
        result = numEdit_->onMouseMoved( p, buttons );
    } else {
        result = slider_->onMouseMoved( p, buttons );
    }
    return result;

}


CMouseEventResult TextSlider::onMouseUp( CPoint& pos, const long& buttons )
{
	CPoint p( pos );
	p.offset( -size.left, -size.top );
    CMouseEventResult result = kMouseEventNotHandled;

    if( numEdit_->isEditing() ) {
        result = numEdit_->onMouseUp( p, buttons );
    } else {
        result = slider_->onMouseUp( p, buttons );
        if( size.pointInside( pos ) == false ) {
            numEdit_->setVisible( false );
        }
    }
    if( result == kMouseEventHandled ) {
        invalid();
    }
    return result;
}


CMouseEventResult TextSlider::onMouseEntered( CPoint& pos, const long& buttons )
{
    numEdit_->setVisible( true );
    numEdit_->onMouseEntered( pos, buttons );
    invalid();
    return kMouseEventHandled;
}


CMouseEventResult TextSlider::onMouseExited( CPoint& pos, const long& buttons )
{
    CPoint p( pos );
    frameToLocal( p );

    if( size.pointInside( p ) == false )
    {
        if( numEdit_->isVisible() && numEdit_->isEditing() == false ) {
            numEdit_->setVisible( false );
            invalid();
        }
    }
    numEdit_->onMouseExited( pos, buttons );
    return kMouseEventHandled;
}


long TextSlider::onKeyDown( VstKeyCode& keyCode )
{
    if( numEdit_->isEditing() ) 
        return numEdit_->onKeyDown( keyCode );
    else 
        return slider_->onKeyDown( keyCode );
}


void TextSlider::looseFocus()
{
    if( numEdit_->isEditing() )
        numEdit_->looseFocus();
}


void TextSlider::valueChanged( CControl* pControl )
{
    if( pControl == numEdit_ ) 
    {
        FLOAT value = numEdit_->getValue();
        value       = paramData_->makeLinear( value );
        slider_->takeFocus();
        slider_->setValue( value );
    }
    else if( pControl == slider_ ) 
    {
        FLOAT value = slider_->getValue();
        value = paramData_->makeLogarithmic( value );
        numEdit_->setValue( value );
        paramData_->value_ = value;
    }
    paramData_->resetController();
    invalid();
    listener->valueChanged( this );
}


float TextSlider::getValue() const
{
    return paramData_->value_;
}


void TextSlider::setValue( float value )
{
    slider_->setValue( value );
    numEdit_->setValue( value );
}


CView* TextSlider::getViewAt( const CPoint& pos ) const
{
	if( bVisible == false )
		return NULL;

    if( pos.isInside( numEdit_->getMouseableArea() )) {
        return numEdit_;
    }
    else return slider_;
}


bool TextSlider::verifyView( CView* view ) const  
{ 
    return view == this || view == slider_ || view == numEdit_; 
}


void TextSlider::draw( CDrawContext* pContext )
{
    slider_->draw( pContext );
    if( numEdit_->isVisible()) numEdit_->draw( pContext );

    setDirty( false );
}


