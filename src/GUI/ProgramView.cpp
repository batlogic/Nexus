
#include "ProgramView.h"
#include "Wire.h"
#include "ModuleBox.h"
#include "ModulePort.h"
#include "PrefsDialog.h"
#include "CtrlView.h"
#include "../Synth.h"
#include "../Program.h"
#include "../ModuleInfo.h"



ProgramView::ProgramView( const CRect& rcSize, Editor* editor, CtrlView* ctrlView ) : 
    CViewContainer( rcSize, editor->getChildFrame(), NULL ),
	editor_( editor ),
	synth_( editor->getSynth() ),
	ctrlView_( ctrlView )
{
    setMouseableArea( rcSize );
    setWantsFocus( true );
    setAutosizeFlags( kAutosizeAll );
    rcPreferred_ = rcSize;

    long style  = CScrollView::kHorizontalScrollbar|CScrollView::kVerticalScrollbar|CScrollView::kDontDrawFrame;
    scrollView_ = new ScrollView( rcSize, rcSize, editor_->getChildFrame(), style );
    scrollView_->enableScrollbars( false, false );
    scrollView_->setAutosizeFlags( kAutosizeAll );
    scrollView_->addView( this );

    CScrollbar* vsb = scrollView_->getVerticalScrollbar();
    vsb->setFrameColor( colTransparent );
    vsb->setWantsFocus( false );

    CScrollbar* hsb = scrollView_->getHorizontalScrollbar();    
    hsb->setFrameColor( colTransparent );
    hsb->setWantsFocus( false );

    setColors();
}


void ProgramView::setColors()
{
    setBackgroundColor( colors.fill1 );
    scrollView_->setBackgroundColor( colors.fill1 );
    selection_.rubberband_.colLine_ = colors.fill2;

    CScrollbar* vsb = scrollView_->getVerticalScrollbar();
    vsb->setBackgroundColor( colors.fill1 );
    vsb->setScrollerColor( colors.fill2 );

    CScrollbar* hsb = scrollView_->getHorizontalScrollbar();    
    hsb->setBackgroundColor( colors.fill1 );
    hsb->setScrollerColor( colors.fill2 );

    for( UINT32 i=0; i<boxes_.size(); i++ ) {
        boxes_[i]->setColors();
    }
    focusBox( NULL, false );
}


bool ProgramView::attached( CView* parent )
{
    bool result = CViewContainer::attached( parent );

    setProgram();
    getFrame()->setFocusView( this );
    return result;
}


long ProgramView::onKeyDown( VstKeyCode& keyCode ) 
{
    //TRACE( "virt=%d character=%d modifier=%d\n", keyCode.virt, keyCode.character, keyCode.modifier );
    try {
        switch( keyCode.virt ) 
        {
        case VKEY_DELETE: deleteSelection(); return 1;
        case VKEY_TAB: 
        {
            bool next = ( keyCode.modifier == MODIFIER_SHIFT ) ? false : true;
            advanceFocus( next ); 
            return 1;
        }
        case VKEY_UP:    trackSelection( North, keyCode.modifier == MODIFIER_CONTROL ); return 1;
        case VKEY_RIGHT: trackSelection( East,  keyCode.modifier == MODIFIER_CONTROL ); return 1;
        case VKEY_DOWN:  trackSelection( South, keyCode.modifier == MODIFIER_CONTROL ); return 1;
        case VKEY_LEFT:  trackSelection( West,  keyCode.modifier == MODIFIER_CONTROL ); return 1;
        }

        if( keyCode.virt == 0 && keyCode.modifier == MODIFIER_CONTROL )
        {
            switch( keyCode.character )
            {
            case 97:  selectAll();                                 break; // Ctrl-a
            case 100: unselectAll();                               break; // Ctrl-d
            case 99:  clipboardCopy();                             break; // Ctrl-c
            case 118: clipboardPaste( selection_.lastClickPos_ );  break; // Ctrl-v
            case 120: clipboardCut();                              break; // Ctrl-x
            case 43:   
            case 45:  collapseOrExpand( keyCode.character == 45 ); break; // Ctrl-+: expand, Ctrl--: collapse
            }
            return 1;
        }
    }
    catch( const exception& e ) {
        TRACE( e.what() );
        editor_->setAppState( Editor::NotReady );
    }
    return -1;
}


