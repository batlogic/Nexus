
#include "TextEdit.h"
#include "ChildFrame.h"



//-------------------------------------------
// class TextEdit
//-------------------------------------------

TextEdit::TextEdit( CRect &rcSize, CControlListener* listener, long tag, string text ) :
	CControl( rcSize, listener, tag ),
	hAlign_( kLeftText ),
	font_( kSystemFont ),
    state_( Idle ),
    style_( SelectOnOpen | LooseFocusSaves ),
    openOn_( SingleClick ),
    data_( NULL ),
    position_( 0 ),
    scrollOffset_( 0 ),
    indent_( CPoint( 2, 0 )),
    caretPosition_( size.left + indent_.x ),
    maxLength_( 255 ),
    selectionFirst_( 0 ),
    selectionLast_( 0 ),
    selecting_( false ),
    mouseSelecting_( false ),
    anchor_( indent_.x ),
    inMenu_( false )
{
	setTransparency( true );
    setWantsFocus( true );
    setText( text );
    setInsertPosition( text_.length(), false );
    setBkgndColors( colTransparent, colWhite, colOrange );
    setFrameColors( colTransparent, colTransparent );
    setTextColors( colBlack, colBlack, colBlack, colBlack );
}


CMouseEventResult TextEdit::onMouseEntered( CPoint& pos, const long& buttons )
{
    if( state_ == Editing ) {
        setCursor( CursorBeam );
    }
    return CControl::onMouseEntered( pos, buttons );
}


CMouseEventResult TextEdit::onMouseExited( CPoint& pos, const long& buttons )
{
    if( state_ == Editing ) {
        setCursor( CursorDefault );
    }
    return CControl::onMouseExited( pos, buttons );
}


CMouseEventResult TextEdit::onMouseDown( CPoint& pos, const long& buttons )
{
    if( state_ == Idle ) 
    {
        if( ((openOn_ == SingleClick) && (buttons == kLButton))     ||
            ((openOn_ == DblClick)    && (buttons & kDoubleClick))  ||
            ((openOn_ == CtrlClick)   && ((buttons & kControl) > 0)))
        {
            if( pParentFrame->getFocusView() != this ) {
                pParentFrame->setFocusView( this );
            }
            setState( Editing );
            return kMouseEventHandled;
        }
    } 
    else if( state_ == Editing ) 
    {
        if( buttons & kLButton )
        {
            UINT16 position = translateCoord( displayText_, pos.x );
            setInsertPosition( position + scrollOffset_ );

            if( selecting_ ) {
                unselectAll();
            }
            beginMouseSelecting( pos );
        }
        else if( buttons & kRButton ) {
            handleMenu( pos );
        }
        return kMouseEventHandled;
    }
    return kMouseEventNotHandled;
}


CMouseEventResult TextEdit::onMouseMoved( CPoint& pos, const long& buttons )
{
    continueMouseSelecting( pos );
    return CControl::onMouseMoved( pos, buttons );
}


CMouseEventResult TextEdit::onMouseUp( CPoint& pos, const long& buttons )
{
    endMouseSelecting();
    return CControl::onMouseUp( pos, buttons );
}


