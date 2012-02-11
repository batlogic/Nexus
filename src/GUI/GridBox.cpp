
#include "GridBox.h"
#include "Widgets.h"
#include "../Utils.h"


//--------------------------------------------------
// class SortInfo
//--------------------------------------------------

SortInfo::SortInfo() :
    columnId_( -1 ),
    type_( None )
    {}


void SortInfo::toggle( INT32 columnId )
{
    columnId_ = columnId;

    switch( type_ ) {
    case None:
    case Dn: type_ = Up; break;
    case Up: type_ = Dn; break;
    }
}



//--------------------------------------------------
// class GridBoxItem
//--------------------------------------------------

GridBoxItem::GridBoxItem() :
    CViewContainer( CRect( 0,0,0,0 ), NULL, NULL )
{
    setTransparency( true );
    setWantsFocus( true );
}


void GridBoxItem::init( GridBox* owner, UINT32 rowId )
{
    owner_       = owner;
    rowId_       = rowId;
    pParentFrame = owner_->getFrame();
}


void GridBoxItem::addControl( CControl* control )
{
    UINT32 numItems = getNbViews();
    control->setTag( numItems );
    addView( control );
}


void GridBoxItem::valueChanged( CControl* pControl )
{
    UINT32 column = (UINT32)pControl->getTag();
    owner_->valueChanged( rowId_, column );
}


void GridBoxItem::layout( const CRect& rcSize )
{
	string name, category, comment;
    getText( 0, name );
    getText( 1, category );
    getText( 2, comment );

    size = rcSize;
    setMouseableArea( rcSize );

    const GridBoxColumnList& columns = owner_->getColumns();
    for( UINT32 i=0; i<columns.size(); i++ )
    {
        CView* view = getView( i );
        if( view ) 
        {
            const ColumnInfo& info = columns.at( i );
            CRect rc( info.left_+2, 0, info.right_-1, size.height() );
            view->setViewSize( rc, false );
            view->setMouseableArea( rc );
        }
	}
}


void GridBoxItem::setState( bool selected, bool focused )
{
    const GridBoxColumnList& columns = owner_->getColumns();
    for( UINT32 i=0; i<columns.size(); i++ )
    {
        CView* view = getView( i );
        if( view ) {
            selected ? view->setAttribute( ATTR_SELECTED, 0, NULL ) : view->removeAttribute( ATTR_SELECTED );
            focused  ? view->setAttribute( ATTR_FOCUSED, 0, NULL )  : view->removeAttribute( ATTR_FOCUSED );
        }
	}
}


void GridBoxItem::drawRect( CDrawContext* pContext, const CRect& rcUpdate )
{
    if( isVisible() == false )
        return;

    CViewContainer::drawRect( pContext, rcUpdate );
}



//---------------------------------------------------
// class GridBoxHeader
//---------------------------------------------------

GridBoxHeader::GridBoxHeader( CCoord height ) :
    CView( CRect( 0,0,0,0 )),
    colBkgnd_( colTransparent ),
    colFrame_( colors.fill2 ),
    colText_( colors.text2 ),
    hAlign_( kCenterText ),
    tracker_( -1 ),
    columns_( NULL )
{
    size( 0, 0, 0, height );
}


void GridBoxHeader::init( const CRect& rcSize, GridBox* owner )
{
    owner_   = owner;
    columns_ = &owner_->columns_;
    size     = rcSize;
    setMouseableArea( size );
}


CMouseEventResult GridBoxHeader::onMouseExited( CPoint& pos, const long& buttons )
{
	tracker_ = -1;
    setCursor( CursorDefault );
    return kMouseEventHandled;
}


CMouseEventResult GridBoxHeader::onMouseDown( CPoint& pos, const long& buttons )
{
    tracker_ = getTracker( pos );
    tracker_ > -1 ? setCursor( CursorHSize ) : setCursor( CursorDefault );

    if( buttons & kDoubleClick ) 
    {
        INT32 columnId = getColumnId( pos );
        owner_->sortInfo_.toggle( columnId );
        owner_->sort();
        invalid();
    }
    return onMouseMoved( pos, buttons );
}