CMouseEventResult ProgramView::onMouseDown( CPoint& pos, const long& buttons )
{
    if( getFrame() ) 
        getFrame()->setFocusView( this );
	
    CViewContainer::onMouseDown( pos, buttons );

    ModuleBox* box = hitTestBox( pos );

    if( buttons & kLButton ) 
    {
        beginWiring( pos, buttons );
        beginSelecting( pos, box, buttons );
        return kMouseEventHandled;
    }
    else if( buttons & kRButton ) 
    {	
        beginSelecting( pos, box, buttons );
        handleMenu( pos, box );
    }
    return kMouseEventNotHandled;
}


CMouseEventResult ProgramView::onMouseUp( CPoint& pos, const long& buttons )
{
    endWiring( pos );
    endSelecting();

	return kMouseEventHandled;
}


CMouseEventResult ProgramView::onMouseMoved( CPoint& pos, const long& buttons )
{
    if( buttons & kLButton ) 
    {
		if( selection_.isTracking_ ) {
            trackSelection( pos );
        }
        continueWiring( pos );
        continueSelecting( pos );

        return kMouseEventHandled;
	}
	return kMouseEventNotHandled;
}


void ProgramView::beginWiring( const CPoint& pos, long buttons )
{
    wireData_.cleanup();

    if( (buttons & kLButton) == 0 ) {
        return;
    }
    ModulePort* port = hitTestPort( pos );
    if( port == NULL ) {
        return;
    }
    ASSERT( port->info_ );
    CPoint posPort( pos );
    port->startDocking( posPort );

    if( port->align_ == OUT_PORT)                                       // create new wire
	{
		wireData_.isWiring_ = true;
        wireData_.wire_     = new Wire( posPort, posPort );
		wireData_.source_   = port;
        wireData_.linkInfo_ = LinkInfo();
        invalid();
	}
    else if( port->align_ == IN_PORT && port->links_.size() > 0 )       // rewire existing link
    {
        bool found = false;
        LinkInfo linkInfo;
        if( port->links_.size() == 1 ) {                                // only one link on port
            linkInfo = port->links_.front();  
            found    = true;
        }
        else for( UINT16 i=0; i<port->links_.size(); i++ )              // links>1: search for first selected wire
        {
            Wire* wire = getWire( port->links_.at( i ), IN_PORT );
            if( wire && wire->selected_ ) {
                linkInfo = port->links_.at( i );
                found    = true;
                break;
            }
        }
        if( found )                                                     // create new wire
        {
            ModuleBox* sourceBox = getBox( linkInfo.sourceId_ );
            ModuleBox* targetBox = getBox( linkInfo.targetId_ );
            wireData_.isWiring_  = true;
            wireData_.linkInfo_  = linkInfo;
            wireData_.target_    = port;
            wireData_.source_    = sourceBox->getPort( linkInfo.outputId_, OUT_PORT );
            wireData_.wire_      = new Wire( targetBox->getWire( linkInfo ));

            synth_->removeLink( (LinkData)linkInfo );
            targetBox->removeWire( linkInfo, IN_PORT );
            selection_.wires_.clear();
        }
    }
}


void ProgramView::continueWiring( const CPoint& pos  )
{
	if( wireData_.isWiring_ ) 
    {
        CPoint posPort( pos );

        ModuleBox* box = hitTestBox( posPort );
        if( box && box->isCollapsed() == true ) 
        {
            box->collapseOrExpand( false );
            wireData_.autoExpanded_.push_back( box );
        }

        ModulePort* port = hitTestPort( posPort );
		if( port != NULL )					                          // hit any target port
        {
            port->startDocking( posPort );
    		wireData_.wire_->back() = posPort;

            if( port != wireData_.source_ ) {                         // hit another port than previous
			    if( wireData_.target_ && port != wireData_.target_ )  // undock previous port
			    {
				    wireData_.target_->endDocking( posPort );
				    wireData_.target_ = NULL;
			    }
			    port->startDocking( posPort );						  // start docking to new port
			    wireData_.target_ = port;
            }
		}
        else {
            if( wireData_.target_ ) {                                 // left a port, but hit no other port                                       
                wireData_.target_->endDocking( posPort );
                wireData_.target_ = NULL;
            }
            else wireData_.wire_->back() = posPort;
        }

        if( wireData_.lastPos_ != pos ) {
            invalid();
            wireData_.lastPos_ = pos;
        }
	}
}


