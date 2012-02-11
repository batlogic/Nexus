
#include "BankView.h"
#include "Widgets.h"
#include "Fonts.h"
#include "TextEdit.h"
#include "Images.h"
#include "../Synth.h"
#include "../VST/cfileselector.h"

#include <sys/stat.h>




//--------------------------------------------------------------------
// class BankBoxItem
//--------------------------------------------------------------------

BankBoxItem::BankBoxItem( INT32 programNum, const string& name, const string& category, const string& comment ) :
    GridBoxItem(),
    programNum_( programNum )
{
    ostringstream os;
    os << programNum_;
    lblProgramNum_          = new Label( CRect( 0,0,0,0 ), os.str() );
    lblProgramNum_->font_   = fontArial10;
    lblProgramNum_->indent_ = CPoint( 2, -1 );
    addControl( lblProgramNum_ );
    
    CRect rcEmpty( 0,0,0,0 );
    txtName_          = new TextEdit( rcEmpty, this, ColName, name );
    txtName_->style_  = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
    txtName_->openOn_ = TextEdit::DblClick;
    txtName_->font_   = fontArial10;
    txtName_->indent_ = CPoint( 2, -1 );
    addControl( txtName_ );

    txtCategory_          = new TextEdit( rcEmpty, this, ColCategory, category );
    txtCategory_->style_  = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
    txtCategory_->openOn_ = TextEdit::DblClick;
    txtCategory_->font_   = fontArial10;
    txtCategory_->indent_ = CPoint( 2, -1 );
    addControl( txtCategory_ );

    txtComment_          = new TextEdit( rcEmpty, this, ColComment,  comment );
    txtComment_->style_  = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
    txtComment_->openOn_ = TextEdit::DblClick;
    txtComment_->font_   = fontArial10;
    txtComment_->indent_ = CPoint( 2, -1 );
    addControl( txtComment_ );

    setColors();
}


void BankBoxItem::setColors()
{
    lblProgramNum_->setColors( colors.text2, colors.text2, colors.text1, colTransparent, colTransparent );
    
    txtName_->setBkgndColors( colTransparent, colWhite, colors.selection );
    txtName_->setTextColors( colors.text2, colBlack, colors.text2, colors.text1 );

    txtCategory_->setBkgndColors( colTransparent, colWhite, colors.selection );
    txtCategory_->setTextColors( colors.text2, colBlack, colors.text2, colors.text1 );

    txtComment_->setBkgndColors( colTransparent, colWhite, colors.selection );
    txtComment_->setTextColors( colors.text2, colBlack, colors.text2, colors.text1 );
}


void BankBoxItem::getText( INT32 columnId, string& text ) const
{
    switch( columnId ) {
    case ColName:     text = txtName_->getText(); break;
    case ColCategory: text = txtCategory_->getText(); break;
    case ColComment:  text = txtComment_->getText(); break;
    }
}


void BankBoxItem::setText( INT32 columnId, const string& text )
{
    switch( columnId ) {
    case ColName:     txtName_->setText( text ); break;
    case ColCategory: txtCategory_->setText( text ); break;
    case ColComment:  txtComment_->setText( text ); break;
    }
}


float BankBoxItem::getValue( INT32 columnId ) const
{
    if( columnId == ColNumber ) {
        return (float)programNum_;
    }
    return 0.0f;
}


bool BankBoxItem::isLess( GridBoxItem* ref ) const
{ 
    const SortInfo& sortInfo = owner_->getSortInfo();
    INT32 columnId           = sortInfo.columnId_;

    switch( columnId ) {
    case ColNumber: 
    {
        float valOwn = getValue( ColNumber );
        float valRef = ref->getValue( ColNumber );

        return sortInfo.compare< float >( valOwn, valRef );
    }
    default:
    {
        string textOwn, textRef;
        getText( columnId, textOwn );
        ref->getText( columnId, textRef );

        return sortInfo.compare< string >( textOwn, textRef );
    }
    }
}