long TextEdit::onKeyDown( VstKeyCode& keyCode )
{
    //TRACE( "virt=%d character=%d modifier=%d\n", keyCode.virt, keyCode.character, keyCode.modifier );

    if( state_ != Editing ) {
        return 0;
    }
    if( keyCode.virt == 0 && (keyCode.modifier == 0 || keyCode.modifier == MODIFIER_SHIFT) )
    {
        INT16 c = keyCode.character;
        if( c >= 32 && c <= 127 )
        {
            deleteSelection();
        
            if( keyCode.modifier == MODIFIER_SHIFT ) {
                c = toupper( c );
            }
            addChar( c );
            return 1;
        }
    }
    if( keyCode.virt == 0 && keyCode.modifier == MODIFIER_CONTROL )
    {
        switch( keyCode.character ) {
        case 97:  selectAll();      break;        // Ctrl-a
        case 99:  clipboardCopy();  break;        // Ctrl-c
        case 118: clipboardPaste(); break;        // Ctrl-v
        case 120: clipboardCut();   break;        // Ctrl-x
        default:  return 0;
        }
        return 1;
    }
    else if( keyCode.virt )
    {
        switch( keyCode.virt ) {
        case VKEY_RIGHT:    
            setInsertPosition( position_ + 1 );       
            if( keyCode.modifier == MODIFIER_SHIFT )
                continueSelecting();
            break;
        case VKEY_LEFT:     
            setInsertPosition( position_ - 1 );
            if( keyCode.modifier == MODIFIER_SHIFT )
                continueSelecting();
            break;
        case VKEY_HOME:    
            setInsertPosition( 0 );
            if( keyCode.modifier == MODIFIER_SHIFT )
                continueSelecting();
            break;
        case VKEY_END:     
            setInsertPosition( text_.length() ); 
            if( keyCode.modifier == MODIFIER_SHIFT )
                continueSelecting();
            break;
        case VKEY_TAB:
            setState( Idle );
            if( style_ & NeedsReturn ) {
                commit( true );
                notify( true );
            }
            return pParentView->onKeyDown( keyCode );
        case VKEY_RETURN:
        case VKEY_ENTER:   
            setState( Idle );
            if( style_ & NeedsReturn ) {
                commit( true );
                notify( true );
            }
            break;
        case VKEY_ESCAPE:   
            commit( false );
            setState( Idle );               
            break;
        case VKEY_DELETE:   deleteSelectionOrChar( false ); break;
        case VKEY_BACK:     deleteSelectionOrChar( true );  break;
        case VKEY_SHIFT:    beginSelecting();               break;
        case VKEY_SPACE:    addChar( 32 );                  break;
        case VKEY_SUBTRACT: addChar( 45 );                  break;
        case VKEY_ADD:      addChar( 43 );                  break;
        }

        if( keyCode.modifier == 0 ) {
            endSelecting();
        }
        return 1;
    }
    return 0;
}


void TextEdit::setState( State state ) 
{
    if( pParentFrame )
    {
        if( state_ == Idle && state == Editing ) {
            setEditState();
        }
        else if( state_ == Editing && state == Idle ) {
            setIdleState();
        }
    }
}


void TextEdit::setEditState()
{
    state_     = Editing;
    savedText_ = text_;
    caret_.show( this, size.height()-4 );
    setCursor( CursorBeam );
    setInsertPosition( position_ );
    if( style_ & SelectOnOpen ) {
        selectAll();
    }
}


void TextEdit::setIdleState()
{
    state_ = Idle;
    unselectAll();
    setInsertPosition( 0 );
    caret_.hide();
    setCursor( CursorDefault );
}


void TextEdit::setText( const string& text ) 
{ 
    text_ = displayText_ = savedText_ = text; 
}


void TextEdit::setBkgndColors( CColor colIdle, CColor colEdit, CColor colSel )
{
    colBkgndIdle_ = colIdle;
    colBkgndEdit_ = colEdit;
    colBkgndSel_  = colSel;
}


void TextEdit::setFrameColors( CColor colIdle, CColor colEdit )
{
    colFrameIdle_ = colIdle;
    colFrameEdit_ = colEdit;
}


void TextEdit::setTextColors( CColor colIdle, CColor colEdit, CColor colSel, CColor colFocus )
{
    colTextIdle_  = colIdle;
    colTextEdit_  = colEdit;
    colTextSel_   = colSel;
    colTextFocus_ = colFocus;
}


void TextEdit::setViewSize( CRect& rcSize, bool invalid )
{
    CPoint offset( size.left - rcSize.left, size.top - rcSize.top );
    caretPosition_ += offset.x;
    caret_.setPosition( CPoint( caretPosition_, rcSize.top+2 ));

    CControl::setViewSize( CRect( rcSize ), invalid );
}


void TextEdit::parentSizeChanged()
{
    if( state_ == Editing )
        caret_.setPosition( CPoint( caretPosition_, size.top+2 ));
}


void TextEdit::setCursor( CursorType type )
{
    ChildFrame* childFrame = dynamic_cast< ChildFrame* >( pParentFrame );
    if( childFrame ) {
        childFrame->setCursor( type );
    }
}


void TextEdit::setInsertPosition( INT16 position, bool scroll )
{
    position  = min( position, (INT16)text_.length());
    position  = max( 0, position );
    position_ = position;

    if( scroll ) scrollText();
}