void ProgramView::endWiring( const CPoint& pos )
{
	if( wireData_.isWiring_ ) 
    {
		CPoint posPort( pos );
        ModulePort* targetPort = hitTestPort( posPort );

        if( targetPort == NULL || (targetPort && targetPort->align_ == OUT_PORT ))		// mouse released, but not over port
        {
            wireData_.source_->endDocking( posPort );
            wireData_.source_->disconnect( wireData_.linkInfo_ );
			delete wireData_.wire_;
            ModuleData* data = selection_.focus_ ? selection_.focus_->data_ : NULL;
            ctrlView_->updateModule( data );
		}
		else       // mouse released over port
		{	
            targetPort->startDocking( posPort );

            wireData_.target_    = targetPort;
			ModuleBox* sourceBox = wireData_.source_->owner_;
			ModuleBox* targetBox = wireData_.target_->owner_;

            LinkData linkData;
            ModuleInfo::getLinkInfo( targetBox->data_->catalog_, linkData.inputId_, &linkData );

			linkData.outputId_ = wireData_.source_->info_->id_;
			linkData.inputId_  = wireData_.target_->info_->id_; 
            linkData.sourceId_ = sourceBox->data_->id_;
			linkData.targetId_ = targetBox->data_->id_;

            ostringstream os;
            os << sourceBox->data_->label_<<"["<<sourceBox->data_->id_<<"]"<<"->"<<linkData.label_;
            linkData.label_ = os.str();

			bool result = synth_->addLink( targetBox->data_, linkData );

			if( result )    // is link valid?
			{
				wireData_.target_->connect( posPort, linkData );
				wireData_.wire_->back() = posPort;
				wireData_.source_->connect( posPort, linkData );
                wireData_.removeAutoExpanded( targetBox );

                targetBox->addWire( linkData, wireData_.wire_ );
                ModuleData* data = selection_.focus_ ? selection_.focus_->data_ : NULL;
                ctrlView_->updateModule( data );
                selectBox( sourceBox, false );
                selectBox( targetBox, true );
                focusBox( targetBox, true );
			}
			else {             
                wireData_.source_->endDocking( posPort );
                targetPort->endDocking( posPort );
                wireData_.cleanup();
            }
		}
		wireData_.reset();
        invalid();
	}
}


void ProgramView::beginSelecting( const CPoint& pos, ModuleBox* box, const long& buttons )
{
    selection_.lastClickPos_ = pos;
    
    if( box == NULL ) {
        if( buttons & kLButton )
        {
            if( (buttons & kControl) == 0 ) {
                unselectAll();
                focusBox( selection_.focus_, false );
            }
            CPoint offset           = scrollView_->getScrollOffset();
            selection_.isSelecting_ = true;
            selection_.rubberband_.begin( pos, offset );
            hitTestWires( CRect( pos.x, pos.y, pos.x, pos.y ));
        }
    }
    else 
    {
        if( (buttons & kControl) == 0 && box->selected_ == false ) {        // left/right click on unselected box
            unselectAll();
        }
        selectBox( box, true, (buttons & kControl) != 0 );

        if( (buttons & kLButton) && (buttons & kControl) == 0 ) {           // left click without control
            focusBox( box, true );
        }
        selection_.isSelecting_ = false;

        if( selection_.isTracking_   == false &&    // track selection with mouse
            selection_.isSelecting_  == false && 
            wireData_.isWiring_      == false && 
            (buttons & kControl)     == 0 &&
            (buttons & kRButton)     == 0)
	    {
            selection_.isTracking_ = true;
            CRect rcBox            = box->getViewSize();
            selection_.offset_     = CPoint( rcBox.left - pos.x, rcBox.top - pos.y );
            selection_.anchor_     = box;
	    }
    }
}


void ProgramView::continueSelecting( const CPoint& pos )
{
    if( selection_.isSelecting_ ) 
    {
        selection_.rubberband_.stretch( pos );
        CRect rcSel = selection_.rubberband_.getViewSize();
        hitTestWires( rcSel );

        for( ModuleBoxList::iterator it=boxes_.begin(); it!=boxes_.end(); it++ )
        {
            CRect rcNext = (*it)->getViewSize();   
            if( rcSel.rectOverlap( rcNext )) {
                selectBox( *it, true );
            }
            else {
                selectBox( *it, false );
                hitTestWires( rcSel );
            }
	    }
        invalid();
    }
}


void ProgramView::endSelecting()
{
    if( selection_.isTracking_ ) {
        rescale();
    }
    selection_.isTracking_ = false;
    selection_.offset_     = CPoint( 0, 0 );
    selection_.anchor_     = NULL;

    invalidRect( selection_.rubberband_.getViewSize() );
    selection_.rubberband_.hide();
    selection_.isSelecting_ = false;
}


void ProgramView::addToSelection( ModuleBox* box, bool toggle )
{
    if( toggle && box->selected_ ) {
        removeFromSelection( box );
        return;
    }
    selection_.map_.insert( ModuleBoxMap::value_type( box->data_->id_, box ));
    box->focusAndSelect( box->focused_, true );
}