CMouseEventResult GridBoxHeader::onMouseUp( CPoint& pos, const long& buttons )
{
    tracker_ = -1;
    setCursor( CursorDefault );
    return kMouseEventHandled;
}


CMouseEventResult GridBoxHeader::onMouseMoved( CPoint& pos, const long& buttons )
{
    if( tracker_ < 0 )
    {
        INT32 tracker = getTracker( pos );
        tracker > -1 ? setCursor( CursorHSize ) : setCursor( CursorDefault );
    }
    else if( tracker_ < (INT32)columns_->size() - 1 )
    {
        ColumnInfo& info = columns_->at( tracker_ );
        ColumnInfo& next = columns_->at( tracker_ + 1 );
        CCoord minPos    = info.left_ + 30;
        CCoord maxPos    = min( size.right - size.left - 30, columns_->at( tracker_ + 1 ).right_ - 30 );
        CCoord newPos    = max( minPos, min( maxPos, pos.x - size.left ));

        info.right_      = newPos;
        info.ratio_      = (info.right_-info.left_) / (float)size.width();
        next.left_       = newPos;
        next.ratio_      = (next.right_-next.left_) / (float)size.width();

        owner_->layout();
        invalid();
    }
    return kMouseEventHandled;
}


INT32 GridBoxHeader::getTracker( const CPoint& pos )
{
    for( UINT32 i=0; i<columns_->size()-1; i++ )
    {
        CCoord right = size.left + columns_->at( i ).right_;
        CRect rc     = CRect( right-2, size.top, right+2, size.bottom );

        if( rc.pointInside( pos )) {
            return i;
        }
    }
    return -1;
}


INT32 GridBoxHeader::getColumnId( const CPoint& pos )
{
    for( UINT32 i=0; i<columns_->size(); i++ )
    {
        CCoord left  = size.left + columns_->at( i ).left_;
        CCoord right = size.left + columns_->at( i ).right_;
        CRect rc     = CRect( left, size.top, right, size.bottom );

        if( rc.pointInside( pos )) {
            return i;
        }
    }
    ASSERT( false );
    return -1;
}


void GridBoxHeader::setCursor( CursorType type )
{
    ChildFrame* frame = dynamic_cast< ChildFrame* >( owner_->getFrame() );
    if( frame ) 
        frame->setCursor( type );
}


void GridBoxHeader::draw( CDrawContext* pContext )
{
    pContext->setFillColor( colBkgnd_ );
    pContext->setFontColor( colText_ );
    pContext->setFrameColor( colFrame_ );
    pContext->setLineWidth( 1 );
    pContext->setFont( font_ );
    pContext->drawRect( size, kDrawFilled );

    const SortInfo& sortInfo  = owner_->sortInfo_;

    for( UINT32 i=0; i<columns_->size(); i++ )
    {
        const ColumnInfo& info = columns_->at( i );
        CCoord left            = size.left + info.left_;
        CCoord right           = min( size.right, size.left+info.right_);
        CRect rcText           = CRect( left+3, size.top, right-3, size.bottom );
        
        if( i<columns_->size()-1 )      // borders between columns
        {
            pContext->moveTo( CPoint( right, size.top ));
            pContext->lineTo( CPoint( right, size.bottom ));
        }
        
        if( sortInfo.columnId_ == i && sortInfo.type_ != SortInfo::None )   // sorting glyph
        {
            CCoord top = size.height() / 2;
            CRect rcGlyph( right-10, size.top+top, right-5, size.top+top+3 );
            DrawHelper::drawTriangle( pContext, rcGlyph, colText_, sortInfo.type_ == SortInfo::Up );
        }
        pContext->drawStringUTF8( info.caption_.c_str(), rcText, hAlign_, true );
	}
    setDirty( false );
}



//---------------------------------------------------
// class GridBox
//---------------------------------------------------

