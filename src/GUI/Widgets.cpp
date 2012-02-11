

#include "Widgets.h"
#include "MainFrame.h"
#include "Fonts.h"
#include "Images.h"
#include "../ModuleData.h"
#include "../ModuleInfo.h"





//-------------------------------------------
// class ScrollView
//-------------------------------------------
//
ScrollView::ScrollView( const CRect &size, const CRect &containerSize, CFrame* frame, long style ) :
    CScrollView( size, containerSize, frame, style )
{}


void ScrollView::enableScrollbars( bool horizontal, bool vertical )
{
    CRect rcContainer = sc->getViewSize();
    const CPoint& scrollOffset = sc->getScrollOffset();

    if( style & kHorizontalScrollbar )
    {
        int height = hsb->getHeight();
        if( horizontal && !hsb->isVisible() ) {
            rcContainer.bottom -= height;
        }
        else if( !horizontal && hsb->isVisible() ) {
            rcContainer.bottom += height;
            sc->setScrollOffset( CPoint( 0, scrollOffset.y ));
        }
        hsb->setVisible( horizontal );
        hsb->invalid();
    }
    if( style & kVerticalScrollbar )
    {
        int width = vsb->getWidth();
        if( vertical && !vsb->isVisible() ) {
            rcContainer.right -= width;
            sc->setScrollOffset( CPoint( scrollOffset.x, 0 ));
        }
        else if( !vertical && vsb->isVisible() ) {
            rcContainer.right += width;
        }
        vsb->setVisible( vertical );
        vsb->invalid();
    }    
    sc->setViewSize( rcContainer );
    sc->setMouseableArea( rcContainer );
    sc->invalid();
}



void ScrollView::setScrollOffset( CPoint& offset )
{
    const CPoint& scrollOffset = sc->getScrollOffset();
	CRect vs;
	sc->getViewSize( vs );

    if( offset != scrollOffset )
	{
		sc->setScrollOffset( offset );
        if( vsb )
		{
			vsb->setValue( (float)(offset.y - vs.top) / (float)(containerSize.getHeight () - vs.getHeight ()) );
			valueChanged (vsb);
		}
		if (hsb)
		{
			hsb->setValue( (float)(offset.x - vs.left) / (float)(containerSize.getWidth () - vs.getWidth ()) );
			valueChanged( hsb );
		}
	}
}



//-------------------------------------------
// class ModalDialog
//-------------------------------------------

ModalDialog::ModalDialog( CView* owner ) :
    CViewContainer( CRect( 0,0,0,0 ), owner->getFrame(), NULL ),
    owner_( owner ),
    mouseOverView_( NULL ),
    state_( Running )
{
}


void ModalDialog::show( CPoint& pos, bool centerPosition )
{
    centerPosition ? center() : moveTo( pos );

    CFrame* frame = getFrame();
    frame->setModalView( this );
    frame->setFocusView( this );

    MSG msg;
    while( state_ == Running && isAttached() && frame->getOpenFlag() )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
        {
            TranslateMessage( &msg );
            LRESULT result = DispatchMessage( &msg );

            if( result == 0 && msg.message == WM_KEYDOWN ) {
                onKeyDown( msg.wParam );
            }
        }
    }
    if( state_ == Running ) {
        state_ = Canceled;
    }
    if( isAttached() )
    {
        frame->setModalView( NULL );
        frame->setFocusView( owner_ );
    }
}


long ModalDialog::onKeyDown( WPARAM wParam )
{
    VstKeyCode keyCode;
    ChildFrame::makeVstKeyCode( wParam, keyCode );
    
    switch( keyCode.virt ) {
    case VKEY_ESCAPE: close( CmdCancel ); return 1;
    case VKEY_RETURN: close( CmdCommit ); return 1;
    case VKEY_TAB:
        advanceFocus( keyCode.modifier != MODIFIER_SHIFT );
        return 1;
    }
    return 0;
}


