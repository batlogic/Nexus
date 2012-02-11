
#include "ModuleBox.h"
#include "ModulePort.h"
#include "ProgramView.h"
#include "Fonts.h"
#include "Widgets.h"
#include "../ModuleInfo.h"




ModuleBox::ModuleBox( ProgramView* owner, ModuleData* data )
	: CViewContainer( CRect( 0,0,0,0 ), owner->getFrame(), NULL ),
	  owner_( owner ),
	  data_( data ),
	  selected_( false ),
      focused_( false ),
	  label_( NULL ),
      portHeight_( 12 )
{
	setVisible( ModuleInfo::getBoxStyle( data->catalog_ ) & BS_VISIBLE );
	
	setTransparency( false );
    setColors();
}


ModuleBox::~ModuleBox()
{
	data_ = NULL;
    for( WireMap::iterator it=wires_.begin(); it!=wires_.end(); it++ ) {
		delete it->second;
	}
}


CMouseEventResult ModuleBox::onMouseDown( CPoint& pos, const long& buttons )
{
	if( buttons & kDoubleClick && data_ ) 
    {
        bool collapsed = !data_->collapsed_;
        collapseOrExpand( collapsed );
    }
    return CViewContainer::onMouseDown( pos, buttons );
}


CCoord getTextHeight( const char* string, CFontRef font )
{
    size_t len = strlen( string );
    int count  = 1;

    for( size_t i=0; i<len; i++ ) 
    {
        if( string[i] == '\n' ) 
            ++count;
    }
    CCoord height = font->getSize() * count;
    return height;
}


void ModuleBox::realize()
{
    calculateSize();
    
	CCoord width      = size.getWidth();
	CCoord height     = size.getHeight();
    ModuleInfo* info  = ModuleInfo::getInfo( data_->catalog_ );

	if( info->boxStyle_ & BS_LABEL ) 
	{
		CCoord textHeight = getTextHeight( info->label_.c_str(), fontArial10 );
        label_            = new Label( CRect( 0, 0, width, 24 ), data_->label_ );
		label_->font_     = fontPirulen9;
		label_->hAlign_   = kCenterText;
		addView( label_ );
	}

    CCoord top    = portHeight_/2;
    CCoord offset = top;

	for( UINT16 i=0; i<info->inPorts_.size(); i++ )
	{
		PortInfo* portInfo   = &info->inPorts_[ i ];
		CRect r              = CRect( 0, offset, width/2, offset+portHeight_ );
		offset              += portHeight_;
		ModulePort* portView = new ModulePort( r, portInfo, IN_PORT, this );
		addView( portView );
	}
	
	offset = top;
	for( UINT16 i=0; i<info->outPorts_.size(); i++ )
	{
		PortInfo* portInfo   = &info->outPorts_[ i ];
		CRect r              = CRect( width-(width/2), offset, width, offset+portHeight_ );
		offset              += portHeight_;
		ModulePort* portView = new ModulePort( r, portInfo, OUT_PORT, this );
		addView( portView );
	}
    setColors();
    setPortVisibility();
}


void ModuleBox::createWires()
{
    for( UINT16 i=0; i<data_->links_.size(); i++ )
	{
		CPoint posSource, posTarget;
        LinkInfo& linkInfo = data_->getLink( i );
		
        ModulePort* inPort = getPort( linkInfo.inputId_, IN_PORT );
		inPort->connect( posTarget, linkInfo );

		ModulePort* outPort = owner_->getPort( linkInfo, OUT_PORT );
		outPort->connect( posSource, linkInfo );

        owner_->getDockPosition( linkInfo.sourceId_, linkInfo.outputId_, OUT_PORT, posSource );
        getDockPosition( linkInfo.inputId_, IN_PORT, posTarget );

        Wire* wire = new Wire( posSource, posTarget );
		addWire( linkInfo, wire );
	}
}


void ModuleBox::removeWires( UINT16 targetId )          // remove all wires, from inputs and outputs
{
    for( UINT16 i=0; i<data_->links_.size(); i++ )      // iterate over all links from and to this box
    {
		LinkInfo& linkInfo = data_->getLink( i );
		
		if( linkInfo.sourceId_ == targetId ) {         // remove wires connected to own inputs
			removeWire( linkInfo, IN_PORT );
        }

        if( data_->id_ == targetId )                    // remove wires connected to outputs of other box
        {
            ModulePort* outPort = owner_->getPort( linkInfo, OUT_PORT );
            outPort->disconnect( linkInfo );
        }
	}
}