GridBox::GridBox( const CRect& rcSize, CFrame* frame, GridBoxListener* listener ) :
    CViewContainer( rcSize, frame ),
    listener_( listener ),
    keyListener_( NULL ),
    scrollbar_( NULL ),
    header_( NULL ),
    itemHeight_( 15 ),
    scrollbarWidth_( 15 ),
    scrollOffset_( 0 ),
    numItems_( 0 ),
    index_( 0 ),
    selStyle_( NoSel )
{
    setWantsFocus( true );
    setTransparency( true );

    rcItems_( size.left, size.top, size.right-scrollbarWidth_, size.bottom );
    rcScrollbar_( rcItems_.right-1, rcItems_.top, rcItems_.right+scrollbarWidth_, rcItems_.bottom );

    scrollbar_ = new CScrollbar( rcScrollbar_, this, 0, CScrollbar::kVertical, CRect( 0,0,0,0 ));
    scrollbar_->setVisible( false );
    scrollbar_->setWantsFocus( false );

    setColors( colors.fill1, colors.fill2, colors.selection, colors.text2 );
}


GridBox::~GridBox()
{
    if( scrollbar_ ) delete scrollbar_;
}


void GridBox::addToContainer( CViewContainer* parent, GridBoxHeader* header )
{
    header_       = header;
    CCoord offset = header_ ? header_->getHeight() : 0;
    size.top     += offset;

    rcHeader_( size.left, size.top-offset, size.right-scrollbarWidth_, size.top );
    rcItems_( size.left, size.top, size.right-scrollbarWidth_, size.bottom );
    rcScrollbar_ ( size.right-scrollbarWidth_-1, size.top, size.right, size.bottom );

    maxVisible_ = rcItems_.height() / itemHeight_;

    parent->addView( this );
    
    if( header_ ) 
    {
        header_->init( rcHeader_, this  );
        parent->addView( header );
    }
    scrollbar_->setVisible( false );
    scrollbar_->setViewSize( rcScrollbar_, false );
}


void GridBox::addColumn( INT16 percent, const string& caption )
{
    float ratio = percent / 100.f;
    CCoord x    = columns_.empty() ? 0 : columns_.back().right_;
    CCoord w    = (CCoord)(rcItems_.width() * ratio);
    
    ColumnInfo info( x, x + w, ratio, caption );
    columns_.push_back( info );
}


void GridBox::setColumnMetrics( const string& metrics )
{
    vector< string > tokens;
    Utils::tokenize( metrics, ",", tokens );

    for( UINT32 i=0; i<tokens.size() && i<columns_.size(); i++ ) {
        columns_[i].ratio_ = Utils::str2int( tokens[ i ]) / 100.f;
    }
    resizeColumns( rcItems_ );
    layout();
}


void GridBox::getColumnMetrics( string& metrics )
{
    ostringstream os;

    for( UINT32 i=0; i<columns_.size(); i++ )
    {
        os << (INT16)(columns_[ i ].ratio_ * 100);
        if( i < columns_.size()-1 ) 
            os << ",";
    }
    metrics = os.str();
}


void GridBox::resizeColumns( const CRect& rcSize )
{
    CCoord left = 0;
    CCoord gridWidth = rcSize.width();

    for( UINT32 i=0; i<columns_.size(); i++ )
    {
        ColumnInfo& info = columns_.at( i );
        CCoord width     = (CCoord)(gridWidth * info.ratio_);
        CCoord right     = max( 30, left + width );
        info.left_       = left;
        info.right_      = right;
        left             = right;
    }
}


void GridBox::addItem( GridBoxItem* item )
{
    addView( item );

    numItems_  = getNbViews();
    CCoord top = numItems_ * itemHeight_;

    CRect rcItem( 0, top, rcItems_.width(), top+itemHeight_ );
    item->setViewSize( rcItem, false );
    item->init( this, numItems_-1 );

    checkScrollbar();
}


void GridBox::clear( bool preserveSelection )
{
    removeFromSelection( index_ );
    index_    = 0;
    numItems_ = 0;
    if( preserveSelection == false ) 
        selection_.clear();

    scrollbar_->setVisible( false );
    rcScrollsize_( 0,0,0,0 );

    removeAll( true );
}