void ModalDialog::advanceFocus( bool forward )
{
    CFrame* frame = getFrame();
    if( frame && getNbViews() )
    {
        CView* next = NULL;
        CView* focusView = getFrame()->getFocusView();

        if( focusView == this ) {
            focusView = pFirstView->pView;
        }
        focusView = getNextFocusView( focusView, forward );
        if( focusView ) {
            frame->setFocusView( focusView );
            focusView->takeFocus();
        }
    }
}


CView* ModalDialog::getNextFocusView( CView* view, bool forward )
{
    CCView* item  = getCCView( view );
    CCView* entry = item;
    
    while( true )
    {
        item = forward ? item->pNext : item->pPrevious;
        if( item == NULL ) 
            item = forward ? pFirstView : pLastView;
        if( item == entry ) 
            break;
        if( item->pView->wantsFocus() ) 
            return item->pView;
    }
    return NULL;
}


CCView* ModalDialog::getCCView( CView* view )
{
    for( CCView* item = pFirstView; item; item = item->pNext ) 
	{
		if( item->pView == view ) 
            return item;
    }
    return NULL;
}


CControl* ModalDialog::getControlByTag( UINT16 tag )
{
    for( CCView* pSv = pFirstView; pSv; pSv = pSv->pNext ) {
        if( pSv->pView->isTypeOf( "CControl" )) 
        {
            CControl* control = dynamic_cast< CControl* >( pSv->pView );
            if( control->getTag() == tag )
                return control;
        }
    }
    return NULL;
}


void ModalDialog::center()
{
    CFrame* frame = getFrame();
    if( frame )
    {
        CRect rcFrame  = frame->getViewSize();
        CPoint topLeft = CPoint( rcFrame.width() / 2 - size.width() / 2, rcFrame.height() / 2 - size.height() / 2 );

        moveTo( topLeft );
    }
}


void ModalDialog::close( UINT16 cmd )
{
    switch( cmd ) {
    case CmdCommit: state_ = Committed; break;
    case CmdCancel: state_ = Canceled; break;
    }
    CFrame* frame = getFrame();
    if( frame ) frame->setFocusView( NULL );
}


CMouseEventResult ModalDialog::onMouseDown( CPoint& pos, const long& buttons )
{
    CView* mouseDownView = getViewAt( pos, true );
    if( mouseDownView == NULL ) {
        getFrame()->setFocusView( NULL );
    }
    if( buttons & kLButton ) {	
		if( rcTop_.pointInside( pos )) {
            trackData_.tracking_ = true;
            trackData_.offset_   = CPoint( size.left - pos.x, size.top - pos.y );

            return onMouseMoved( pos, buttons );
        }
    }
    return CViewContainer::onMouseDown( pos, buttons );
}


CMouseEventResult ModalDialog::onMouseUp( CPoint& pos, const long& buttons )
{
	CMouseEventResult result = CViewContainer::onMouseUp( pos, buttons );
    trackData_.tracking_ = false;
    
    return result;
}


CMouseEventResult ModalDialog::onMouseMoved( CPoint& pos, const long& buttons )
{
	if( trackData_.tracking_ && pos != size.getTopLeft() ) 
    {
        pos.offset( trackData_.offset_.x, trackData_.offset_.y );
        moveTo( pos );
        getFrame()->invalid();
        return kMouseEventHandled;
    }

	CView* v = getViewAt( pos, true );
	if( v != mouseOverView_ )
	{
		if( mouseOverView_ )
		{
			CPoint lr( pos );
			mouseOverView_->frameToLocal( lr );
			mouseOverView_->onMouseExited( lr, buttons );
		}
		mouseOverView_ = 0;
		if( v )
		{
			CPoint lr( pos );
			v->frameToLocal( lr );
			v->onMouseEntered( lr, buttons );
			mouseOverView_ = v;
		}
		return kMouseEventHandled;
	}
	else if( mouseOverView_ )
	{
		CPoint lr( pos );
		mouseOverView_->frameToLocal( lr );
		return mouseOverView_->onMouseMoved( lr, buttons );
	}
    return CViewContainer::onMouseMoved( pos, buttons );
    //return kMouseEventHandled;
}