//--------------------------------------------------------------------
// class BankView
//--------------------------------------------------------------------

BankView::BankView( const CRect& size, Editor* editor) : 
    CViewContainer( size, editor->getChildFrame(), NULL ),
	editor_( editor ),
    synth_( editor->getSynth() ),
    bank_( editor->getBank() )
{
    setMouseableArea( size );
	setTransparency( true );
    setWantsFocus( true );
    setAutosizeFlags( kAutosizeAll );

    VstFileType nexusType( "Nexus Bank", "nexus", "nexus", "nexus", "audio/nexus", "audio/nexus" );
	fileTypes_[ 0 ] = nexusType;
    
    CCoord itemHeight = 20;

    CRect rcLeftPanel( size.left, size.top, size.right-268, size.bottom );
    CRect rcRightPanel( size.right-268, size.top, size.right, size.bottom );
    CRect rcLabelBankPath( 20, 20, 560, 20+fontArial11->getSize() + 8 );
    CRect rcGrid( 20, 55, size.right-291, 55+itemHeight*16 );

    CRect rcLabelCaption( 18, 0, 112, 18 );
    CRect rcLabelBankName( 18, 40, 62, 40+16 );
    CRect rcEditBankName( 18, 55, rcRightPanel.width()-8, 55+fontArial11->getSize() + 8 );
    CRect rcNew( 18, 100, 110, 100+24 );
    CRect rcOpen( 18, 132, 110, 132+24 );
    CRect rcSave( 18, 164, 110, 164+24 );
    CRect rcSaveAs( 18, 196, 110, 196+24 );
    CRect rcLabelAuto( 40, 238, 150, 238+18 );
    CRect rcEditAuto( 18, 238, 36, 238+18 );

    leftPanel_ = new CViewContainer( rcLeftPanel, pParentFrame );
    leftPanel_->setAutosizeFlags( kAutosizeAll );
    addView( leftPanel_ );

    rightPanel_ = new CViewContainer( rcRightPanel, pParentFrame );
    rightPanel_->setAutosizeFlags( kAutosizeRight|kAutosizeTop|kAutosizeBottom );
    rightPanel_->setTransparency( true );
    addView( rightPanel_ );
    
    lblBankPath_        = new Label( rcLabelBankPath, bank_->path_ );
	lblBankPath_->font_ = fontArial11;
	leftPanel_->addView( lblBankPath_ );
    
    gridHeader_          = new GridBoxHeader( 20 );
    gridHeader_->font_   = fontArial10;
    gridHeader_->hAlign_ = kLeftText;
    
    grid_ = new GridBox( rcGrid, getFrame(), this );
    grid_->setSelectionStyle( GridBox::MultipleSel );
    grid_->setItemHeight( itemHeight );
    grid_->setKeyListener( this );
    grid_->addColumn( 10, "#" );
    grid_->addColumn( 33, "Name" );
    grid_->addColumn( 33, "Category" );
    grid_->addColumn( 33, "Comment" );
    grid_->setColumnMetrics( editor_->getOptions()->columnMetrics_ );
    grid_->setSortInfo( editor_->getOptions()->sortColumn_, editor_->getOptions()->sortType_ );
    grid_->setAutosizeFlags( kAutosizeAll );
    grid_->addToContainer( leftPanel_, gridHeader_ );

    lblCaption_        = new Label( rcLabelCaption, "Bank" );
	lblCaption_->font_ = fontPirulen14;
	
	rightPanel_->addView( lblCaption_ );

    lblBankName_        = new Label( rcLabelBankName, "Name:" );
	lblBankName_->font_ = fontArial9;
	rightPanel_->addView( lblBankName_ );
        
    txtBankName_        = new TextEdit( rcEditBankName, this, CmdBankName, bank_->name_ );
    txtBankName_->font_ = fontArial11;
	rightPanel_->addView( txtBankName_ );

    btnNewBank_ = new KickButton( rcNew, "New", this, CmdNewBank );
    btnNewBank_->font_ = fontPirulen9;
    btnNewBank_->setWantsFocus( false );
	rightPanel_->addView( btnNewBank_ );

    btnOpenBank_ = new KickButton( rcOpen, "Open", this, CmdOpenBank );
    btnOpenBank_->font_ = fontPirulen9;
    btnOpenBank_->setWantsFocus( false );
	rightPanel_->addView( btnOpenBank_ );

    btnSaveBank_ = new KickButton( rcSave, "Save", this, CmdSaveBank );
    btnSaveBank_->font_ = fontPirulen9;
    btnSaveBank_->setWantsFocus( false );
	rightPanel_->addView( btnSaveBank_ );

    btnSaveBankAs_ = new KickButton( rcSaveAs, "Save As", this, CmdSaveBankAs );
    btnSaveBankAs_->font_ = fontPirulen9;
    btnSaveBankAs_->setWantsFocus( false );
	rightPanel_->addView( btnSaveBankAs_ );
 
	lblAutosave_        = new Label( rcLabelAuto, "Autosave" );
	lblAutosave_->font_ = fontArial9;
	rightPanel_->addView( lblAutosave_ );

    cbAuto_ = new Checkbox( rcEditAuto, this, CmdAutosave, editor_->getOptions()->autosave_ );
    rightPanel_->addView( cbAuto_ );

    setColors();
}