void GridBox::sort()
{
    INT32 logicalIndex = getLogicalIndex( index_ );                   // preserve index

    getSelection();
    selection_.clear();
    
    typedef vector< GridBoxItem* > ItemList;
    ItemList list;

    for( CCView* pSv = pFirstView; pSv; pSv = pSv->pNext )          // push item to temporary list
	{
		GridBoxItem* item = dynamic_cast< GridBoxItem* >( pSv->pView );
        list.push_back( item );
	}

    stable_sort( list.begin(), list.end(), compare );                       // sort temporary list

	UINT32 i = 0;
    for( CCView* pSv = pFirstView; pSv; pSv = pSv->pNext )                  // reassign sorted items to the views
	{
		pSv->pView = list[ i++ ];
	}

    for( SelectionSet::iterator it=logicalSelection_.begin(); it!=logicalSelection_.end(); it++ )   // restore selection
    {
        INT32 index = getPhysicalIndex( *it );
        selection_.insert( index );
    }
    index_ = getPhysicalIndex( logicalIndex );                                  // restore index
    layout();
}


void GridBox::setSortInfo( INT32 columnId, INT32 type )
{
    sortInfo_.columnId_ = columnId;
    sortInfo_.type_     = (SortInfo::Type)type;
}


void GridBox::valueChanged( INT32 row, INT32 col )
{
    if( listener_ ) {
		INT32 idxLogical = getLogicalIndex( row );
        listener_->valueChanged( idxLogical, col );
    }
    getFrame()->setFocusView( this );
}


void GridBox::valueChanged( CControl* pControl )
{
    if( scrollbar_->isVisible() )
	{
		float value   = pControl->getValue();
        UINT32 offset = (UINT32)((rcScrollsize_.height() - rcItems_.height()) * value);
        scrollOffset_ = (UINT32)(offset / (itemHeight_-0.5f));
        scrollOffset_ = min( numItems_ - maxVisible_, scrollOffset_ );
	}
    layout();
}


UINT32 GridBox::scrollTo( INT32 index, bool toTop )
{
    if( selStyle_ == NoSel ) 
    {
        INT32 oldIndex  = index_;
        index_          = max( 0, min( numItems_ - maxVisible_, index ));
        scrollOffset_  += max( 0, index_ - oldIndex );
    }
    else {
        INT32 newIndex = toTop ? max( 0, index + maxVisible_ - 1 ) : index;
        newIndex      = max( 0, min( numItems_- 1, newIndex ));

        if( newIndex >= maxVisible_ + scrollOffset_ ) 
            scrollOffset_ = (newIndex - maxVisible_) + 1;
        if( newIndex < scrollOffset_ ) 
            scrollOffset_ = newIndex;

        index_ = max( 0, min( numItems_- 1, index ));
    }

    int offset  = scrollOffset_ * itemHeight_;
    float value = offset / (float)(rcScrollsize_.height() - rcItems_.height());
    scrollbar_->setValue( value );

    layout();
    return index_;
}


void GridBox::setItemStates()
{
	if( selStyle_ > NoSel )
    {
        for( INT32 i=0; i<numItems_; i++ )
	    {
            GridBoxItem* item = getItem( i );
            item->setState( isSelected( i ), i == index_ );
	    }
        invalid();
    }
}


void GridBox::layout()
{
    setItemStates();

    CCoord top = (itemHeight_ * scrollOffset_) * -1;
	for( INT32 i=0; i<numItems_; i++ )
	{
        GridBoxItem* item = getItem( i );
        CRect rc( 0, top+2, rcItems_.width(), top+itemHeight_-1 );
        item->layout( rc );
        item->setVisible( top < rcItems_.height() );
        top += itemHeight_;
	}
    invalid();
}