void ModalDialog::valueChanged( CControl *pControl )
{
    UINT16 cmd = (Command)pControl->getTag();

    switch( cmd ) {
    case CmdCommit:   
    case CmdCancel:
    {
        if( pControl->getValue() == 0 )
            close( cmd ); 
        break;
    }
    }
}


void ModalDialog::moveTo( const CPoint& pos )
{
    size.moveTo( pos.x, pos.y );
    setViewSize( size );
    setMouseableArea( size );

    rcTop_( size.left, size.top, size.right-10, size.top+18 );
    rcBottom_( size.left, size.top+18, size.right-10, size.bottom-10 );
    rcClient_( size.left, size.top, size.right-10, size.bottom-10 );
}



//-------------------------------------------
// class Label
//-------------------------------------------
//
Label::Label( const CRect& rcSize, const string& text, CControlListener* listener, long tag ) :
	CControl( rcSize, listener, tag ), 
	text_( text ),
	hAlign_( kLeftText ),
	font_( kSystemFont ),
    indent_( 0, 0 ),
    data_( NULL ),
    onMouse_( OnMouseNothing )
{
	setTransparency( true );
    setWantsFocus( false );
    setMouseableArea( CRect( 0,0,0,0 ));
    setColors( colors.text2, colors.text2, colors.text1, colTransparent, colTransparent );
}


void Label::setText( const string& text )
{
    text_ = text;
    invalid();
}


void Label::setColors( 
    const CColor& colText, const CColor& colTextSel, const CColor& colTextFocus, 
    const CColor& colBkgnd, const CColor& colFrame )
{
    colText_      = colText;
    colTextSel_   = colTextSel;
    colTextFocus_ = colTextFocus;
    colBkgnd_     = colBkgnd;
    colFrame_     = colFrame;
}


CMouseEventResult Label::onMouseDown( CPoint& pos, const long& buttons )
{
    if( listener ) {
        if( (buttons & kLButton) && (onMouse_ & OnMouseLeftDown) )
        {
            listener->valueChanged( this );
        }
    }
    return kMouseEventNotHandled;
}


void Label::draw( CDrawContext *pContext )
{
    setDirty ( false );
    if( isVisible() == false ) 
        return;

    CRect rcBkgnd( size );
    CRect rcText( size.left+indent_.x, size.top+indent_.y, size.right-1, size.bottom );
    
    CColor colText = colText_;
    colText = isSelected() ? colTextSel_   : colText;
    colText = isFocused()  ? colTextFocus_ : colText;
    
    pContext->setFillColor( colBkgnd_ );
    pContext->setFrameColor( colFrame_ );
    pContext->setLineWidth( 1 );
    pContext->drawRect( rcBkgnd, kDrawFilledAndStroked );
    
    pContext->setFont( font_, font_->getSize());
    pContext->setFontColor( colText );
	pContext->drawStringUTF8( text_.c_str(), rcText, hAlign_, true );
}


bool Label::isSelected()
{
    long dummy;
    return getAttribute( ATTR_SELECTED, 0, NULL, dummy );
}


bool Label::isFocused()
{
    long dummy;
    return getAttribute( ATTR_FOCUSED, 0, NULL, dummy );
}






//-------------------------------------------
// class Checkbox
//-------------------------------------------
//
Checkbox::Checkbox( CRect& rcSize, CControlListener* listener, long tag, float state ) :
	CControl( rcSize , listener, tag, NULL ),
    colTick_( colors.control1 ),
    colFrame_ ( colTransparent ),
    colBkgnd_( colors.control2 ),
    data_( NULL )
{
    value = state;
}


void Checkbox::setState( bool state )
{
    value = state;
    invalid();
}


void Checkbox::setValue( float value )
{
    bool checked = value ? true : false;
    setState( checked );
}


