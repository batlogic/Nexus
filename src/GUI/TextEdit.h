
#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include "Editor.h"
#include "Widgets.h"



//--------------------------------------------------
// class TextEdit
//--------------------------------------------------

class TextEdit : public CControl
{
public:
    enum Style { SelectOnOpen=1, NeedsReturn=2, LooseFocusSaves=4 };
    enum OpenOn { SingleClick, DblClick, CtrlClick };
    enum State { Idle, Editing };

	TextEdit( CRect &rcSize, CControlListener* listener, long tag, string text="" );
	
    CMouseEventResult onMouseEntered( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseExited( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseMoved( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseUp( CPoint& pos, const long& buttons );
    long onKeyDown( VstKeyCode& keyCode );

    void draw( CDrawContext* pContext );
    void takeFocus();
    void looseFocus();
    CPoint& localToFrame( CPoint& pos ) const;
    void setState( State state );
    bool isEditing()    { return state_ == Editing; }
    bool isSelected();
    bool isFocused();

    void setText( const string& text );
    string& getText()                   { return text_; }
    void setBkgndColors( CColor colIdle, CColor colEdit, CColor colSel );
    void setFrameColors( CColor colIdle, CColor colEdit );
    void setTextColors( CColor colIdle, CColor colEdit, CColor colSel, CColor colFocus );
    void setViewSize( CRect& rcSize, bool invalid );
    void parentSizeChanged();
    
    CColor colBkgndIdle_, colBkgndEdit_, colBkgndSel_;
    CColor colTextIdle_, colTextEdit_, colTextSel_, colTextFocus_;
    CColor colFrameIdle_, colFrameEdit_;
	CHoriTxtAlign hAlign_;
	CFontDesc* font_;
    UINT16 maxLength_;
    CPoint indent_;
    UINT16 style_;
    OpenOn openOn_;
    void* data_;
    
    CLASS_METHODS( TextEdit, CControl )

protected:
    State state_;
    INT16 position_;
    INT16 scrollOffset_;
    CCoord caretPosition_;
    string text_, displayText_, savedText_;
    Caret caret_;
    CRect rcSelection_;
    INT16 selectionFirst_;
    INT16 selectionLast_;
    bool selecting_;
    bool mouseSelecting_;
    CCoord anchor_;
    bool inMenu_;
    enum { CmdCopy, CmdCut, CmdPaste, CmdDeleteSel, CmdSelectAll };
        
    void setInsertPosition( INT16 spot, bool scroll=true );
    void setSelection( bool select );
    void addChar( int code );
    void deleteSelectionOrChar( bool backspace );
    void deleteSelection();
    void deleteAll();
    void scrollText();
    CCoord findPosition( string& text, INT16 position, INT16& offset );
    UINT16 translateCoord( const string& text, CCoord pos );
    CCoord getCoord( const string& text );
    void setCursor( CursorType type );
    virtual void setEditState();
    virtual void setIdleState();
    void handleMenu( const CPoint& pos );
    COptionMenu* createMenu( const CPoint& pos );

    virtual void drawText( CDrawContext* pContext );

    virtual bool validate( int code );
    virtual bool validate( const string& s );
    virtual void insert( size_t pos, int c );
    virtual void insert( size_t pos, const string& text );
    virtual void erase( size_t pos, size_t len );
    virtual void commit( bool commit );
    virtual void notify( bool force=false );
    
    void beginSelecting();
    void continueSelecting();
    void endSelecting();
    void selectAll();
    void unselectAll();
    
    void beginMouseSelecting( const CPoint& pos );
    void continueMouseSelecting( const CPoint& pos );
    void endMouseSelecting();
    
    void normalizeSelection( INT16& first, INT16& last );
    string getSelectedString();

    void clipboardCopy( const string& s );
    void clipboardCopy();
    void clipboardPaste();
    void clipboardCut();
};


//-------------------------------------------
// class NumericEdit
//-------------------------------------------


class NumericEdit : public TextEdit
{
public:
    enum NumberFormat { 
	    Integer = 0,
	    Float   = 1,
	    Decibel = 2,
    };

    NumericEdit( CRect &rcSize, CControlListener* listener, long tag, float value, NumberFormat format=Float );

    CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseMoved( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseUp( CPoint& pos, const long& buttons );

    bool validate( int code );
    float getValue() const;
    void setValue( float val );

    CLASS_METHODS( NumericEdit, CControl )

    float value_;
    float minValue_;
    float maxValue_;
    NumberFormat format_;
    int precision_;
    string unit_;
    
protected:
    void insert( size_t pos, int c );
    void insert( size_t pos, const string& text );
    void erase( size_t pos, size_t len );
    void commit( bool commit );
    bool getValue( float& value, const string& text) const;
    void toString( FLOAT value, string& result, UINT16 precision=0  ) const;
    void dbToString( FLOAT value, string& result, UINT16 precision ) const;
    void trackValue( const CPoint& pos );
    void setMouseCapture( CView* view );
    void setIdleState();
    void drawText( CDrawContext* pContext );
    
    template <class T>
    bool fromString( T& value, const string& text ) const
    {
      istringstream iss( text );
      return ( iss >> dec >> value ).fail() == false;
    }
    bool fromDecibel( float& value, const string& text ) const;

    bool tracking_;
    float trackInc_;
    CCoord trackLast_;
};




#endif TEXTEDIT_H