void ProgramView::removeFromSelection( ModuleBox* box )
{
    ModuleBoxMap::iterator pos = selection_.map_.find( box->data_->id_ );
    if( pos != selection_.map_.end() )     
    {
        selection_.map_.erase( pos );
        box->focusAndSelect( box->focused_, false );
    }
}


void ProgramView::selectAll()
{
	for( ModuleBoxList::iterator it=boxes_.begin(); it!=boxes_.end(); it++ ) {
        selectBox( *it, true );
	}
    invalid();
}


void ProgramView::unselectAll()
{
    for( ModuleBoxMap::iterator it=selection_.map_.begin(); it!=selection_.map_.end(); it++ ) {
        it->second->focusAndSelect( it->second->focused_, false );
    }
    selection_.map_.clear();

    for( WireMap::iterator it = selection_.wires_.begin(); it != selection_.wires_.end(); )  
    {
        it->second->selected_ = false;
        selection_.wires_.erase( it++ );
    }
    invalid();
}


void ProgramView::advanceFocus( bool forward )
{
    ModuleBox* next = NULL;
    if( selection_.focus_ ) {
        for( UINT16 i=0; i<boxes_.size(); i++ )
	    {
		    if( boxes_[i] == selection_.focus_ ) 
            {
                INT16 idx = forward ? i+1 : i-1;
                if( idx == boxes_.size()) idx = 0;
                if( idx == -1 ) idx = boxes_.size()-1;
                next = boxes_[ idx ];
            }
	    }
    }
    focusBox( next, true );
    invalid();
}


void ProgramView::selectBox( ModuleBox* box, bool select, bool toggleSelection )
{
    if( box )  {
        select ? addToSelection( box, toggleSelection ) : removeFromSelection( box );
    } 
    invalid();
}


void ProgramView::focusBox( ModuleBox* box, bool focus )
{
    ModuleBox* oldFocus = selection_.focus_;

    if( oldFocus ) {    // unfocus old focus
        oldFocus->focusAndSelect( false, oldFocus->selected_ );
    }

    if( box ) 
    {
        if( focus )
        {
            selection_.focus_ = box;
            bringToFront( box );
        }
        else { 
            selection_.focus_ = NULL;
        }
        box->focusAndSelect( focus, box->selected_ );
    } 
    else {
        selection_.focus_ = NULL;
    }

    if( oldFocus != selection_.focus_ ) {
        if( selection_.focus_ ) {
            ctrlView_->updateModule( selection_.focus_->data_ );
        }
        invalid();
    }
    if( selection_.focus_ == NULL ) {
        ctrlView_->updateProgram();
    }
}


void ProgramView::showPrefsDialog()
{
    AppOptions* options = editor_->getOptions();

    PrefsDialog dialog( this, editor_ );
    dialog.show( CPoint( 0, 0 ), true );

    if( dialog.state_ == ModalDialog::Committed )
    {
        if( editor_->isStandalone() == false )
        {
            options->windowRight_  = options->windowLeft_ + dialog.width_;
            options->windowBottom_ = options->windowTop_  + dialog.height_;
            getFrame()->setSize( dialog.width_, dialog.height_ );
        }
        
        options->tuning_ = dialog.tuning_;
        synth_->setTuning( options->tuning_ );

        if( dialog.colorScheme_ != options->colorScheme_ ) {
            options->colorScheme_ = dialog.colorScheme_;
            editor_->setColorScheme();
        }
    }
}


void ProgramView::drawBackgroundRect( CDrawContext *pContext, CRect& rcUpdate )
{
    CViewContainer::drawBackgroundRect( pContext, rcUpdate );

	if( wireData_.wire_ ) {                                                     // draw the wire, that is just being created
		wireData_.wire_->draw( pContext );
	}
	for( ModuleBoxList::iterator it=boxes_.begin(); it!=boxes_.end(); it++ ) {  // draw exising wires
		(*it)->drawWires( pContext );
	}
    if( selection_.rubberband_.isVisible()) {                                   // draw rubberband
        selection_.rubberband_.draw( pContext );
    }
    setDirty( false );
}


void ProgramView::bringToFront( CView* view )
{
	CCView* pSv = pLastView;
	while( pSv )
	{
		if( view == pSv->pView )
		{
			CView* tmp       = pLastView->pView;
			pLastView->pView = pSv->pView;
			pSv->pView       = tmp;
			return;
		}
		pSv = pSv->pPrevious;
	}
}


void ProgramView::createWires()
{
    for( ModuleBoxList::iterator it=boxes_.begin(); it!=boxes_.end(); it++ ) {
        (*it)->createWires();
    }
}


