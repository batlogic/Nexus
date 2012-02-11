
#ifndef BANKVIEW_H
#define BANKVIEW_H

#include "Editor.h"
#include "GridBox.h"


class Synth;
class TextEdit;
class Label;
class Checkbox;
class KickButton;



class BankBoxItem : public GridBoxItem
{
public:
    BankBoxItem( INT32 programNum, const string& name, const string& category, const string& comment );

    void getText( INT32 columnId, string& text ) const;
    void setText( INT32 columnId, const string& text );
    float getValue( INT32 columnId ) const;
    bool isLess( GridBoxItem* ref ) const;
    void setColors();

    enum GridCol { ColNumber, ColName, ColCategory, ColComment };

protected:
    TextEdit *txtName_, *txtCategory_, *txtComment_;
    Label* lblProgramNum_;
    INT32 programNum_;
};



class BankView : public CViewContainer, public GridBoxListener, public CControlListener
{
public:
	BankView( const CRect& size, Editor* editor );

    CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );
    long onKeyDown( VstKeyCode& keyCode );

    bool attached( CView* parent );
    void setColors();
    void valueChanged( INT32 row, INT32 col );
    void valueChanged( CControl* pControl );

    void getColumnMetrics( string& metrics );
    void getSortInfo( INT32& sortColumn, INT32& sortType );
    bool onDrop( CDragContainer* drag, const CPoint& pos );
    void onSaveBank();
    void onOpenBank();
    void update();
    bool programMessage( Editor::ProgramMessage msg, CView* sender );

    CLASS_METHODS( BankView, CViewContainer )

    enum Command { CmdBankName, CmdAutosave, CmdNewBank, CmdOpenBank, CmdSaveBank, CmdSaveBankAs, 
                   CmdNewProgram, CmdNewProgramBefore, CmdNewProgramOver, CmdNewProgramAfter,
                   CmdSaveProgram, CmdSaveProgramBefore, CmdSaveProgramAfter,
                   CmdLoadProgram, CmdDeleteSelection, 
                   CmdCopy, CmdCut, CmdPaste, CmdPasteBefore, CmdPasteOver, CmdPasteAfter 
    };

protected:
    void handleMenu( const CPoint& pos );
    COptionMenu* createMenu( const CPoint& pos );
    void setCursor( CursorType type );
    INT32 calcInsertPosition( const CPoint& pos, UINT16 tag );

    void onNewBank();
    void onSaveBankAs();
    void initFileSelect( INT32 cmd, const string& initialPath, const string& label, VstFileSelect& fileSelect );

    void onNewProgram( const CPoint& pos, UINT16 tag );
    void onSaveProgram( const CPoint& pos, UINT16 tag );
    void onDeleteSelection();
    void onClipboardCopy();
    void onClipboardCut();
    void onClipboardPaste( const CPoint& pos, UINT16 tag );

    Editor* editor_;
    Synth* synth_;
    Bank* bank_;
    Program* program_;
    GridBox* grid_;
    GridBoxHeader* gridHeader_;
    CViewContainer *leftPanel_, *rightPanel_;
    Label *lblBankPath_, *lblCaption_, *lblBankName_, *lblAutosave_;
    TextEdit* txtBankName_;
    KickButton *btnNewBank_, *btnOpenBank_, *btnSaveBank_, *btnSaveBankAs_;
    Checkbox* cbAuto_;
    VstFileType fileTypes_[ 1 ];
};



#endif // BANKVIEW_H