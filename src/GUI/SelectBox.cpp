
#include "SelectBox.h"
#include "Widgets.h"



SelectBox::SelectBox( const CRect& rcSize, CControlListener* listener, long tag ) :
    CControl (rcSize, listener, tag ),
    scrollbar_( NULL ),
    state_( 0 ),
    dropDir_( Dropdown ),
	font_( kSystemFont ),
    index_( 0 ), 
    indexSel_( 0 ),
    indexSaved_( 0 ),
    maxVisible_( 3 ),
    scrollOffset_( 0 )
{
    calcMetrics();

    CRect rcScrollbar( size.right-WidthButton, size.bottom, size.right, size.bottom+150 );
    scrollbar_ = new CScrollbar( rcScrollbar, this, 0, CScrollbar::kVertical, CRect( 0,0,0,0 ));

    setWantsFocus( true );
    setBackgroundColor( kWhiteCColor );
    setFrameColor( kBlackCColor );
    setTextColor( kBlackCColor );
    setSelectionColor( colOrange );
}


void SelectBox::getText( INT32 index, string& text )
{
    text = index < (INT32)list_.size() ? list_[ index ] : "";
}


void SelectBox::calcMetrics()
{
    rcArrow_( size.right-WidthButton+3, size.top+size.height()/2-2, size.right-WidthButton+8, size.top+size.height()/2+2 );
    rcArrowSel_( size.right-WidthButton, size.top+1, size.right-1, size.bottom-1 );
    rcBox_     ( size.left, size.top, size.right, size.bottom );
    rcText_    ( size.left+2, size.top, size.right-WidthButton, size.bottom );

    CCoord dropHeight   = maxVisible_ * TextHeight;
    CCoord indent       = maxVisible_ < (CCoord)list_.size() ? WidthButton : 0;
    CCoord scrollHeight = (CCoord)(list_.size() * TextHeight);

    if( dropDir_ == Dropdown )
    {
        rcDropdown_  ( size.left, size.bottom, size.right-indent, size.bottom+dropHeight );
        rcScrollbar_ ( size.right-WidthButton, size.bottom, size.right, size.bottom+dropHeight );
        rcScrollsize_( size.left, size.bottom, size.right, size.bottom+scrollHeight );
        rcClip_      ( size.left, size.top, size.right, size.bottom+dropHeight );
    }
    else {
        rcDropdown_  ( size.left, size.top-dropHeight, size.right-indent, size.top );
        rcScrollbar_ ( size.right-WidthButton, size.top-dropHeight, size.right, size.top );
        rcScrollsize_( size.left, size.top, size.right, size.top+scrollHeight );
        rcClip_      ( size.left, size.top-dropHeight, size.right, size.bottom );
    }
}


SelectBox::~SelectBox()
{
    delete scrollbar_;
}


void SelectBox::setList( const SelectBoxList& list, INT32 numVisible )
{
    list_.clear();

    list_       = list;
    maxVisible_ = min( (INT32)list_.size(), numVisible );

    calcMetrics();

    if( maxVisible_ < (INT32)list_.size() ) 
    {
        scrollbar_->setVisible( true );
        scrollbar_->setViewSize( rcScrollbar_, false );
        scrollbar_->setScrollSize( rcScrollsize_ );
        scrollbar_->setWheelInc( 1 / (float)list_.size() );
    }
    else { 
        scrollbar_->setVisible( false );
        rcDropdown_.right = size.right;
    }
}


void SelectBox::setViewSize( CRect& rcSize, bool invalid )
{
    CControl::setViewSize( rcSize, invalid );

    CRect rcScrollbar( size.right-WidthButton, size.bottom, size.right, size.bottom+150 );
    scrollbar_->setViewSize( rcScrollbar, invalid );

    calcMetrics();
}


void SelectBox::getTextRect( CRect& rc, INT32 index )
{
    CCoord offset = 1 + index * TextHeight;
    rc( rcDropdown_.left, rcDropdown_.top+offset, rcDropdown_.right, rcDropdown_.top+offset+TextHeight );
}


INT32 SelectBox::indexFromPosition( const CPoint& pos )
{
    CPoint p( pos );

    if( dropDir_ == Dropdown )
        p.offset( 0, - (size.top + rcBox_.height() + 2 ));
    else 
        p.offset( 0, -rcDropdown_.top );

    INT32 offset = (INT32)( max( 0, p.y ) / TextHeight );
    return max( 0, min( maxVisible_-1+scrollOffset_, offset + scrollOffset_ ));
}


void SelectBox::setIndex( string s ) 
{
    for( INT32 i=0; i<(INT32)list_.size(); i++ ) {
        if( s == list_[i] ) 
        {
            indexSaved_ = scrollTo( i );
        }
    }
}