void ProgramView::removeWires( UINT16 targetId )
{
    for( ModuleBoxList::iterator it=boxes_.begin(); it!=boxes_.end(); it++ ) {
        (*it)->removeWires( targetId );
    }
}


void ProgramView::positionWires( ModuleBox* movedBox )
{
	for( ModuleBoxList::iterator it=boxes_.begin(); it!=boxes_.end(); it++ ) {
        (*it)->positionWires( movedBox );
    }
}


void ProgramView::hitTestWires( const CRect& rc )
{
    selection_.wires_.clear();

    for( ModuleBoxList::iterator it=boxes_.begin(); it!=boxes_.end(); it++ ) {   // test which wires are within the rubberband
        (*it)->hitTestWires( CRect( rc.left-2, rc.top-2, rc.right+2, rc.bottom+2 ), selection_.wires_ );
    }
    if( selection_.wires_.empty() == false )    // mark the found wires 
    {
        for( WireMap::iterator it = selection_.wires_.begin(); it != selection_.wires_.end(); it++ )  {
            it->second->selected_ = true;
        }
    }
}


void ProgramView::setProgram()
{
    Program* program = editor_->getCurrentProgram();
    if( program )
	{
		removeAllBoxes();

		for( Program::iterator it=program->begin(); it!=program->end(); it++ ) {
			createBox( *it );
		}
		createWires();
		focusBox( NULL, true ); 
        rescale();
        invalid();
	}
}


bool ProgramView::programMessage( Editor::ProgramMessage msg, CView* sender )
{
	if( isAttached() == false ) 
        return true;

    switch( msg ) {
    case Editor::ProgramChanged:
        setProgram();
        break;
    case Editor::ProgramEdited:
        if( selection_.focus_ == NULL ) {
            ctrlView_->updateProgram();
        }
        break;
    }
    return true;
}


ModuleBox* ProgramView::createBox( ModuleData* data )
{
	ModuleBox* box = new ModuleBox( this, data );
	
	CRect r = box->getViewSize();
	r.moveTo( data->xPos_, data->yPos_ );
	box->moveTo( r );
	
	//addBox( box );
    boxes_.push_back( box );
    addView( box );

	box->realize();

    return box;
}


//bool ProgramView::addBox( ModuleBox* box )
//{
//    boxes_.push_back( box );
//    return addView( box );
//}


bool ProgramView::removeBox( ModuleBox* box )
{
    ModuleBoxList::iterator pos = find( boxes_.begin(), boxes_.end(), box );
    ASSERT( pos != boxes_.end() );
    boxes_.erase( pos );

    return CViewContainer::removeView( box, false );
}


bool ProgramView::removeAllBoxes()
{
    selection_.clear();
    boxes_.clear();
    return removeAll( true );
}


void ProgramView::setBoxLabel( UINT16 id, const string& label )
{
    ModuleBox* box = getBox( id );
    if( box ) {
        box->label_->text_ = label;
        box->invalid();
    }
}


void ProgramView::setBoxPolyphony( UINT16 id )
{
    ModuleBox* box = getBox( id );
    if( box ) {
        box->setColors();
        box->invalid();
    }
}


void ProgramView::newModule( const CPoint& pos, Catalog catalog )
{
    ModuleData* data = new ModuleData( catalog );
    data->setPosition( (UINT16)pos.x, (UINT16)pos.y );

    synth_->addModule( data );
    ModuleBox* box = createBox( data );

    unselectAll();
    focusBox( box, true );
    selectBox( box, true, false );
}


void ProgramView::deleteSelection()
{
    LinkDataList list;
    for( WireMap::iterator it = selection_.wires_.begin(); it != selection_.wires_.end(); ) // delete selected wires 
    {
        if( it->second->selected_ ) {
            list.push_back( it->first );
        }
        ModuleBox* sourceBox = getBox( it->first.sourceId_ );
        sourceBox->removeWire( it->first, OUT_PORT );

        ModuleBox* targetBox = getBox( it->first.targetId_ );
        targetBox->removeWire( it->first, IN_PORT );

        selection_.wires_.erase( it++ );
    }
    synth_->removeLinks( list );

    for( ModuleBoxMap::iterator it=selection_.map_.begin(); it!=selection_.map_.end(); )      // delete boxes
    {
	    try {
            ModuleBox* box = it->second;
            selection_.map_.erase( it++ );

            removeBox( box );
            box->removeWires( box->data_->id_ );
	        removeWires( box->data_->id_ );
	        synth_->removeModule( box->data_ );

            if( box == selection_.focus_ ) {
                selection_.focus_ = NULL;
            }
            delete box;
        }
        catch( const exception& e ) {
            TRACE( e.what() );
            editor_->setAppState( Editor::NotReady );
        }
    }
    if( selection_.focus_ == NULL ) {
        advanceFocus();
    }
    invalid();
}