void TextEdit::takeFocus()
{
    if( inMenu_ == false && state_ == Idle ) 
        setState( Editing );
}


void TextEdit::looseFocus()
{
    if( inMenu_ == false && state_ == Editing )
    {
        bool save = ((style_ & LooseFocusSaves) != 0) && (savedText_ != text_);
        commit( save );
        notify( save );
        setState( Idle );
    }
}


CPoint& TextEdit::localToFrame( CPoint& pos ) const
{
	if( pParentView )
		return pParentView->localToFrame( pos );
	return pos;
}


void TextEdit::addChar( int code )
{
    size_t prev = text_.length();
    insert( position_, code );
    size_t diff = (text_.length() - prev);

    setInsertPosition( position_ + diff );
}


void TextEdit::insert( size_t pos, int c )
{
    VERIFY( pos >= 0 && pos <= text_.length() );

    if( validate( c )) 
    {
        text_.insert( pos, (const char*)&c );
        notify();
    }
}


void TextEdit::insert( size_t pos, const string& text )
{
    VERIFY( pos >= 0 && pos <= text_.length() );

    if( validate( text )) {    
        text_.insert( pos, text );
        notify();
    }
}


void TextEdit::erase( size_t pos, size_t len )
{
    VERIFY( pos >= 0 && pos < text_.length() && len <= text_.length() );
    text_.erase( pos, len );
    notify();
}


void TextEdit::commit( bool commit )
{
    if( state_ == Editing ) 
        commit ? savedText_ = text_ : text_ = savedText_;
}


void TextEdit::notify( bool force )
{
    if( listener && ( force || (style_ & NeedsReturn) == 0 )) {
        listener->valueChanged( this );
    }
}


void TextEdit::deleteSelection()
{
    if( selecting_ )
    {
        normalizeSelection( selectionFirst_, selectionLast_ );
        erase( selectionFirst_, selectionLast_ - selectionFirst_ );
        INT16 diff = selectionLast_ - selectionFirst_;
        endSelecting();
        setInsertPosition( position_ - diff );
    }
}


void TextEdit::deleteSelectionOrChar( bool backspace )
{
    if( selecting_ == false ) 
    {
        INT16 position = backspace ? max( 0, position_ - 1 ) : position_;
        erase( position, 1 );
        if( backspace ) setInsertPosition( position_ - 1 );
    }
    else {
        deleteSelection();
    }
    setInsertPosition( position_ );
}


void TextEdit::deleteAll()
{
    deleteSelection();
    erase( 0, text_.length() );
    setInsertPosition( 0 );
}


bool TextEdit::validate( int code )
{
    return (INT16)text_.length() <= maxLength_ - 1;
}


bool TextEdit::validate( const string& s )
{
    for( UINT16 i=0; i<s.length(); i++ )
    {
        int c = (int)s.at( i );
        if( validate( c ) == false ) {
            return false;
        }
    }
    return true;
}


void TextEdit::beginSelecting()
{
    if( selecting_ == false ) 
    {
        selectionFirst_   = position_;
        selectionLast_    = position_;
        rcSelection_      = CRect( caretPosition_, size.top+2, caretPosition_, size.bottom-2 );
    }
    selecting_ = true;
}


void TextEdit::continueSelecting()
{
    rcSelection_.right = min( size.right-indent_.x, caretPosition_ );
    selectionLast_     = position_;
}


void TextEdit::endSelecting()
{
    rcSelection_( size.left, size.top, size.left, size.top );
    selectionFirst_ = 0;
    selectionLast_  = 0;
    selecting_      = false;
}


void TextEdit::selectAll()
{
    rcSelection_.top    = size.top+2;
    rcSelection_.bottom = size.bottom-2;
    rcSelection_.left   = size.left + indent_.x;
    rcSelection_.right  = text_.empty() ? size.left+indent_.x : getCoord( text_ ) + indent_.x*2;
    selectionFirst_     = 0;
    selectionLast_      = text_.length();
    selecting_          = true;

    invalid();
}


void TextEdit::unselectAll()
{
    endSelecting();
    invalid();
}