void GridBox::setColors( const CColor& colBkgnd, const CColor colFrame, const CColor& colSel, const CColor& colFocus )
{
    colBkgnd_ = colBkgnd;
    scrollbar_->setBackgroundColor( colBkgnd_ );

    colSel_   = colSel;
    colFocus_ = colFocus;

    colFrame_ = colFrame;
    scrollbar_->setFrameColor( colFrame_ );
    scrollbar_->setScrollerColor( colFrame_ );

    for( CCView* pSv = pFirstView; pSv; pSv = pSv->pNext )     
	{
		GridBoxItem* item = dynamic_cast< GridBoxItem* >( pSv->pView );
        item->setColors();
	}
}


//void GridBox::setBackgroundColor( const CColor& col )
//{
//    colBkgnd_ = col;
//    scrollbar_->setBackgroundColor( col );
//}
//
//
//void GridBox::setFrameColor( const CColor& col )
//{
//    colFrame_ = col;
//    scrollbar_->setFrameColor( col );
//    scrollbar_->setScrollerColor( col );
//}


void GridBox::setSelectionStyle( SelStyle selStyle )
{
    selStyle_ = selStyle;
}


void GridBox::setItemHeight( CCoord height ) 
{ 
    itemHeight_ = height;     
    maxVisible_ = rcItems_.height() / itemHeight_;
}


void GridBox::setViewSize( CRect& rcSize, bool invalid )
{
    CRect rc      = rcSize;
    size          = rc;
    mouseableArea = rc;
    
    rcItems_( size.left, size.top, size.right-scrollbarWidth_, size.bottom );
    CCoord height   = (CCoord)((rcItems_.height() / itemHeight_) * itemHeight_);
    rcItems_.bottom = rcItems_.top + height;
    
    maxVisible_ = min( numItems_, rcItems_.height() / itemHeight_ );

    rcScrollbar_( rcItems_.right-1, rcItems_.top, rcItems_.right+scrollbarWidth_, rcItems_.bottom );
    scrollbar_->setViewSize( rcScrollbar_, invalid );
    checkScrollbar();
    
    if( header_ ) {
        rcHeader_( rcItems_.left, rcItems_.top-header_->getHeight(), rcItems_.right, rcItems_.top );
        header_->setViewSize( rcHeader_, invalid );
    }
    resizeColumns( rcItems_ );
    layout();
}


void GridBox::checkScrollbar()
{
    if( numItems_ * itemHeight_ > (INT32)rcItems_.height() ) 
    {
        rcScrollsize_( 0, 0, 0, numItems_*itemHeight_ );
        scrollbar_->setVisible( true );
        scrollbar_->setScrollSize( rcScrollsize_ );
        scrollbar_->setWheelInc( 1 / (float)numItems_ );
    }
    else {
        scrollbar_->setVisible( false );
        scrollOffset_ = 0;
    }
}


INT32 GridBox::calcIndexFromPosition( const CPoint& pos )
{
    CPoint p( pos );
    p.offset( -size.left, -size.top );

    return (INT32)( max( 0, p.y ) / itemHeight_ ) + scrollOffset_;
}


bool GridBox::isInLowerHalfOfRow( const CPoint& pos )
{
    CPoint p( pos );
    p.offset( -size.left, -size.top );

    return (p.y % itemHeight_) > itemHeight_ / 2;
}


INT32 GridBox::getPhysicalIndex( INT32 rowId )
{
    int idx = 0;
    for( CCView* pSv = pFirstView; pSv; pSv = pSv->pNext )     
	{
		GridBoxItem* item = dynamic_cast< GridBoxItem* >( pSv->pView );
        if( item->rowId_ == rowId ) {
            return idx;
        }
        idx++;
	}
    return -1;
}


INT32 GridBox::getLogicalIndex( INT32 index )
{
    GridBoxItem* item = getItem( index );
    if( item ) {
        return item->rowId_;
    }
    return -1;
}


INT32 GridBox::getLogicalIndex()
{
    return getLogicalIndex( index_ );
}


INT32 GridBox::getLogicalIndex( const CPoint& pos )
{
    UINT32 index = calcIndexFromPosition( pos );
    return getLogicalIndex( index );
}