void ProgramView::clipboardCopy()
{
    ostringstream xml;

    for( ModuleBoxMap::iterator it=selection_.map_.begin(); it!=selection_.map_.end(); it++) 
    {
        ModuleBox* box = it->second;

        Bank::getXml( box->data_, xml );
        xml << endl;
    }
    editor_->copyText( xml );
}


void ProgramView::clipboardCut()
{
    clipboardCopy();
    deleteSelection();
}


void ProgramView::clipboardPaste( const CPoint& pos )
{
    istringstream xml;
    bool result = editor_->pasteText( xml );

    if( result ) 
    {
        unselectAll();
        
        Program program;
        Bank::createFromXml( xml, program );
        program.paste( editor_->getCurrentProgram(), synth_ );

        CPoint anchor( LONG_MAX, LONG_MAX );
        for( Program::iterator it=program.begin(); it!=program.end(); it++ )
        {
            ModuleData* data = *it;
            ModuleBox* box   = createBox( data );
            addToSelection( box, false );

            if( data->xPos_ < anchor.x && data->yPos_ < anchor.y )
            {
                selection_.anchor_ = box;
                anchor( data->xPos_, data->yPos_ );
            }
        }

        for( ModuleBoxMap::iterator it = selection_.map_.begin(); it!=selection_.map_.end(); it++ ) {
            it->second->createWires();
        }

        selection_.isTracking_ = true;
        trackSelection( pos, false );
        endSelecting();
        program.clear( false );

        synth_->setProgram( editor_->getProgramNumber() );  // force a compilation
    }
}


ModuleBox* ProgramView::getBox( UINT16 id )
{
	for( ModuleBoxList::iterator it=boxes_.begin(); it!=boxes_.end(); it++ )
	{
		if( (*it)->data_->id_ == id ) {
			return *it;
		}
	}
	return NULL;
}


ModulePort* ProgramView::getPort( const LinkInfo& linkInfo, PortAlignment portAlign )
{
    ModulePort* port = NULL;
    ModuleBox* box   = getBox( linkInfo.sourceId_ );
    if( box ) {
        port = (portAlign == IN_PORT) ?
            box->getPort( linkInfo.inputId_, IN_PORT ) :
            box->getPort( linkInfo.outputId_, OUT_PORT );
    }
    return port;
}


void ProgramView::getDockPosition( UINT16 moduleId, UINT16 portId, PortAlignment portAlign, CPoint& pos )
{
    ModuleBox* box = getBox( moduleId );
    if( box ) {
        box->getDockPosition( portId, portAlign, pos );
    }
}


Wire* ProgramView::getWire( const LinkInfo& linkInfo, PortAlignment portAlign )
{
    UINT16 id      = (portAlign == OUT_PORT) ? linkInfo.sourceId_ : linkInfo.targetId_;
    ModuleBox* box = getBox( id );

    return box ? box->getWire( linkInfo ) : NULL;
}


void ProgramView::onBoxResized( ModuleBox* box )
{
    positionWires( box );
    invalid();
}


void ProgramView::trackSelection( Direction dir, bool wide )
{
    if( selection_.map_.empty() == false )
    {
        selection_.anchor_ = selection_.map_.begin()->second;
        selection_.offset_ = CPoint( 0, 0 );
        CPoint pos         = selection_.anchor_->getViewSize().getTopLeft();
        int offset         = wide ? 10 : 1;

        switch( dir ) {
        case North: pos.y -= offset; break;
        case East:  pos.x += offset; break;
        case South: pos.y += offset; break;
        case West:  pos.x -= offset; break;
        }
        trackSelection( pos );
        rescale();
    }
}


void ProgramView::trackSelection( const CPoint& pos )
{
    bool canTrack = trackSelection( pos, true );
    if( canTrack ) 
        trackSelection( pos, false );
}