CMouseEventResult Checkbox::onMouseDown( CPoint& pos, const long& buttons )
{
    value = !value;

	if( listener ) {
		listener->valueChanged( this );
	}
    invalid();
    return kMouseEventHandled;
}


void Checkbox::draw (CDrawContext *pContext)
{
    pContext->setLineWidth( 1 );
    pContext->setFillColor( colBkgnd_ );
    pContext->setFrameColor( colFrame_ );
	pContext->drawRect( size, kDrawFilledAndStroked );

    if( value ) 
    {
        CRect rcTick = size;
        if( tickSize_.x == 0 && tickSize_.y == 0 ) {
            rcTick.inset( 3, 3 );
        } else {
            CCoord left = size.left + size.width()/2 - tickSize_.x/2;
            CCoord top  = size.top  + size.height()/2 - tickSize_.y/2;
            rcTick( left, top, left+tickSize_.x, top+tickSize_.y );
        }

        pContext->setFillColor( colTick_ );
        pContext->drawRect( rcTick, kDrawFilled );
    }
	setDirty ( false );
}



//-------------------------------------------
// class Slider
//-------------------------------------------

Slider::Slider( const CRect& rcSize, CControlListener* listener, long tag ) :
	CControl( rcSize, listener, tag, NULL ),
    colHandle_( kGreyCColor ),
    colBkgnd_( kWhiteCColor ),
    style_( Monopolar ),
    value_( 0.0f ), 
    defaultValue_( 0.5f ),
    minValue_( 0.0f ), 
    maxValue_( 1.0f ),
    numSteps_( (UINT16)rcSize.width() ),
    finetuning_( 1.0f ),
    coef_( 0.0f ),
    tracking_( false ),
    focus_( false ),
    data_( NULL )
{
	orientation_ = size.width() > size.height() ? Horizontal : Vertical; 

    setResolution( 1.0f );
	setTransparency( false );
	setWantsFocus( true );
}


CMouseEventResult Slider::onMouseDown( CPoint& pos, const long& buttons )
{
    if( buttons & kDoubleClick ) 
    {
        setValue( defaultValue_ );
        notifyListener();
        return kMouseEventHandled;
    }
    else {
    	beginEdit();
        finetuning_  = buttons & kRButton ? 0.1f : 1.0f;
        posAnchor_   = pos;
        valueAnchor_ = value_;
        tracking_    = true;

        return onMouseMoved( pos, buttons );
    }
}


CMouseEventResult Slider::onMouseUp( CPoint& pos, const long& buttons )
{
	tracking_ = false;
    endEdit();

	return kMouseEventHandled;
}


CMouseEventResult Slider::onMouseMoved( CPoint& pos, const long& buttons )
{
	if( buttons & kLButton || buttons & kRButton )
	{
        if( tracking_ )
		{
            CPoint offset = CPoint( (CCoord)((pos.x - posAnchor_.x) * resolution_ * finetuning_), pos.y - posAnchor_.y );
            float value   = valueAnchor_ + offset2Value( offset );
                        
            setValue( value ); 
            notifyListener();
            return kMouseEventHandled;
		}
	}
	return kMouseEventNotHandled;
}


long Slider::onKeyDown( VstKeyCode& keyCode )
{
    float inc = 0.0f;
    switch( keyCode.virt ) 
    {
    case VKEY_RIGHT:  
        inc = numSteps_ ? (maxValue_ - minValue_) / (float)(numSteps_-0.5f) : (maxValue_ - minValue_) / (float)(size.width()-0.5f);
        setValue( value_ + inc );
        notifyListener();
        return 1;
    case VKEY_LEFT:     
        inc = numSteps_ ? (maxValue_ - minValue_) / (float)numSteps_ : (maxValue_ - minValue_) / (float)size.width();
        setValue( value_ - inc );
        notifyListener();
        return 1;
    case VKEY_HOME:
        setValue( minValue_ );
        notifyListener();
        return 1;
    case VKEY_END:
        setValue( maxValue_ );
        notifyListener();
        return 1;
    case VKEY_INSERT:
        setValue( defaultValue_ );
        notifyListener();
        return 1;
    }
    return 0;
}