void TextEdit::beginMouseSelecting( const CPoint& pos )
{
    mouseSelecting_   = true;
    CCoord last       = getCoord( displayText_ );
    CCoord x          = max( size.left+indent_.x, min( last, pos.x ));
    rcSelection_      = CRect( x, size.top+2, x, size.bottom-2 );
    selectionFirst_   = translateCoord( displayText_, pos.x );
    selectionLast_    = selectionFirst_;
}


void TextEdit::continueMouseSelecting( const CPoint& pos )
{
    if( mouseSelecting_ ) 
    {
        CCoord last        = getCoord( displayText_ );
        rcSelection_.right = max( size.left+indent_.x*2, min( last, pos.x ));

        if( abs(rcSelection_.width()) >= 7 ) {
            selectionLast_     = translateCoord( displayText_, rcSelection_.right );
            rcSelection_.right = getCoord( displayText_.substr(0, selectionLast_ ));
            invalid();
        }
        else rcSelection_.right = rcSelection_.left;
    }
}


void TextEdit::endMouseSelecting()
{
    mouseSelecting_ = false;
    selecting_      = true;

    normalizeSelection( selectionFirst_, selectionLast_ );
    setInsertPosition( selectionLast_ );
}


void TextEdit::normalizeSelection( INT16& first, INT16& last )
{
    if( first > last ) {
        INT16 temp = last;
        last       = first;
        first      = temp;
    }
    first = max( 0, min( (INT16)text_.length(), first ));
    last  = max( 0, min( (INT16)text_.length(), last ));
}


string TextEdit::getSelectedString()
{
    INT16 first = selectionFirst_;
    INT16 last  = selectionLast_;
    normalizeSelection( first, last );

    return text_.substr( first, last-first );
}


bool TextEdit::isSelected()
{
    long dummy;
    return getAttribute( ATTR_SELECTED, 0, NULL, dummy );
}


bool TextEdit::isFocused()
{
    long dummy;
    return getAttribute( ATTR_FOCUSED, 0, NULL, dummy );
}


void TextEdit::clipboardCopy( const string& s )
{
    if( getFrame() == NULL )
        return;

    HWND hwnd  = (HWND)getFrame()->getSystemWindow();
    UINT16 len = s.length() + 1;
    
    if( OpenClipboard( hwnd ) )
    {
        EmptyClipboard();
        HGLOBAL hClipboardData;
        hClipboardData = GlobalAlloc( GMEM_DDESHARE, len );

        char* buf;
        buf = (char*)GlobalLock( hClipboardData );
        strncpy_s( buf, len, LPCSTR( s.c_str() ), len );

        GlobalUnlock( hClipboardData );
        SetClipboardData( CF_TEXT, hClipboardData );
        CloseClipboard();
    }
}


void TextEdit::clipboardCopy()
{
    string s = getSelectedString();
    clipboardCopy( s );
}


void TextEdit::clipboardPaste()
{
    if( getFrame() == NULL )
        return;

    HWND hwnd = (HWND)getFrame()->getSystemWindow();
    if( OpenClipboard( hwnd )) 
    {
        deleteSelection();

        if( IsClipboardFormatAvailable( CF_TEXT ) || IsClipboardFormatAvailable( CF_OEMTEXT ))
        {
            HANDLE hClipboardData = GetClipboardData( CF_TEXT );
            char* buf = (char*)GlobalLock( hClipboardData );

            istringstream iss( buf );                       // get only first line
            string line;
            getline( iss, line);
            
            if( validate( line )) 
            {
                insert( position_, line );
                setInsertPosition( position_ + line.length() );       // scroll to end of pasted segment
            }
        }
        CloseClipboard();
    } 
}


void TextEdit::clipboardCut()
{
    string s = getSelectedString();
    deleteSelectionOrChar( false );
    clipboardCopy( s );
}


void TextEdit::scrollText()
{
    if( getFrame() )
    {
        CDrawContext* context = getFrame()->createDrawContext();
        context->setFont( font_, font_->getSize());

        string front  = text_.substr( 0, position_ );
        string back   = text_.substr( position_, text_.length() - position_ );

        CCoord pos    = 0;
        CCoord left   = size.left + indent_.x;
        CCoord right  = size.right - indent_.x;
        scrollOffset_ = 0;
        do {
            if( pos > right ) {
                scrollOffset_ = min( (INT16)text_.length(), ++scrollOffset_ );
                front = front.substr( scrollOffset_, (INT16)text_.length() - scrollOffset_ );
            }
            pos = left + context->getStringWidthUTF8( front.c_str() );
        }
        while( pos > right );

        displayText_ = front + back;

        caretPosition_ = min( size.right-indent_.x, pos );
        caret_.setPosition( CPoint( caretPosition_, size.top+2 ));
        context->forget();
        invalid();
    }
}