void BankView::setColors()
{
    leftPanel_->setBackgroundColor( colors.fill1 );
    gridHeader_->colBkgnd_ = colors.fill2;
    gridHeader_->colFrame_ = colors.fill1;
    gridHeader_->colText_  = colors.text3;
    grid_->setColors( colors.fill1, colors.fill2, colors.selection, colors.text1 );
	
    lblCaption_->colText_  = colors.text3;
    lblBankPath_->colText_ = colors.text2;
    lblBankName_->colText_ = colors.text3;
    lblAutosave_->colText_ = colors.text3;

    txtBankName_->setTextColors( colors.text3, colBlack, colors.text3, colBlack );
    txtBankName_->setBkgndColors( colTransparent, colWhite, colors.selection );
    txtBankName_->setFrameColors( colors.fill3, colors.fill3 );

    btnNewBank_->setColors( colors.control1, colors.control2, colors.control1, colors.selection );
    btnOpenBank_->setColors( colors.control1, colors.control2, colors.control1, colors.selection );
    btnSaveBank_->setColors( colors.control1, colors.control2, colors.control1, colors.selection );
    btnSaveBankAs_->setColors( colors.control1, colors.control2, colors.control1, colors.selection );

    cbAuto_->colTick_  = colors.control1;
    cbAuto_->colBkgnd_ = colors.control2;
}


bool BankView::attached( CView* parent )
{
    bool result = CViewContainer::attached( parent );

    update();
    getFrame()->setFocusView( grid_ );
    return result;
}


CMouseEventResult BankView::onMouseDown( CPoint& pos, const long& buttons )
{
    CView* mouseDownView = getViewAt( pos, true );
    if( mouseDownView == NULL ) {
        getFrame()->setFocusView( NULL );
    }
    CMouseEventResult result = CViewContainer::onMouseDown( pos, buttons );

    if( buttons & kRButton ) 
    {	
        handleMenu( pos );
    }
    return result;
}


long BankView::onKeyDown( VstKeyCode& keyCode )
{
    if( keyCode.virt == 0 && keyCode.modifier == MODIFIER_CONTROL )
    {
        switch( keyCode.character ) {
        case 99:  onClipboardCopy();                  return 1; // Ctrl-c
        case 118: onClipboardPaste( CPoint( -1,-1 ), CmdPaste ); return 1;    // Ctrl-v
        case 120: onClipboardCut();                   return 1;    // Ctrl-x
        default: return 0;
        }
    }
    else if( keyCode.virt )
    {
        switch( keyCode.virt ) 
        {
        case VKEY_DELETE: onDeleteSelection(); break;
        default: return 0;
        }
    }
    return 1;
}