float Slider::offset2Value( const CPoint& offset ) const
{
    switch( orientation_ ) 
    {
    case Horizontal: return offset.x * coef_; break;
    case Vertical:   return offset.y * coef_; break;
    default:         VERIFY( false );
    }
}

void Slider::value2Pos( const float value, CPoint& pos ) const
{
    pos( 0, 0 );

    switch( orientation_ ) 
    {
    case Horizontal: 
        pos.x = (CCoord)((value-minValue_) / coef_ + 0.5f); 
        pos.x = max( 0, min( size.width(), pos.x ));
        break;
    case Vertical:   
        pos.y = (CCoord)((value-minValue_) / coef_ + 0.5f); 
        pos.y = max( 0, min( size.height(), pos.y ));
        break;
    default: VERIFY( false );
    }
}


float Slider::rasterize( float value )
{
    if( numSteps_ > 0 )
    {
        double step = ( maxValue_ - minValue_ ) / (double)numSteps_; 
        int count   = int(( value - minValue_ ) / step);
        value       = (float)(minValue_ + step * count);
    }
    return value;
}


void Slider::notifyListener()
{
	if( listener ) {
		listener->valueChanged( this );
	}
}


float Slider::getValue() const 
{ 
    return value_; 
}


void Slider::setValue( float value )
{
    value  = rasterize( value );
    value  = max( minValue_, min( maxValue_, value ));
    value_ = value;

    setHandle( value_ );
}


void Slider::setResolution( float resolution )
{
    CCoord span = orientation_ == Horizontal ? size.width() : size.height();
    resolution_ = span / (span * resolution);
    coef_       = (maxValue_ - minValue_) / (float)span;
}


void Slider::setHandle( float value )
{
    CPoint offset;
    value2Pos( value, offset );

    if( orientation_ == Horizontal ) {
        style_ == Monopolar ?
	        rcHandle_( size.left, size.top, size.left+offset.x, size.bottom ) :
            rcHandle_( size.left+offset.x-10, size.top, size.left+offset.x+10, size.bottom );
        style_ == Monopolar ?
            rcFocus_( rcHandle_.right-7, rcHandle_.top+2, rcHandle_.right-3, rcHandle_.top+6 ) :
            rcFocus_( rcHandle_.left+8, rcHandle_.top+2, rcHandle_.left+11, rcHandle_.top+5 );

    } else {
        style_ == Monopolar ?
	        rcHandle_( size.left, size.bottom-offset.y, size.right, size.bottom ) :
            rcHandle_( size.left, size.bottom-offset.y-10, size.right, size.bottom-offset.y+10 );
    }
    invalid();
}


void Slider::takeFocus() 
{ 
    focus_ = true; 
    invalidRect( rcFocus_ ); 
}


void Slider::looseFocus()   
{ 
    focus_ = false; 
    invalidRect( rcFocus_ ); 
}


void Slider::draw( CDrawContext *pContext )
{
	pContext->setFillColor( colBkgnd_ );
	pContext->drawRect( size, kDrawFilled );

	pContext->setFillColor( colHandle_ );
	pContext->drawRect( rcHandle_, kDrawFilled );

    if( focus_ )
    {
        pContext->setFillColor( colBkgnd_ );
        pContext->drawRect( rcFocus_, kDrawFilled );
    }
    setDirty( false );
}


//-------------------------------------------
// class KickButtons
//-------------------------------------------

KickButton::KickButton( const CRect& rcSize, const string& label, CControlListener* listener, long tag ) :
    CKickButton( rcSize, listener, tag, NULL ),
    label_( label )
{
    setColors( colors.control1, colors.control2, colors.control1, colors.selection );
}