CCoord TextEdit::getCoord( const string& text )
{
    if( getFrame() )
    {
        CDrawContext* context = getFrame()->createDrawContext();
        context->setFont( font_, font_->getSize());

        CCoord pos = size.left + indent_.x +context->getStringWidth( text.c_str() );
        context->forget();

        return pos;
    }
    return indent_.x;
}


UINT16 TextEdit::translateCoord( const string& text, CCoord pos )
{
    CDrawContext* context = getFrame()->createDrawContext();
    context->setFont( font_, font_->getSize());
    
    UINT16 len = displayText_.length() + 1;
    char* buf  = new char[ len ];
    strncpy_s( buf, len, text.c_str(), len );

    CCoord width; 
    UINT16 idx = len - 1;
    pos -= size.left + indent_.x;
    do {
        width = context->getStringWidth( buf );
        if( width <= pos || idx <= 0 ) {
            break;
        }
        buf[ --idx ] = '\0';
    } 
    while( true );

    context->forget();
    delete buf;
    return idx;
}


void TextEdit::handleMenu( const CPoint& pos )
{
    try {
        inMenu_ = true;
        setCursor( CursorDefault );
        COptionMenu* menu = createMenu( pos );
	    menu->takeFocus();

	    long idx;
	    COptionMenu* result = menu->getLastItemMenu( idx );
	    if( result ) 
        {
            CMenuItemEx* item = dynamic_cast< CMenuItemEx* >( result->getCurrent() );
            
            switch( item->tag_ ) 
            {
            case CmdCopy:      clipboardCopy();   break;
            case CmdCut:       clipboardCut();    break;
            case CmdPaste:     clipboardPaste();  break;
            case CmdDeleteSel: deleteSelection(); break;
            case CmdSelectAll: selectAll();       break;
            }
	    }
	    pParentFrame->removeView( menu );
        pParentFrame->setFocusView( this );
        inMenu_ = false;
    }
    catch( const exception& e ) {
        TRACE( e.what() );
    }
}


COptionMenu* TextEdit::createMenu( const CPoint& pos )
{
    bool copy    = selecting_ && selectionFirst_ < selectionLast_;
    bool paste   = IsClipboardFormatAvailable( CF_TEXT ) || IsClipboardFormatAvailable( CF_OEMTEXT );
    bool hasText = text_.empty() == false;
    
    CPoint menuPos( pos );
	localToFrame( menuPos );
	COptionMenu* menu = new COptionMenu( CRect( menuPos, CPoint( 0, 0 )), NULL, 0 );

    menu->addEntry( new CMenuItemEx( "Copy",  CmdCopy,      copy    ? CMenuItem::kNoFlags : CMenuItem::kDisabled ));
    menu->addEntry( new CMenuItemEx( "Cut",   CmdCut,       copy    ? CMenuItem::kNoFlags : CMenuItem::kDisabled  ));
    menu->addEntry( new CMenuItemEx( "Paste", CmdPaste,     paste   ? CMenuItem::kNoFlags : CMenuItem::kDisabled ));
    menu->addEntry( new CMenuItemEx( "Clear", CmdDeleteSel, hasText ? CMenuItem::kNoFlags : CMenuItem::kDisabled ));
    menu->addSeparator();
    menu->addEntry( new CMenuItemEx( "Select All", CmdSelectAll, hasText ? CMenuItem::kNoFlags : CMenuItem::kDisabled ));

	getFrame()->addView( menu );
    return menu;
}