void ModuleBox::removeWire( const LinkInfo& linkInfo, PortAlignment portAlign )
{
    if( portAlign == OUT_PORT )
    {
        ModulePort* outPort = getPort( linkInfo.outputId_, OUT_PORT );
        outPort->disconnect( linkInfo );
    }
    else {
        ModulePort* inPort = getPort( linkInfo.inputId_, IN_PORT );
        inPort->disconnect( linkInfo );
        deleteWire( linkInfo );
    }
}


void ModuleBox::positionWires( ModuleBox* movedBox )
{
	UINT16 targetId = movedBox->data_->id_;
	CPoint pos;
	
	for( UINT16 i=0; i<data_->links_.size() && isVisible(); i++ )
	{
		LinkInfo& linkInfo = data_->getLink( i );
		Wire* wire         = getWire( linkInfo );
		ASSERT( wire );

		if( movedBox != this ) {
			if( linkInfo.sourceId_ == targetId ) 
			{
				movedBox->getDockPosition( linkInfo.outputId_, OUT_PORT, pos );
				wire->front() = pos;
			}
		} 
		else {
            getDockPosition( linkInfo.inputId_, IN_PORT, pos );
			wire->back() = pos;

            if( linkInfo.sourceId_ == data_->id_ )     // link to self
            {
                getDockPosition( linkInfo.outputId_, OUT_PORT, pos );
			    wire->front() = pos;
            }
		}
	}
}


void ModuleBox::deleteWire( const LinkInfo& linkInfo )
{
    WireMap::iterator pos = wires_.find( linkInfo );
	if( pos != wires_.end() ) 
    {
        Wire* wire = pos->second;   
        wires_.erase( pos );
		delete wire;
	}
    else ASSERT( false );
}


void ModuleBox::addWire( const LinkInfo& linkInfo, Wire* wire )
{
    WireMap::iterator pos = wires_.find( linkInfo );
    ASSERT( pos == wires_.end() );
    ASSERT( wire );

    wires_.insert( WireMap::value_type( linkInfo, wire ));
}


void ModuleBox::hitTestWires( const CRect& rc, WireMap& map )
{
    for( WireMap::iterator it=wires_.begin(); it!=wires_.end() && isVisible(); it++ ) 
    {
        Wire* wire = it->second;
        if( wire->hitTest( rc )) 
        {
            map.insert( WireMap::value_type( it->first, it->second ));
        }
	}
}


Wire* ModuleBox::getWire( const LinkInfo& linkInfo )
{
    WireMap::iterator pos = wires_.find( linkInfo );
    return (pos != wires_.end()) ? pos->second : NULL;
}


void ModuleBox::drawWires( CDrawContext* pContext )
{
	for( WireMap::iterator it=wires_.begin(); it!=wires_.end() && isVisible(); it++ ) {
		it->second->draw( pContext );
	}
}


void ModuleBox::moveTo( CRect& r )
{
	setViewSize( r );
	setMouseableArea( r );
	data_->setPosition( (INT16)r.left, (INT16)r.top );
}


void ModuleBox::collapseOrExpand( bool collapse )
{
    if( collapse != data_->collapsed_ )
    {
        data_->collapsed_ = collapse;
        setPortVisibility();
        calculateSize();

        owner_->onBoxResized( this );
    }
}


bool ModuleBox::isCollapsed()  
{ 
    return data_->collapsed_;
}


void ModuleBox::focusAndSelect( bool focus, bool select )
{
    bool mustInvalid = selected_ != select || focused_ != focus;  

    if( selected_ != select ) {
        selected_   = select;
    }
	if( focused_ != focus ) {
        focused_ = focus;
    }
    if( mustInvalid ) setColors();
}