void BankView::update()
{
    if( isAttached() == false ) 
        return;

    grid_->clear( true );

    for( UINT16 i=0; i<bank_->size(); i++ )
    {
        Program* program = bank_->getProgram( i );
        if( program ) {
            BankBoxItem* item = new BankBoxItem( i, program->name_, program->category_, program->comment_ );
            grid_->addItem( item );
        }
    }
    grid_->setIndex( bank_->programNum_, true );
    grid_->sort();

    lblBankPath_->setText( bank_->path_ );
    txtBankName_->setText( bank_->name_ );
    invalid();
}


bool BankView::programMessage( Editor::ProgramMessage msg, CView* sender )
{
    if( sender != this )
    {
        switch( msg )
        {
        case Editor::BankChanged:
            update();
            break;
        case Editor::ProgramEdited:
        {
            Program* program  = bank_->getCurrentProgram();
            INT32 programNum  = bank_->programNum_;
            INT32 index       = grid_->getPhysicalIndex( programNum );
            GridBoxItem* item = grid_->getItem( index );

            if( item && program ) {
                item->setText( BankBoxItem::ColName, program->name_ );
                item->setText( BankBoxItem::ColCategory, program->category_ );
                item->setText( BankBoxItem::ColComment, program->comment_ );
            }
            break;
        }
        case Editor::ProgramChanged:
        {
            INT32 index = grid_->getPhysicalIndex( bank_->programNum_ );
            grid_->setIndex( index, true );
            break;
        }
        }
    }
    return true;
}


void BankView::valueChanged( INT32 row, INT32 col )
{
    INT32 programNum = editor_->getProgramNumber();
    if( row != programNum ) 
    {
        synth_->setProgram( row );
    }

    if( col >= 0 )
    {
        INT32 index       = grid_->getPhysicalIndex( row );
        GridBoxItem* item = grid_->getItem( index );
        string text;
        item->getText( col, text );

        Program* program = bank_->getProgram( row );
        ASSERT( program );
        if( program )
        {
            switch( col ) {
            case BankBoxItem::ColName:     program->name_     = text; break;
            case BankBoxItem::ColCategory: program->category_ = text; break;
            case BankBoxItem::ColComment:  program->comment_  = text; break;
            }

            bank_->saveProgram( row, program );
            if( row == bank_->programNum_ ) {
                editor_->programMessage( Editor::ProgramEdited, this );
            }
        }
    }
}


void BankView::valueChanged( CControl* pControl )
{
    long tag    = pControl->getTag();
    float value = pControl->getValue();

    TextEdit* textEdit;

    if( pControl->isTypeOf( "TextEdit" ) || pControl->isTypeOf( "TextEdit" ))
        textEdit = dynamic_cast< TextEdit* >( pControl );

    switch( tag ) 
    {
    case CmdBankName: bank_->name_ = textEdit->getText(); break;
    case CmdAutosave: editor_->getOptions()->autosave_ = value == 1.0f ? true : false; break;

    case CmdNewBank:
        if( value == 0) onNewBank();
        break;
    case CmdOpenBank: 
        if( value == 0 ) onOpenBank(); 
        break;
    case CmdSaveBank: 
        if( value == 0 ) onSaveBank();
        break;
    case CmdSaveBankAs: 
        if( value == 0 ) onSaveBankAs();
        break;
    case CmdLoadProgram:
        valueChanged( (INT32)value, -1 );
        break;
    }
}