void TextEdit::draw (CDrawContext* pContext)
{
    CRect rc = getViewSize();
    CColor colBkgnd = state_ == Idle ? colBkgndIdle_ : colBkgndEdit_;
    CColor colFrame = state_ == Idle ? colFrameIdle_ : colFrameEdit_;

    pContext->setFillColor( colBkgnd );
    pContext->setFrameColor( colFrame );
    pContext->setLineWidth( 1 );
    pContext->drawRect( rc, kDrawFilledAndStroked );

    CRect rcSelection( rcSelection_ );
    rcSelection.normalize();
    pContext->setFillColor( colBkgndSel_ );
    pContext->drawRect( rcSelection, kDrawFilled );
    
    drawText( pContext );
    setDirty ( false );
}


void TextEdit::drawText( CDrawContext* pContext )
{
    CColor colText;
    colText = isSelected() ? colTextSel_   : colTextIdle_;
    colText = isFocused()  ? colTextFocus_ : colText;
    colText = isEditing()  ? colTextEdit_  : colText;

    CRect rc( size.left+indent_.x, size.top+indent_.y, size.right-indent_.y, size.bottom );
    CHoriTxtAlign hAlign = state_ == Idle ? hAlign_ : kLeftText;

    pContext->setFont( font_, font_->getSize());
	pContext->setFontColor( colText );
	pContext->drawStringUTF8( displayText_.c_str(), rc, hAlign, true );
}



//-------------------------------------------
// class NumericEdit
//-------------------------------------------

NumericEdit::NumericEdit( CRect &rcSize, CControlListener* listener, long tag, float value, NumberFormat format ) :
    TextEdit( rcSize, listener, tag, "" ),
    format_( format ),
    value_( 0 ),
    maxValue_( (float)INT_MAX ),
    minValue_( (float)INT_MIN ),
    precision_( 2 ),
    tracking_( false ),
    trackInc_( 0.0f )
{
    style_ = SelectOnOpen;
    setValue( value );
}


CMouseEventResult NumericEdit::onMouseDown( CPoint& pos, const long& buttons )
{
    TextEdit::onMouseDown( pos, buttons );
    if( buttons & kLButton && state_ == Editing ) 
    {
        if( size.pointInside( pos ) == false ) {
            tracking_ = false;
            setMouseCapture( NULL );
            getFrame()->setFocusView( NULL );
            return kMouseEventNotHandled;
        }
        trackInc_  = 0;
        trackLast_ = pos.y;
        tracking_  = true;
        setMouseCapture( this );

        UINT16 position = translateCoord( displayText_, pos.x );

        int decimal = displayText_.find( '.', 0 );
        if( decimal < 0 ) {
            trackInc_ = max( 1, pow( (float)10, (int)(displayText_.length() - position - 1 )));
        }
        else if( position <= decimal ) {
            string intPart = displayText_.substr( 0, decimal-1 );
            trackInc_ = max( 1, pow( (float)10, (int)(intPart.length() - position - 1 )));
        }
        else if( position > decimal ) {
            string fractPart = displayText_.substr( decimal, displayText_.length() );
            trackInc_ = 1 / pow( (float)10, (int)( position - 1 ));
        }
    }
    return kMouseEventHandled;
}


CMouseEventResult NumericEdit::onMouseMoved( CPoint& pos, const long& buttons )
{
    TextEdit::onMouseMoved( pos, buttons );

    trackValue( pos );

    if( state_ == Editing ) {
        size.pointInside( pos ) ? onMouseEntered( pos, buttons ) : onMouseExited( pos, buttons );
    }
    return kMouseEventHandled;
}


CMouseEventResult NumericEdit::onMouseUp( CPoint& pos, const long& buttons )
{
    TextEdit::onMouseUp( pos, buttons );

    tracking_ = false;
    setMouseCapture( NULL );

    return kMouseEventHandled;
}


bool NumericEdit::validate( int code )
{
    if( code == 46 ) {                  // decimal point
        return format_ == Float || format_ == Decibel;
    }
    if( code == 43 || code == 45 ) {    // plus/minus
        return true;
    }
    return isdigit( code ) != 0;
}


float NumericEdit::getValue() const
{
    return max( minValue_, min( maxValue_, value_ ));
}


bool NumericEdit::getValue( float& value, const string& text) const
{
    switch( format_ ) {
    case Integer: 
    {
        long lValue = (long)value;
        bool result = fromString<long>( lValue, text );
        value       = (float)lValue;
        return result;
    }
    case Float:   return fromString<float>( value, text );
    case Decibel: return fromDecibel( value, text );
    default:      return false;
    }
}