void KickButton::setColors( const CColor& colBkgnd, const CColor& colText, const CColor& colFrame, const CColor& colFrameKicked )
{
    colBkgnd_ = colBkgnd;
    colText_  = colText;
    colFrame_ = colFrame;
    colFrameKicked_ = colFrameKicked;
}


void KickButton::draw( CDrawContext *pContext )
{
	bounceValue();

    CColor colFrame = value ? colFrameKicked_ : colFrame_;

    pContext->setFrameColor( colFrame );
    pContext->setFillColor( colBkgnd_ );
    pContext->setLineWidth( 1 );
    pContext->drawRect( size, kDrawFilledAndStroked );

    pContext->setFont( font_, font_->getSize());
    pContext->setFontColor( colText_ );
	pContext->drawStringUTF8( label_.c_str(), size, kCenterText, true );

	setDirty( false );
}


//-------------------------------------------
// class Caret
//-------------------------------------------

Caret::Caret() :
    parent_( NULL ),
    hwnd_( NULL )
    {}


void Caret::show( CView* parent, CCoord height )
{
    ASSERT( parent->getFrame() );

    parent_ = parent;
    height_ = height;
    hwnd_   = (HWND)parent_->getFrame()->getSystemWindow();

    BOOL result = CreateCaret( hwnd_, NULL, 1, height_ );
    result      = ShowCaret( hwnd_ );
}


void Caret::hide()
{
    HideCaret( hwnd_ );
    DestroyCaret();
}


void Caret::setPosition( const CPoint& pos )
{
    if( parent_ ) 
    {
        position_ = pos;
        parent_->localToFrame( position_ );
        SetCaretPos( position_.x, position_.y );
    }
}



//-------------------------------------------
// class Rubberband
//-------------------------------------------

Rubberband::Rubberband() : 
    CView( CRect( 0,0,0,0 ) ),
    lineStyle_( kLineOnOffDash ),
    colLine_( colors.fill2 )
    {}


void Rubberband::begin( const CPoint& pos, const CPoint& offset )
{
    anchor_ = pos;
    offset_ = offset;
    anchor_.offset( -offset_.x, -offset_.y );
    setViewSize( CRect( anchor_.x, anchor_.y, anchor_.x, anchor_.y ));
    setVisible( true );
}


void Rubberband::stretch( const CPoint& pos )
{
    CCoord width  = pos.x - anchor_.x - offset_.x;
    CCoord height = pos.y - anchor_.y - offset_.y;

    CRect rcSize( anchor_.x, anchor_.y, anchor_.x+width, anchor_.y+height );
    rcSize.normalize();
    setViewSize( rcSize );
}


void Rubberband::hide() 
{
    setVisible( false );
}


void Rubberband::draw( CDrawContext* pContext )
{
    CLineStyle oldLineStyle = pContext->getLineStyle();

    pContext->setLineStyle( lineStyle_ );
    pContext->setLineWidth( 1 );
    pContext->setFillColor( colTransparent );
    pContext->setFrameColor( colLine_ );

    pContext->drawRect( size, kDrawFilledAndStroked );
    pContext->setLineStyle( oldLineStyle );
}


//-------------------------------------------
// class DrawHelper
//-------------------------------------------

void DrawHelper::drawTriangle( CDrawContext* pContext, const CRect& rcSize, CColor color, bool upwards )
{
    CColor colOld   = pContext->getFrameColor();
    CCoord oldWidth = pContext->getLineWidth();
    pContext->setFrameColor( color );

    CCoord lines  = rcSize.height();
    CCoord left   = rcSize.left;
    CCoord right  = rcSize.right;
    CCoord start  = rcSize.top;
    CCoord y      = start;

    for( CCoord i=0; i<lines; i++ )
    {
        if( left > right ) break;
        pContext->moveTo( CPoint( left, y ));
        pContext->lineTo( CPoint( right, y ));
        left++;
        right--;
        y = upwards ? y-1 : y+1;
    }
    pContext->setFrameColor( colOld );
    pContext->setLineWidth( oldWidth );
}