void SelectBox::setIndex( INT32 idx ) 
{ 
    indexSaved_ = scrollTo( idx ); 
}


void SelectBox::setState( UINT16 state )
{
    if( state != state_ )
    {
        state_ = state;
        invalidRect( rcClip_ );

        ChildFrame* frame = dynamic_cast< ChildFrame* >( getFrame() );
        if( frame ) 
        {
            if( state_ & Dropped ) {
                frame->setMouseCaptureView( this );
                indexSel_ = index_;
            } else {
                frame->setMouseCaptureView( NULL );
            }
        }
    }
}


void SelectBox::takeFocus()
{
    state_ |= Focused;
    invalidRect( rcBox_ );
}


void SelectBox::looseFocus()
{
    state_ &= ~Focused;
    invalidRect( rcBox_ );

    commit( true );
}


void SelectBox::commit( bool commit )
{
    if( commit == false ) {
        index_ = indexSaved_;
    }
    indexSaved_ = index_;
    setState( state_ & ~(Dropped|Hover) );

    if( listener ) {
        value = (float)indexSaved_;
        listener->valueChanged( this );
    }
}


void SelectBox::setBackgroundColor( const CColor& col )
{
    colBkgnd_ = col;
    scrollbar_->setBackgroundColor( col );
}


void SelectBox::setFrameColor( const CColor& col )
{
    colFrame_ = col;
    scrollbar_->setFrameColor( col );
    scrollbar_->setScrollerColor( col );
}


void SelectBox::setTextColor( const CColor& col )          
{ 
    colText_ = col; 
}


void SelectBox::setSelectionColor( const CColor& col )     
{ 
    colSel_ = col; 
}


void SelectBox::setFont( CFontRef font ) 
{
    font_ = font;
}


void SelectBox::valueChanged( CControl* pControl )
{
    if( rcScrollsize_.height() > rcDropdown_.height() )
	{
		float value   = pControl->getValue();
        CCoord offset = (CCoord)( (rcScrollsize_.height() - rcDropdown_.height()) * value );
        scrollOffset_ = (INT32)( offset / (TextHeight-0.5f) );
        scrollOffset_ = min( (INT32)list_.size() - maxVisible_, scrollOffset_ );
	}
    invalidRect( rcClip_ );
}


CMouseEventResult SelectBox::onMouseDown( CPoint& pos, const long& buttons )
{
	if( list_.empty() == false ) 
    {
        getFrame()->setFocusView( this ); 
        state_ |= Focused;

        if( buttons & kLButton )
        {
            if( (state_ & Dropped) == 0 && rcBox_.pointInside( pos )) {
                setState( state_ | Dropped );
            } 
            else { 
                if( rcScrollbar_.pointInside( pos )) {                  // click in scrollbar
                    scrollbar_->onMouseDown( pos, buttons );
                }
                else if( rcDropdown_.pointInside( pos ) == false ) {    // click somewhere outside
                    commit( false );
                }
            }
            return kMouseEventHandled;
        }
    }
    return kMouseEventNotHandled;
}


CMouseEventResult SelectBox::onMouseMoved( CPoint& pos, const long& buttons )
{
    if( (state_ & Dropped) )  
    {
        if( rcScrollbar_.pointInside( pos ) && scrollbar_->isVisible() )    // draging the scrollbar handle
        {
            scrollbar_->onMouseMoved( pos, buttons );
        }
        else if( rcDropdown_.pointInside( pos ) && buttons != kRButton )    // hovering or draging inside list
        {
            INT32 oldIdx = indexSel_;
            indexSel_    = indexFromPosition( pos );

            if( oldIdx != index_ ) {
                invalidRect( rcClip_ );
            }
        }
    }
    return kMouseEventHandled;
}


CMouseEventResult SelectBox::onMouseUp( CPoint& pos, const long& buttons )
{
    if( state_ & Dropped )
    {
        if( rcClip_.pointInside( pos ) == false ) {
            commit( false );
        }
        else if( rcDropdown_.pointInside( pos ) && rcBox_.pointInside( pos ) == false ) 
        {
            index_ = indexFromPosition( pos );
            commit( true );
        }
        return kMouseEventHandled;
    }
    return kMouseEventNotHandled;
}


CMouseEventResult SelectBox::onMouseEntered( CPoint& pos, const long& buttons )
{
	if( (state_ & Dropped) == 0 ) {
        state_ |= Hover;
        invalid();
    }
    return kMouseEventHandled;
}


CMouseEventResult SelectBox::onMouseExited( CPoint& pos, const long& buttons )
{
	if( (state_ & Dropped) == 0 ) {
        state_ &= ~Hover;
        invalid();
    }
	return kMouseEventHandled;
}