void GridBox::select( INT32 index, bool ctrl, bool shift )
{
    index = max( 0, min( numItems_-1, index ));
    
    if( selStyle_ == SingleSel ) 
    {
        selection_.clear();
        selection_.insert( index );
        index_ = index;
    }
    else if (selStyle_ == MultipleSel ) 
    {
        if( shift == false && ctrl == false ) {
            selection_.clear();
            selection_.insert( index );
            index_ = index;
        }
        else if( ctrl ) {
            toggleSelection( index );
        }
        else if( shift )
        {
            UINT32 minSel = min( index, index_ );
            UINT32 maxSel = max( index, index_ );

            for( UINT32 i=minSel; i<=maxSel; i++ ) {
                toggleSelection( i );
            }
        }
    }
    setItemStates();
}


void GridBox::toggleSelection( INT32 index )
{
    SelectionSet::iterator pos = selection_.find( index );

    if( pos != selection_.end() && index != index_ ) {
        selection_.erase( pos );
    } else {
        selection_.insert( index );
    }
}


void GridBox::removeFromSelection( INT32 index )
{
    SelectionSet::iterator pos = selection_.find( index );

    if( pos != selection_.end() ) 
        selection_.erase( pos );
}


void GridBox::selectAll()
{
    for( INT32 i=0; i<=numItems_; i++ ) {
        selection_.insert( i );
    }
    invalid();
}


void GridBox::clearSelection()
{
    selection_.clear();
    selection_.insert( index_ );
    invalid();
}


const SelectionSet& GridBox::getSelection()      
{ 
    logicalSelection_.clear();
    for( SelectionSet::iterator it=selection_.begin(); it!=selection_.end(); it++ )
    {
        INT32 index = getLogicalIndex( *it );
        logicalSelection_.insert( index );
    }
    return logicalSelection_;
}


void GridBox::setIndex( INT32 index, bool scroll )
{
    ASSERT( index >= 0 && index < numItems_ );

    removeFromSelection( index_ );
    selection_.insert( index );
    index_ = index;
    setItemStates();
    if( scroll )
        scrollTo( index_, false );
}


bool GridBox::isSelected( INT32 index )
{
    SelectionSet::iterator pos = selection_.find( index );
    return pos != selection_.end();
}


GridBoxItem* GridBox::getItem( UINT32 rowId )
{
    return dynamic_cast< GridBoxItem* >( getView( rowId ));
}


CMouseEventResult GridBox::onMouseDown( CPoint& pos, const long& buttons )
{
    if( buttons & kLButton ) 
    {
        getFrame()->setFocusView( this ); 

        if( rcScrollbar_.pointInside( pos ))   // click in scrollbar
        {
            scrollbar_->onMouseDown( pos, buttons );
            return kMouseEventHandled;
        }
        else if( rcItems_.pointInside( pos ) && selStyle_ > NoSel )
        {
            INT32 index = calcIndexFromPosition( pos );
            if( index < numItems_ )
            {
                index = max( 0, min( numItems_- 1, index ));

                INT32 oldIndex = index_;
                select( index, (buttons & kControl) != 0, (buttons & kShift) != 0 );

                if( oldIndex != index_ && selStyle_ > NoSel ) 
                {
                    valueChanged( index_, -1 );
                    layout();
                }
            }
        }
    }
    return CViewContainer::onMouseDown( pos, buttons );
}


CMouseEventResult GridBox::onMouseMoved( CPoint& pos, const long& buttons )
{
    if( buttons && kLButton ) 
    {
	    if( rcScrollbar_.pointInside( pos ) && scrollbar_->isVisible() )   // moving the scrollbar handle
	    {
	        return scrollbar_->onMouseMoved( pos, buttons );
	    }
	}
    return kMouseEventNotHandled;
}


bool GridBox::onWheel( const CPoint& pos, const CMouseWheelAxis& axis, const float& distance, const long& buttons )
{
    if( axis == kMouseWheelAxisY ) {
        return scrollbar_->onWheel( pos, axis, distance, buttons );
    }
	return false;
}