void ModuleBox::setColors()
{
	if( data_->catalog_ != MASTER ) 
    {
        colBkgnd_ = data_->polyphony_ == MONOPHONIC ? colors.moduleMono : colors.module;
        colFrame_ = selected_ ? colors.moduleFrame2 : colors.moduleFrame1;
        colFrame_ = focused_  ? colors.moduleFrame3 : colFrame_;
        colText_  = focused_  ? colors.moduleText2 : colors.moduleText1;
        colPort_  = colors.moduleFrame1;
    }
    else
    {
		colBkgnd_ = colors.master;
        colFrame_ = selected_ ? colors.masterFrame2 : colors.masterFrame1;
		colFrame_ = focused_ ? colors.masterFrame3 : colFrame_;
		colText_  = focused_ ? colors.masterText2 : colors.masterText1;
        colPort_  = colors.masterFrame1;
	} 
    if( label_ ) 
        label_->colText_ = colText_;

    setBackgroundColor( colBkgnd_ );
}


void ModuleBox::calculateSize()
{
	CRect rcSize     = size;
    ModuleInfo* info = ModuleInfo::getInfo( data_->catalog_ );

    if( data_->collapsed_ == false )
    {
	    int numPorts = max( info->inPorts_.size(), info->outPorts_.size() );
        int height   = portHeight_ + numPorts * portHeight_;
	    CPoint x     = CPoint( 100, height ); 
       	rcSize.setWidth( x.x );
    	rcSize.setHeight( x.y );
    }
    else {
        rcSize( rcSize.left, rcSize.top, rcSize.left + 100, rcSize.top + portHeight_ * 2 );
    }
	
	setViewSize( rcSize );
	setMouseableArea( rcSize );
}


void ModuleBox::setPortVisibility()
{
	for( CCView* pSv = pFirstView; pSv; pSv = pSv->pNext ) 
	{
		if( pSv->pView->isTypeOf( "ModulePort" )) {
            ModulePort* port = dynamic_cast< ModulePort* >( pSv->pView );
		    if( port ) {
                port->setVisible( data_->collapsed_ == false );
            }
		}
	}
}


ModulePort* ModuleBox::getPort( UINT16 portId, PortAlignment portAlign )
{
	for( CCView* pSv = pFirstView; pSv; pSv = pSv->pNext) 
	{
		if( pSv->pView->isTypeOf( "ModulePort" )) {
            ModulePort* port = dynamic_cast< ModulePort* >( pSv->pView );
		    if( port && port->info_->id_ == portId && port->align_ == portAlign )
		    {
			    return port;
		    }
        }
	}
    VERIFY( false );
	return NULL;
}


void ModuleBox::getDockPosition( UINT16 portId, PortAlignment portAlign, CPoint& pos )
{
    if( data_->collapsed_ == false ) 
    {
		ModulePort* port = getPort( portId, portAlign );
		port->getDockPosition( pos );
    }
    else 
    {
    	pos.y = size.top + size.getHeight() / 2;
	    pos.x = portAlign == IN_PORT ? size.left+1 : size.right-1;
	    
        CRect rcParent; 
        owner_->getVisibleSize( rcParent );
	    pos.offset( -rcParent.x, -rcParent.y );
    }
}


void ModuleBox::drawBackgroundRect( CDrawContext *pContext, CRect& updateRect )
{
	CViewContainer::drawBackgroundRect( pContext, updateRect );

	CRect rc (size);
	rc.offset (-rc.left, -rc.top);

	pContext->setLineWidth( 1 );
	pContext->setFillColor( colBkgnd_ );
	pContext->setFrameColor( colFrame_ );
	pContext->drawRect( rc, kDrawFilledAndStroked );

    if( data_ && data_->collapsed_ )    // draw port dummies in collapsed state
    {
        ModuleInfo* info = ModuleInfo::getInfo( data_->catalog_ );

        pContext->setFillColor( colPort_ );
        if( info && info->inPorts_.empty() == false )
        {
            CRect rcSquare    = CRect( rc.left+4, rc.top+9, rc.left+10, rc.top+15 );
            pContext->drawRect( rcSquare, kDrawFilled );
        }
        if( info && info->outPorts_.empty() == false )
        {
    		CRect rcSquare    = CRect( rc.right-10, rc.top+9, rc.right-4, rc.top+15 );
            pContext->drawRect( rcSquare, kDrawFilled );
        }
    }
    setDirty( false );
}