long SelectBox::onKeyDown( VstKeyCode& keyCode ) 
{
    switch( keyCode.virt ) 
    {
    case VKEY_UP:
        if( state_ & Dropped ) {
            scrollTo( indexSel_-1 ); 
        } else {
            if( (keyCode.modifier & MODIFIER_ALTERNATE) == 0 ) {
                index_ = max( 0, index_-1 );
                commit( true );
            } else if (dropDir_ == Dropup ) {
                setState( state_ | Dropped );
            }
        }
        return 1;

    case VKEY_DOWN: 
        if( state_ & Dropped ) {
            scrollTo( indexSel_+1 ); 
        }
        else {
            if( (keyCode.modifier & MODIFIER_ALTERNATE) == 0 ) {
                index_ = min( (INT32)list_.size()-1, index_+1 );
                commit( true );
            } else if (dropDir_ == Dropdown ) {
                setState( state_ | Dropped );
            }
        }
        return 1;

    case VKEY_RETURN: 
        if( state_ & Dropped ) {
            commit( true );  
            return 1;
        }
    case VKEY_ESCAPE: 
        if( state_ & Dropped ) {
            commit( false ); 
            return 1;
        }
    }
    return 0;
}


bool SelectBox::onWheel( const CPoint& pos, const CMouseWheelAxis& axis, const float& distance, const long& buttons )
{
    if( axis == kMouseWheelAxisY ) 
    {
        if (state_ & Dropped ) 
        {
            INT32 oldIdx = indexSel_;
            bool result  = scrollbar_->onWheel( pos, axis, distance, buttons );
            indexSel_    = indexFromPosition( pos );

            if( oldIdx != index_ ) {
                invalidRect( rcClip_ );
            }
            return result;
        }
        else if( rcBox_.pointInside( pos ) ) {
            scrollTo( index_ + (INT32)distance*-1 );
        }
    }
	return false;
}


INT32 SelectBox::scrollTo( INT32 index )
{
    index_    = max( 0, min( (INT32)list_.size()-1, index ));
    indexSel_ = index_;

    if( index_ >= maxVisible_ + scrollOffset_ ) 
        scrollOffset_ = (index_ - maxVisible_) + 1;
    if( index_ < scrollOffset_ ) 
        scrollOffset_ = index_;

    int offset  = scrollOffset_ * TextHeight;
    float value = offset / (float)(rcScrollsize_.height() - rcDropdown_.height());
    scrollbar_->setValue( value );

    if( (state_ & Dropped) == 0 ) {
        invalid();
    }
    else { 
        invalidRect( rcClip_ );
    }
    return index_;
}


void SelectBox::draw( CDrawContext* pContext )
{
    pContext->setFont( font_, font_->getSize());
	pContext->setFontColor( colText_ );

    pContext->setFrameColor( colFrame_ );
    pContext->setLineWidth( 1 );

    pContext->setFillColor( colBkgnd_ );
    pContext->drawRect( rcBox_, kDrawFilledAndStroked );

    if( (state_ & Hover) || state_ && Focused )  {
        pContext->setFillColor( colSel_ );
        pContext->drawRect( rcArrowSel_, kDrawFilled );
    }
	if( index_ < (INT32)list_.size() ) {
        pContext->drawStringUTF8( list_[ index_ ].c_str(), rcText_, kLeftText, true );
    }
    DrawHelper::drawTriangle( pContext, rcArrow_, colText_, false );
    
    if( state_ & Dropped )
    {
       	CRect rcOldClip;
    	pContext->getClipRect( rcOldClip );
        pContext->setClipRect( rcClip_ );

        drawDropdown( pContext );
        if( scrollbar_->isVisible() ) {
            scrollbar_->draw( pContext );
        }
        pContext->setClipRect( rcOldClip );
    }
    setDirty( false );
}


void SelectBox::drawDropdown( CDrawContext* pContext )
{
    pContext->setFillColor( colBkgnd_ );
    pContext->drawRect( rcDropdown_, kDrawFilledAndStroked );

    ASSERT( indexSel_ >= 0 && (indexSel_ < (INT32)list_.size() || list_.empty()));
    if( indexSel_ >= 0 && (indexSel_ <= (INT32)list_.size()))
    {
        CRect rcSel;
        getTextRect( rcSel, indexSel_-scrollOffset_ );
        rcSel.offset( 0, -1 );
        pContext->setFillColor( colSel_ );
        pContext->drawRect( rcSel, kDrawFilledAndStroked );
    
        CRect rc;
        for( INT32 i=0; i+scrollOffset_<(INT32)list_.size() && rc.bottom<rcDropdown_.bottom; i++ ) 
        {
            getTextRect( rc, i );
            rc.left += 2;
            pContext->drawStringUTF8( list_[ i+scrollOffset_ ].c_str(), rc, kLeftText, true );
        }
    }
}