void BankView::onNewBank()
{
    VstFileSelect fileSelect;
    initFileSelect( kVstFileSave, bank_->path_, "New Bank", fileSelect );

	CFileSelector selector( editor_->getMainFrameHandle() );
	if (selector.run( &fileSelect ))
	{
        editor_->getChildFrame()->redraw();   // instantly redraw area covered by fileselector
        setCursor( CursorWait );
        editor_->getSynth()->newBank( fileSelect.returnPath, "New Bank", true );
        setCursor( CursorDefault );
	}
    getFrame()->setFocusView( grid_ );
    getFrame()->invalid();

	delete[] fileSelect.returnPath;
    delete[] fileSelect.initialPath;
}


void BankView::onOpenBank()
{
    VstFileSelect fileSelect;
    initFileSelect( kVstFileLoad, bank_->path_, "Open Bank", fileSelect );
    ChildFrame* frame = editor_->getChildFrame();

	CFileSelector selector( editor_->getMainFrameHandle() );
	if (selector.run( &fileSelect ))
	{
        if( isAttached() ) {
            grid_->clear( false );
            grid_->invalid(); 
        }
        frame->redraw();   // instantly redraw area covered by fileselector

        setCursor( CursorWait );
        editor_->getSynth()->loadBank( fileSelect.returnPath );
        setCursor( CursorDefault );
	}
    if( isAttached() ) 
        frame->setFocusView( grid_ );
    frame->invalid();
	delete[] fileSelect.returnPath;
    delete[] fileSelect.initialPath;
}


void BankView::onSaveBank()
{
    setCursor( CursorWait );
    editor_->saveBank();
    setCursor( CursorDefault );
    update();
}


void BankView::onSaveBankAs()
{
    VstFileSelect fileSelect;
    initFileSelect( kVstFileSave, bank_->path_, "Save Bank", fileSelect );

	CFileSelector selector( editor_->getMainFrameHandle() );
	if (selector.run( &fileSelect ))
	{
        editor_->getChildFrame()->redraw();   // instantly redraw area covered by fileselector

        struct stat fileInfo; 
        int fileTest  = stat( fileSelect.returnPath, &fileInfo ); 
        int overwrite = IDYES;

        if( fileTest == 0 ) 
        { 
	        overwrite = MessageBox( 
                (HWND)editor_->getMainFrameHandle(), 
                L"File already exists. Do you want to overwrite it?",
                L"Question",
                MB_YESNO | MB_ICONQUESTION );
        }
        if( overwrite == IDYES )
        {
            setCursor( CursorWait );
            editor_->saveBank( fileSelect.returnPath );
            setCursor( CursorDefault );
            update();
        }
	}
    getFrame()->setFocusView( grid_ );
    getFrame()->invalid();

	delete[] fileSelect.returnPath;
    delete[] fileSelect.initialPath;
}


void BankView::initFileSelect( INT32 cmd, const string& initialPath, const string& label, VstFileSelect& fileSelect )
{
    char* cInitialPath = new char[ 1024 ];
    strcpy_s( cInitialPath, 1024, initialPath.c_str() );
    
 	memset( &fileSelect, 0, sizeof( VstFileSelect ));
    strcpy( fileSelect.title, label.c_str() );

	fileSelect.command     = cmd;
	fileSelect.type        = kVstFileType;
	fileSelect.nbFileTypes = 1;
	fileSelect.fileTypes   = (VstFileType*)&fileTypes_; //(VstFileType*)&types;
	fileSelect.returnPath  = new char[ 1024 ];
	fileSelect.initialPath = cInitialPath;
	fileSelect.future[0]   = 1;	// utf-8 path on macosx
}