void NumericEdit::setValue( float value )
{
    value         = max( minValue_, min( maxValue_, value ));
    value         = (format_ == Float || format_ == Decibel) ? value : (long)floor( value );
    value_        = max( minValue_, min( maxValue_, value ));
    int precision = (format_ == Float || format_ == Decibel ) ? precision_ : 0;

	switch( format_ ) 
    {
	case Integer:   
	case Float:     toString( value, text_, precision );   break;
    case Decibel:	dbToString( value, text_, precision ); break;
    default: ASSERT( false ); break;
	}
    displayText_ = text_;
    invalid();
}


void NumericEdit::commit( bool commit )
{
    setValue( value_ );     // range check
    TextEdit::commit( commit );
}


void NumericEdit::toString( FLOAT value, string& result, UINT16 precision  ) const
{
    ostringstream os;
    os << setprecision( precision ) << fixed << value;

    result = os.str();
}


void NumericEdit::dbToString( FLOAT value, string& result, UINT16 precision ) const
{
	if (value <= 0) {
        result = "-oo";
	} else {
		toString( 20.f * log10( value ), result, precision );
	}
}


bool NumericEdit::fromDecibel( float& value, const string& text ) const
{
    bool result = true;
    if( text == "-oo" ) value = 0.f;
    else {
        result = fromString( value, text );
        value  = pow( 10, (value/20.f));
    }
    return result;
}


void NumericEdit::insert( size_t pos, int c )
{
    ASSERT( pos >= 0 && pos <= text_.length() );

    if( validate( c ))
    {
        if( pos == 0 && ( c == 43 || c == 45 ))  // plus/minus at first position
        {
            text_.insert( pos, (const char*)&c );
        }
        else if( c != 45 && c != 43 && pos <= text_.length() )
        {
            string s = text_;
            s.insert( pos, (const char*)&c );
        
            float val;
            bool valid = getValue( val, s );
            if( valid )
            {
                text_.insert( pos, (const char*)&c );
                value_ = val;

                //if( val < minValue_ )
                //    setValue( minValue_ );
                //else if( val > maxValue_ )
                //    setValue( maxValue_ );
                //else 
                //    value_ = val;
              
                notify();
            }
        }
    }
}


void NumericEdit::insert( size_t pos, const string& text )
{
    for( UINT16 i=0; i<text.length(); i++ ) {
        insert( i + pos, text[ i ]);
    }
}


void NumericEdit::erase( size_t pos, size_t len )
{
    ASSERT( pos >= 0 && pos < text_.length() && len <= text_.length() );
    text_.erase( pos, len );

    bool valid = getValue( value_, text_ );
    if( valid == false ) {
        value_ = 0.0f;
    }
    notify();
}


void NumericEdit::trackValue( const CPoint& pos )
{
    if( tracking_ )
    {
        CCoord delta = trackLast_ - pos.y;
        if( abs( delta ) > 5 ) 
        {
            float offset = delta >= 0 ? trackInc_ : -trackInc_;
            setValue( value_ + offset );
            notify();
            invalid();
            trackLast_ = pos.y;
        }
    }
}


void NumericEdit::setMouseCapture( CView* view )
{
    ChildFrame* childFrame = dynamic_cast< ChildFrame* >( pParentFrame );
    if( childFrame )
        childFrame->setMouseCaptureView( view );
}


void NumericEdit::setIdleState()
{
    TextEdit::setIdleState();
    setMouseCapture( NULL );
    tracking_ = false;
}


void NumericEdit::drawText( CDrawContext* pContext )
{
    CRect rc             = getViewSize();
    CColor colText       = state_ == Idle ? colTextIdle_  : colTextEdit_;
    CHoriTxtAlign hAlign = state_ == Idle ? hAlign_ : kLeftText;

    rc.inset( indent_.x, indent_.y );
    pContext->setFont( font_, font_->getSize());
	pContext->setFontColor( colText );
	pContext->drawStringUTF8( displayText_.c_str(), rc, hAlign, false );

    rc.left += pContext->getStringWidth( displayText_.c_str() ) + 2;
    pContext->drawStringUTF8( unit_.c_str(), rc, hAlign, false );
}