long GridBox::onKeyDown( VstKeyCode& keyCode )
{
    if( keyListener_ ) {
        long result = keyListener_->onKeyDown( keyCode );
        if( result != 0 ) 
            return result;
    }
    
    if( keyCode.virt == 0 && keyCode.modifier == MODIFIER_CONTROL )
    {
        switch( keyCode.character ) 
        {
        case 97:  selectAll();      break;   // Ctrl-a
        case 100: clearSelection(); break;   // Ctrl-d
        default: return 0;
        }
        return 1;
    }
    else if( keyCode.virt )
    {
        INT32 index = -INT_MAX; 
        
        switch( keyCode.virt ) 
        {
        case VKEY_DOWN:     index = min( numItems_, index_+1 ); break;
        case VKEY_UP:       index = max( 0, index_ - 1 );       break;
        case VKEY_PAGEDOWN: index = index_ + maxVisible_;       break;
        case VKEY_PAGEUP:   index = index_ - maxVisible_;       break;
        case VKEY_HOME:     index = 0;                          break;
        case VKEY_END:      index = numItems_;                  break;
        default: return 0;
        }

        ASSERT( index >= 0 );
        if( index > -INT_MAX )
        {
            select( index, false, (keyCode.modifier & MODIFIER_SHIFT) != 0 );
            scrollTo( index, false );
            
            if( selStyle_ > NoSel ) {
                valueChanged( index_, -1 );
            }
            return 1;
        }
    }
    return 0;
}


void GridBox::drawRect( CDrawContext* pContext, const CRect& updateRect )
{
    pContext->setFillColor( colBkgnd_ );
    pContext->setFrameColor( colFrame_ );
    pContext->setLineWidth( 1 );
    pContext->drawRect( rcItems_, kDrawFilledAndStroked );

    if( selStyle_ > NoSel )         // draw selection indicator
    {
        pContext->setFillColor( colSel_ );
        for( SelectionSet::iterator it=selection_.begin(); it!=selection_.end(); it++ )
        {
            UINT32 index = *it;

            CCoord top = rcItems_.top + (index - scrollOffset_) * itemHeight_;
            CRect rcSel( rcItems_.left+2, top+2, rcItems_.right-2, top+itemHeight_-1 );
            if( rcSel.bottom <= rcItems_.bottom )
            {
                pContext->drawRect( rcSel, kDrawFilled );
            }
        }
    }

    CViewContainer::drawRect( pContext, updateRect );

    CCoord top = size.top;
    pContext->setFrameColor( colFrame_ );
    for( INT32 i=scrollOffset_; i<numItems_; i++ )                      // row lines
    {
		top += itemHeight_;
        if( top+itemHeight_ <= rcItems_.bottom )
        {
            pContext->moveTo( CPoint( rcItems_.left, top ) );
		    pContext->lineTo( CPoint( rcItems_.right-1, top ) );
        }
    }

    for( UINT32 i=0; i<columns_.size()-1; i++ )                         // column lines
    {
        CCoord x = rcItems_.left + columns_.at( i ).right_;
        CCoord y = min( rcItems_.height()-1, itemHeight_*numItems_ );
        pContext->moveTo( CPoint( x, rcItems_.top ) );
		pContext->lineTo( CPoint( x, rcItems_.top+y ));
    }

    if( selStyle_ == MultipleSel )                                      // draw focus indicator
    {
        CCoord top = rcItems_.top + (index_ - scrollOffset_) * itemHeight_;
        CRect rcSel( rcItems_.left+4, top+3, rcItems_.right-4, top+itemHeight_-2 );
        if( rcSel.bottom <= rcItems_.bottom )
        {
            CLineStyle oldLineStyle = pContext->getLineStyle();
            pContext->setFillColor( colTransparent );
            pContext->setFrameColor( colFocus_ );
            pContext->setLineWidth( 1 );
            pContext->setLineStyle( kLineOnOffDash );

            pContext->drawRect( rcSel, kDrawFilledAndStroked );
            pContext->setLineStyle( oldLineStyle );
        }
    }

    if( scrollbar_->isVisible() ) {
        scrollbar_->draw( pContext );
    }
    setDirty( false );
}