void BankView::handleMenu( const CPoint& pos )
{
    CView* view = getViewAt( pos, false );
    if( view != leftPanel_ ) {
        return;
    }
    
    try {
        COptionMenu* menu = createMenu( pos );
	    menu->takeFocus();

	    long idx;
	    COptionMenu* result = menu->getLastItemMenu( idx );
	    if( result ) 
        {
            CMenuItemEx* item = dynamic_cast< CMenuItemEx* >( result->getCurrent() );
            
            switch( item->tag_ ) 
            {
            case CmdNewProgramBefore:
            case CmdNewProgramOver:  
            case CmdNewProgramAfter:      onNewProgram( pos, item->tag_ ); break;
            case CmdSaveProgramBefore:
            case CmdSaveProgramAfter:     onSaveProgram( pos, item->tag_ );  break;
            case CmdDeleteSelection:      onDeleteSelection();     break;
            case CmdCopy:                 onClipboardCopy();       break;
            case CmdCut:                  onClipboardCut();        break;
            case CmdPasteBefore:     
            case CmdPasteOver:     
            case CmdPasteAfter:           onClipboardPaste( pos, item->tag_ ); break;
            }
	    }
	    pParentFrame->removeView( menu );
        pParentFrame->setFocusView( grid_ );
    }
    catch( const exception& e ) {
        TRACE( e.what() );
    }
}


COptionMenu* BankView::createMenu( const CPoint& pos )
{
    CPoint menuPos( pos );
	localToFrame( menuPos );
	COptionMenu* menu = new COptionMenu( CRect( menuPos, CPoint( 0, 0 )), NULL, 0 );
    COptionMenu* submenu = NULL;

    submenu = new COptionMenu( CRect( 0,0,0,0 ), NULL, CmdNewProgram );
    submenu->addEntry( new CMenuItemEx( "Before", CmdNewProgramBefore ));
    submenu->addEntry( new CMenuItemEx( "Overwrite", CmdNewProgramOver ));
    submenu->addEntry( new CMenuItemEx( "After", CmdNewProgramAfter ));
    menu->addEntry( submenu, "New Program" );

    submenu = new COptionMenu( CRect( 0,0,0,0 ), NULL, CmdSaveProgram );
    submenu->addEntry( new CMenuItemEx( "Insert Before", CmdSaveProgramBefore ));
    submenu->addEntry( new CMenuItemEx( "Insert After", CmdSaveProgramAfter ));
    menu->addEntry( submenu, "Duplicate Current" );

    menu->addSeparator();
    menu->addEntry( new CMenuItemEx( "Copy Selection", CmdCopy ));
    menu->addEntry( new CMenuItemEx( "Cut Selection", CmdCut ));

    submenu = new COptionMenu( CRect( 0,0,0,0 ), NULL, CmdPaste );
    long flags = editor_->isTextAvailable() ? CMenuItem::kNoFlags : CMenuItem::kDisabled;
    submenu->addEntry( new CMenuItemEx( "Before",    CmdPasteBefore, flags ));
    submenu->addEntry( new CMenuItemEx( "Overwrite", CmdPasteOver,   flags ));
    submenu->addEntry( new CMenuItemEx( "After",     CmdPasteAfter,  flags ));
    menu->addEntry( submenu, "Paste" );

    menu->addSeparator();
    menu->addEntry( new CMenuItemEx( "Delete Selection", CmdDeleteSelection ));
    
	getFrame()->addView( menu );
    return menu;
}


void BankView::onNewProgram( const CPoint& pos, UINT16 tag )
{
    INT32 index      = calcInsertPosition( pos, tag );
    Program* program = Bank::generateDefaultProgram( index );

    if( tag == CmdNewProgramOver ) 
    {
        if( index == bank_->programNum_ )       // synth has to be suspended if we delete the current program
        {
            synth_->suspend();
            bank_->removeProgram( index, true );
            bank_->insertProgram( index, program );
            bank_->setCurrentProgram( program );
            synth_->setProgram( index );
        }
        else {
            bank_->removeProgram( index, true );
            bank_->insertProgram( index, program );
        }
    }
    else {
        bank_->insertProgram( index, program );
    }
    editor_->programMessage( Editor::BankEdited, this );
    update();
}


void BankView::onSaveProgram( const CPoint& pos, UINT16 tag )
{
    INT32 index = calcInsertPosition( pos, tag );

    bank_->saveCurrentProgram( index ); 
    editor_->programMessage( Editor::BankEdited, this );
    update();
}