bool ProgramView::trackSelection( const CPoint& pos, bool testing )
{
    if( selection_.map_.size() == 0 ) {
        return false;
    }
    ModuleBox* anchor = selection_.anchor_ ? selection_.anchor_ : selection_.map_.begin()->second;
    CRect rcAnchor    = anchor->getViewSize();
    CPoint offset     = selection_.anchor_ ? selection_.offset_ : CPoint( 0,0 );
        
    for( ModuleBoxMap::iterator it=selection_.map_.begin(); it!=selection_.map_.end(); it++ ) 
    {
        ModuleBox* next = it->second;
        CRect rcNext    = it->second->getViewSize();

        CPoint dest( pos );
        if( next != anchor ) {
            dest.offset( rcNext.left-rcAnchor.left, rcNext.top-rcAnchor.top );
        }

        dest.offset( offset.x, offset.y );
        if( dest.x < 0 || dest.y < 0 ) {
            return false;
        }
        rcNext.moveTo( dest.x, dest.y );

        if( testing == false ) {
	        next->moveTo( rcNext );
	        positionWires( next );
        }
    }
    
    if( testing == false && selection_.lastTrackPos_ != pos ) {
        invalid();
        selection_.lastTrackPos_ = pos;
    }      
    return true;
}


void ProgramView::handleMenu( const CPoint& pos, ModuleBox* box )
{
	try {
        COptionMenu* menu = createMenu( pos, box );
	    menu->takeFocus();

	    long idx;
	    COptionMenu* result = menu->getLastItemMenu( idx );
	
	    if( result ) {
		    long tag = result->getTag();

            switch( tag ) 
            {
            case MenuMain: 
            {
                CMenuItemEx* item = dynamic_cast< CMenuItemEx* >( result->getCurrent() );
                switch( item->tag_ ) {
                case MenuDelete:      deleteSelection();         break;
                case MenuCopy:        clipboardCopy();           break;
                case MenuCut:         clipboardCut();            break;
                case MenuPaste:       clipboardPaste( pos );     break;
                case MenuCollapse:    collapseOrExpand( true );  break;
                case MenuExpand:      collapseOrExpand( false ); break;
                case MenuSelectAll:   selectAll();               break;
                case MenuUnselectAll: unselectAll();             break;
                case MenuPreferences: showPrefsDialog();         break;
                }
                break;
            }

            case MenuModuleNew:
            {
                CMenuItemEx* item = dynamic_cast< CMenuItemEx* >( result->getCurrent() );
                newModule( pos, (Catalog)item->tag_ );
                break;
            }
            }
	    }
        if( isAttached() ) {
	        pParentFrame->removeView( menu );
            pParentFrame->setFocusView( this );
            invalid();
        }
    }
    catch( const exception& e ) {
        TRACE( e.what() );
        editor_->setAppState( Editor::NotReady );
    }
}


COptionMenu* ProgramView::createMenu( const CPoint& pos, ModuleBox* box )
{
	CPoint menuPos( pos );
	localToFrame( menuPos );

	COptionMenu* menu    = new COptionMenu( CRect( menuPos, CPoint( 0, 0 )), NULL, MenuMain );
	COptionMenu* submenu = NULL;
    bool pasteAvail      = editor_->isTextAvailable();
    bool hasMaster       = editor_->getCurrentProgram()->hasMaster();

	if( box == NULL )
    {
		submenu = new COptionMenu( CRect( 0,0,0,0 ), NULL, MenuModuleNew );
											
		ModuleInfoMap* map = ModuleInfo::getMap();
		for( ModuleInfoMap::iterator it=map->begin(); it!=map->end(); it++ ) 
		{
			ModuleInfo* info = it->second;
			if( info->boxStyle_ & BS_VISIBLE ) 
            {
                long flags        = info->catalog_ == MASTER && hasMaster ? CMenuItem::kDisabled : CMenuItem::kNoFlags;
                CMenuItemEx* item = new CMenuItemEx( info->label_.c_str(), info->catalog_, flags );
				submenu->addEntry( item );
			}
		}
		menu->addEntry( submenu, "New Module" );
	}

    if( selection_.map_.empty() == false ) 
    {
	    menu->addEntry( new CMenuItemEx( "Delete", MenuDelete ));
        menu->addSeparator();
        menu->addEntry( new CMenuItemEx( "Copy", MenuCopy ));
        menu->addEntry( new CMenuItemEx( "Cut", MenuCut ));
        menu->addEntry( new CMenuItemEx( "Paste", MenuPaste, pasteAvail ? CMenuItem::kNoFlags : CMenuItem::kDisabled ));
    }
    else {
        menu->addEntry( new CMenuItemEx( "Delete", MenuDelete, CMenuItem::kDisabled ));
        menu->addSeparator();
        menu->addEntry( new CMenuItemEx( "Copy", MenuCopy, CMenuItem::kDisabled ));
        menu->addEntry( new CMenuItemEx( "Cut", MenuCut, CMenuItem::kDisabled ));
        menu->addEntry( new CMenuItemEx( "Paste", MenuPaste, pasteAvail ? CMenuItem::kNoFlags : CMenuItem::kDisabled ));
    }

    menu->addSeparator();
    menu->addEntry( new CMenuItemEx( "Collapse", MenuCollapse ));
    menu->addEntry( new CMenuItemEx( "Expand", MenuExpand ));
    menu->addSeparator();
    menu->addEntry( new CMenuItemEx( "Select All", MenuSelectAll ));
    menu->addEntry( new CMenuItemEx( "Unselect All", MenuUnselectAll ));
    menu->addSeparator();
    menu->addEntry( new CMenuItemEx( "Preferences...", MenuPreferences ));
	
	getFrame()->addView( menu );
	if( submenu ) submenu->forget();
	return menu;
}


