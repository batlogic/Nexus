
#include "PrefsDialog.h"
#include "TextEdit.h"
#include "Images.h"
#include "Fonts.h"
#include "SelectBox.h"



PrefsDialog::PrefsDialog( CView* owner, Editor* editor ) : 
    ModalDialog( owner )
{
    AppOptions* options = editor->getOptions();
    tuning_ = options->tuning_;
    width_  = options->windowRight_-options->windowLeft_;
    height_ = options->windowBottom_-options->windowTop_;
    
    CBitmap* imgBkgnd     = images.get( Images::bkgndPrefsDialog );
    CBitmap* imgBtnOk     = images.get( Images::btnOk );
    CBitmap* imgBtnCancel = images.get( Images::btnCancel );

    setTransparency( true );
    setBackground( imgBkgnd );
    size( 0, 0, imgBkgnd->getWidth(), imgBkgnd->getHeight() );

    CCoord textHeight = fontArial11->getSize() + 6;
    rcBtnOk_( 190, 180, 190+imgBtnOk->getWidth(), 180+imgBtnOk->getHeight()/2 );
    rcBtnCancel_( 215, 180, 215+imgBtnCancel->getWidth(), 180+imgBtnCancel->getHeight()/2 );
    CRect rcTitle( 5, 5, 200, 15 );
    CRect rcLabelTuning( 10, 40, 100, 58 );
    CRect rcLabelTuningUnit( 165, 40, 245, 58 );
    CRect rcEditTuning( 60, 40, 160, 58 );
    CRect rcLabelSkin( 10, 76, 100, 94 );
    CRect rcEditSkin( 60, 76, 160, 94 );
    CRect rcLabelWidth( 10, 122, 100, 122+textHeight );
    CRect rcEditWidth( 60, 122, 160, 122+textHeight );
    CRect rcLabelHeight( 10, 148, 100, 148+textHeight );
    CRect rcEditHeight( 60, 148, 160, 148+textHeight );

    CKickButton* button;
    Label* label;
    NumericEdit* numEdit;

    button = new CKickButton( rcBtnOk_, this, CmdCommit, imgBtnOk->getHeight()/2, imgBtnOk );
    button->setWantsFocus( false );
	addView( button );
	
    button = new CKickButton( rcBtnCancel_, this, CmdCancel, imgBtnCancel->getHeight()/2, imgBtnCancel );
    button->setWantsFocus( false );
	addView( button );
        
    label           = new Label( rcTitle, "Preferences" );
	label->font_    = fontPirulen9;
	label->colText_ = colors.dlgText;
	addView( label );

    label           = new Label( rcLabelTuning, "Tune:" );
	label->font_    = fontArial11;
	label->colText_ = colors.dlgText;
	addView( label );

    label           = new Label( rcLabelTuningUnit, "(+- cent)" );
	label->font_    = fontArial11;
	label->colText_ = colors.dlgText;
	addView( label );

    numEdit            = new NumericEdit( rcEditTuning, this, CmdTuning, tuning_, NumericEdit::Integer );
    numEdit->style_    = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
    numEdit->minValue_ = -1200;
    numEdit->maxValue_ = 1200;
    numEdit->format_   = NumericEdit::Integer;
    numEdit->font_     = fontLucida11;
    numEdit->setBkgndColors( colors.dlgTextBkgnd, colWhite, colors.selection );
    numEdit->setTextColors( colors.dlgText, colBlack, colors.dlgText, colBlack );
	addView( numEdit );

    label           = new Label( rcLabelSkin, "Skin:" );
	label->font_    = fontArial11;
	label->colText_ = colors.dlgText;
	addView( label );

    sbSkin_ = new SelectBox( rcEditSkin, this, CmdSkin );
    sbSkin_->setFont( fontArial11 );
    sbSkin_->setTextColor( colors.dlgText );
    sbSkin_->setBackgroundColor( colors.dlgTextBkgnd );
    sbSkin_->setFrameColor( colors.dlgText );
    sbSkin_->setSelectionColor( colors.selection );

    SelectBoxList list;
    UINT16 index = 0;
    for( UINT16 i=0; i<gColorSchemes.size(); i++ ) 
    {
        string name = gColorSchemes[i].name_;
        list.push_back( name );
        if( name == options->colorScheme_ ) {
            index = i;
            colorScheme_ = name;
        }
    }
    sbSkin_->setList( list, 5 );
    sbSkin_->setIndex( index );

    if( editor->isStandalone() == false )
    {
        label              = new Label( rcLabelWidth, "Width:" );
	    label->font_       = fontArial11;
	    label->colText_    = colors.dlgText;
	    addView( label );

        numEdit            = new NumericEdit( rcEditWidth, this, CmdWidth, (float)width_, NumericEdit::Integer );
        numEdit->font_     = fontLucida11;
        numEdit->style_    = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
        numEdit->minValue_ = 640;
        numEdit->maxValue_ = 1400;
        numEdit->setBkgndColors( colors.dlgTextBkgnd, colWhite, colors.selection );
        numEdit->setTextColors( colors.dlgText, colBlack, colors.dlgText, colBlack );
        addView( numEdit );

        label              = new Label( rcLabelHeight, "Height:" );
	    label->font_       = fontArial11;
	    label->colText_    = colors.dlgText;
	    addView( label );

        numEdit            = new NumericEdit( rcEditHeight, this, CmdHeight, (float)height_, NumericEdit::Integer );
        numEdit->font_     = fontLucida11;
        numEdit->style_    = TextEdit::SelectOnOpen | TextEdit::NeedsReturn | TextEdit::LooseFocusSaves;
        numEdit->minValue_ = 480;
        numEdit->maxValue_ = 1000;
        numEdit->setBkgndColors( colors.dlgTextBkgnd, colWhite, colors.selection );
        numEdit->setTextColors( colors.dlgText, colBlack, colors.dlgText, colBlack );
	    addView( numEdit );
    }
    addView( sbSkin_ );
}


void PrefsDialog::valueChanged( CControl *pControl )
{
    UINT16 cmd = (Command)pControl->getTag();
    float value = pControl->getValue();

    switch( cmd ) 
    {
    case CmdTuning: tuning_ = (INT16)value; break;
    case CmdSkin:   sbSkin_->getText( (UINT16)value, colorScheme_ ); break;
    case CmdWidth:  width_  = (CCoord)value; break; 
    case CmdHeight: height_ = (CCoord)value; break;
    default: ModalDialog::valueChanged( pControl );
    }
}


void PrefsDialog::drawBackgroundRect( CDrawContext* pContext, CRect& rcUpdate )
{
    ModalDialog::drawBackgroundRect( pContext, rcUpdate );

    CRect rcClient( 0, 18, size.width()-10, size.height()-10 );
    CRect rcCaption( 0, 0, rcClient.right, 18 );

    pContext->setFillColor( colors.dlgFill1 );
    pContext->drawRect( rcClient, kDrawFilled );

    pContext->setLineWidth( 1 );
    pContext->setFrameColor( colors.dlgFill1 );
    pContext->setFillColor( colors.dlgCaption );
    pContext->drawRect( rcCaption, kDrawFilledAndStroked );

    pContext->setFillColor( colors.dlgFill2 );
    pContext->drawRect( rcBtnOk_, kDrawFilled );
    pContext->drawRect( rcBtnCancel_, kDrawFilled );

    setDirty( false );
}