void BankView::onDeleteSelection()
{
    const SelectionSet& selection = grid_->getSelection();
    INT32 index                   = grid_->getLogicalIndex();
    INT32 survivor                = -1;                         // find a slot in the bank that will not be deleted
                                                                
    for( INT32 i=index+1; i<(INT32)bank_->size(); i++ )         // search upward
    {
        SelectionSet::iterator pos = selection.find( i );
        if( pos == selection.end() ) {
            survivor = i;
            break;
        }
    }
    if( survivor == -1 )                                         // no slot found, search downward
    {
        for( INT32 i=index-1; i>=0; i--)
        {
            SelectionSet::iterator pos = selection.find( i );
            if( pos == selection.end() ) {
                survivor = i;
                break;
            }
        }
    }
    if( survivor == -1 )                                         // again no slot, this means everything will be deleted
    {
        editor_->getSynth()->newBank( bank_->path_, bank_->name_, false );
    }
    else                                                         // remove all selected programs
    {
        editor_->getSynth()->setProgram( survivor );

        for( SelectionSet::reverse_iterator it = selection.rbegin(); it!=selection.rend(); it++ ) {
            bank_->removeProgram( *it );
        }
    }
    grid_->clearSelection();
    editor_->programMessage( Editor::BankEdited, this );
    update();
}


void BankView::onClipboardCopy()
{
    ostringstream xml;
    const SelectionSet& selection = grid_->getSelection();

    for( SelectionSet::iterator it = selection.begin(); it!=selection.end(); it++ )
    {
        Program* program = bank_->getProgram( *it );
        Bank::getXml( program, xml );
        xml << endl;
    }
    editor_->copyText( xml );
}


void BankView::onClipboardCut()
{
    onClipboardCopy();
    onDeleteSelection();
}


void BankView::onClipboardPaste( const CPoint& pos, UINT16 tag )
{
    istringstream xml;
    bool result = editor_->pasteText( xml );

    if( result )
    {
        ProgramList list;
        Bank::createFromXml( xml, list );

        INT32 index = calcInsertPosition( pos, tag );  
        if( tag == CmdPasteOver ) {
            bank_->removeProgram( index, true );
        }

        for( UINT32 i=0; i<list.size(); i++ )
        {
            Program* program = list.at( i );
            bank_->insertProgram( index++, program );
        }
        editor_->programMessage( Editor::BankEdited, this );
        update();
    }
}


void BankView::setCursor( CursorType type )
{
    ChildFrame* childFrame = dynamic_cast< ChildFrame* >( pParentFrame );
    if( childFrame ) {
        childFrame->setCursor( type );
    }
}


void BankView::getColumnMetrics( string& metrics )
{
    return grid_->getColumnMetrics( metrics );
}


void BankView::getSortInfo( INT32& sortColumn, INT32& sortType )
{
    const SortInfo& info = grid_->getSortInfo();
    sortColumn           = info.columnId_;
    sortType             = info.type_;
}


bool BankView::onDrop( CDragContainer* drag, const CPoint& pos )
{
    long size, type;
    string filename;
    setCursor( CursorWait );
    for( void* ptr = drag->first( size, type ); ptr != NULL; ptr = drag->next( size, type ))
    {
        filename = (char*)ptr;
        bank_->load( filename, true );
    }
    update();
    setCursor( CursorDefault );
    return true;
}


INT32 BankView::calcInsertPosition( const CPoint& pos, UINT16 tag )
{
    INT32 index = grid_->getLogicalIndex( pos );

    switch( tag ) 
    {
    case CmdNewProgramBefore:
    case CmdSaveProgramBefore:
    case CmdPasteBefore: break;

    case CmdNewProgramOver:
    case CmdPasteOver: break;

    case CmdNewProgramAfter:
    case CmdSaveProgramAfter:
    case CmdPaste:
    case CmdPasteAfter: index++; break;
    }
    return min( (INT32)bank_->size(), max( 0, index ));
}