void ProgramView::collapseOrExpand( bool collapse )
{
	if( selection_.map_.empty() == false ) {
        for( ModuleBoxMap::iterator it=selection_.map_.begin(); it!=selection_.map_.end(); it++ ) {
            it->second->collapseOrExpand( collapse );
        }
    } else {
        for( ModuleBoxList::iterator it=boxes_.begin(); it!=boxes_.end(); it++ ) {
            (*it)->collapseOrExpand( collapse );
        }
    }
}


ModulePort* ProgramView::hitTestPort( const CPoint& pos )
{
    CView* view = getViewAt( pos, true );
    if( view && view->isTypeOf( "ModulePort" ))	{
		ModulePort* port = dynamic_cast< ModulePort* >( view );

        if( port->testDocking( pos )) {
            return port;
        }
	}
    return NULL;
}


ModuleBox* ProgramView::hitTestBox( const CPoint& pos )
{
    CView* view = getViewAt( pos, false );
    if( view && view->isTypeOf( "ModuleBox" )) {
		return dynamic_cast< ModuleBox* >( view );
	}
    return NULL;
}


void ProgramView::setPreferredSize( const CRect& rcSize )   
{ 
    rcPreferred_ = rcSize; 
    rescale();
}


void ProgramView::rescale()
{
    CRect rcUsed( 0,0,0,0 );
    getUsedArea( rcUsed );

    CRect rcScroll = rcPreferred_;
    rcScroll.unite( rcUsed );
    scrollView_->setContainerSize( rcScroll, true );
        
    CRect rcVisible = scrollView_->getViewSize();
    scrollView_->enableScrollbars( rcUsed.width() > rcVisible.width(), rcUsed.height() > rcVisible.height() );
        
    CPoint offset = scrollView_->getScrollOffset();
    rcScroll.unite( rcVisible );
    size( offset.x, -offset.y, offset.x + rcScroll.width(), -offset.y + rcScroll.height() );
    setMouseableArea( size );
    invalid();
}


void ProgramView::getUsedArea( CRect& rcUnion )
{
    for( ModuleBoxList::iterator it=boxes_.begin(); it!=boxes_.end(); it++ )
	{
        CRect next = (*it)->getViewSize();
        rcUnion.unite( next );
	}
}


//---------------------------------------------------------------
// struct SelectionData
//---------------------------------------------------------------

void ProgramView::SelectionData::clear()
{
    focus_       = NULL;
    anchor_      = NULL;
    isSelecting_ = false;
    isTracking_  = false;
    offset_      = CPoint( 0, 0 );

    map_.clear();
    wires_.clear();
}


void ProgramView::SelectionData::getBoundingRect( CRect& rcUnion )
{
    for( ModuleBoxMap::iterator it=map_.begin(); it!=map_.end(); it++ )
	{
        CRect next = it->second->getViewSize();
        rcUnion.unite( next );
	}
}





//---------------------------------------------------------------
// struct WireData
//---------------------------------------------------------------

ProgramView::WireData::WireData()		
{ 
	reset(); 
}


void ProgramView::WireData::reset()	
{ 
	isWiring_ = false; 
	wire_     = NULL; 
	source_   = NULL; 
	target_   = NULL; 

    for( ModuleBoxList::iterator it=autoExpanded_.begin(); it!=autoExpanded_.end(); it++ ) {
        (*it)->collapseOrExpand( true );
    }
    autoExpanded_.clear();
}


void ProgramView::WireData::cleanup()	
{ 
	if( wire_ ) delete wire_; 
	reset(); 
}


void ProgramView::WireData::removeAutoExpanded( ModuleBox* box )
{
    ModuleBoxList::iterator pos = find( autoExpanded_.begin(), autoExpanded_.end(), box );
    if( pos != autoExpanded_.end() ) {
        autoExpanded_.erase( pos );
    }
}


