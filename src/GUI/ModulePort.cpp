
#include "ModulePort.h"
#include "ModuleBox.h"
#include "Fonts.h"
#include "../ModuleInfo.h"


//----------------------------------------------------------------
// class ModulePort
//----------------------------------------------------------------

ModulePort::ModulePort( const CRect& size, PortInfo* info, PortAlignment align, ModuleBox* owner ) : 
	CView( size ),
	info_( info ),
	owner_( owner ),
	state_( Idle ),
    align_( align )
{
	ASSERT( info_ );
    setTransparency( true );
	setDirty( true );

	if( align_ == IN_PORT )
	{
		rcSquare_    = CRect( size.left+4, size.top+3, size.left+10, size.top+9 );
        rcIndicator_ = CRect( size.left, size.top+5, size.left+4, size.top+7 );
        rcHover_     = CRect( size.left, size.top, size.left+12, size.bottom );
        rcText_      = CRect( size.left+13, size.top, size.right+37, size.bottom );
        rcInvalid_   = CRect( size.left, size.top, size.right+37, size.bottom );
	}
	else {
		rcSquare_    = CRect( size.right-10, size.top+3, size.right-4, size.top+9 );
        rcIndicator_ = CRect( size.right-4, size.top+5, size.right, size.top+7 );
        rcHover_     = CRect( size.right-12, size.top, size.right, size.bottom );
		rcText_      = CRect( size.left-37, size.top, size.right-15, size.bottom );
        rcInvalid_   = CRect( size.left-37, size.top, size.right, size.bottom );
	}
}


CMouseEventResult ModulePort::onMouseEntered( CPoint& pos, const long& buttons )
{
	return onMouseMoved( pos, buttons );
}


CMouseEventResult ModulePort::onMouseExited( CPoint& pos, const long& buttons )
{
	state_ &= ~Hover;
    invalidRect( rcInvalid_ );

	return kMouseEventHandled;
}


CMouseEventResult ModulePort::onMouseDown( CPoint& pos, const long& buttons )
{
    return onMouseMoved( pos, buttons );
}


CMouseEventResult ModulePort::onMouseMoved( CPoint& pos, const long& buttons )
{
	UINT16 oldState = state_;
    setHover( pos );
    if( state_ != oldState ) {
        invalidRect( rcInvalid_ );
    }
    return kMouseEventHandled;
}


bool ModulePort::testDocking( const CPoint& pos )
{
  	CPoint p( pos );
    patchToPort( p );
    return testHover( p );
}


bool ModulePort::startDocking( CPoint& pos )
{
	patchToPort( pos );
    bool result = setHover( pos );

	if( result ) 
	{
        getDockPosition( pos );
		state_ |= Docking;
        invalidRect( rcInvalid_ );
	}
	return result;
}


void ModulePort::endDocking( CPoint& pos )
{
    if( links_.empty() )
        state_ &= ~Connected;
    
    patchToPort( pos );
    state_ &= ~Docking;
    setHover( pos );
    invalidRect( rcInvalid_ );
}


void ModulePort::connect( CPoint& pos, const LinkInfo& linkInfo )
{
	patchToPort( pos );
	setHover( pos );
    links_.push_back( linkInfo );
	getDockPosition( pos );
    state_ |= Connected;
}


void ModulePort::disconnect( const LinkInfo& linkInfo )
{
    vector< LinkInfo >::iterator pos = find( links_.begin(), links_.end(), linkInfo );
    if( pos != links_.end() ) {
        links_.erase( pos );
    }


    state_ &= ~Docking;
    if( links_.empty() ) {
        state_ &= ~Connected;
    }
}


bool ModulePort::testHover( const CPoint& pos )
{
    return rcHover_.pointInside( pos );
}


bool ModulePort::setHover( const CPoint& pos )
{
    testHover( pos ) ? state_ |= Hover : state_ &= ~Hover;
    return state_ & Hover;
}


void ModulePort::getDockPosition( CPoint& pos )
{
	pos.y = size.top + size.getHeight() / 2;
	pos.x = align_ == IN_PORT ? size.left+1 : size.right-1;
	portToPatch( pos );
}


void ModulePort::patchToPort( CPoint& pos ) 
{ 
	CRect rcParent = owner_->getViewSize();
	pos.offset( -rcParent.x, -rcParent.y );
}


void ModulePort::portToPatch( CPoint& pos )
{ 
	CRect rcParent = owner_->getViewSize();
	pos.offset( rcParent.x, rcParent.y );
}


void ModulePort::draw( CDrawContext *pContext )
{
	CColor colPort1 = (state_ & Connected) ? colors.modulePort2 : colors.modulePort1;
    CColor colPort2 = colors.modulePort2;
    if( owner_->data_->catalog_ == MASTER ) {
        colPort1 = (state_ & Connected) ? colors.masterPort2 : colors.masterPort1;
        colPort2 = colors.masterPort2;
    }
    pContext->setFillColor( colPort1 );
    pContext->setFrameColor( colPort2 );

	if( state_ & (Hover || Docking) )	    // draw port square
	{
		CRect rcFrame( rcSquare_ );
		rcFrame.inset( -1, -1 );
		pContext->setLineWidth( 1 );
		pContext->drawRect( rcFrame, kDrawFilledAndStroked );
	}
	else {
		pContext->drawRect( rcSquare_, kDrawFilled );
	}

	if( links_.empty() == false ) {	    	// draw connection indicator
		pContext->drawRect( rcIndicator_, kDrawFilled );
	}

	if( state_ & (Hover || Docking) )	    // draw label
	{
		CRect oldClip;
		pContext->getClipRect( oldClip );
		pContext->setClipRect( rcInvalid_ );

        pContext->setFont( fontArial9, fontArial9->getSize() );
        pContext->setFontColor( colors.moduleText2 );
		
        pContext->setFillColor( MakeCColor( owner_->colBkgnd_.red, owner_->colBkgnd_.green, owner_->colBkgnd_.blue, 255 ));
        pContext->drawRect( rcText_, kDrawFilled );

        pContext->drawStringUTF8( info_->label_.c_str(), rcText_, align_==IN_PORT ? kLeftText : kRightText, true );
		pContext->setClipRect( oldClip );
	}
	setDirty( false );
}

