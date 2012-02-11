
#ifndef GRIDBOX_H
#define GRIDBOX_H

#include "Editor.h"
#include "../VST/cscrollview.h"


class GridBox;
class GridBoxItem;
class GridBoxListener;


struct ColumnInfo
{
    ColumnInfo() {}
    ColumnInfo( CCoord left, CCoord right, float ratio, const string& caption ) :
        left_( left ),
        right_( right ),
        ratio_( ratio ),
        caption_( caption )
        {}

    CCoord left_, right_;
    float ratio_;
    string caption_;
};


class SortInfo
{
public:
    SortInfo();

    void toggle( INT32 columnId );

    enum Type { None, Up, Dn };
    INT32 columnId_;
    Type type_;

    template <class T>
    bool compare( T first, T second ) const
    {
        bool result = false;

        if( type_ == SortInfo::Up )
        {
            if( first < second ) return true; 
            if( first > second ) return false; 
        }
        else {
            if( first > second ) return true;
            if( first < second ) return false;
        }
        return result;
    }
};


typedef set< INT32 > SelectionSet;
typedef vector< GridBoxItem* > GridBoxItemList;
typedef vector< ColumnInfo > GridBoxColumnList;


class GridBoxListener
{
public:
    virtual void valueChanged( INT32 row, INT32 col ) = 0;
};


class GridBoxItem : public CViewContainer, public CControlListener
{
friend class GridBox;

public:
    GridBoxItem();

    virtual void addControl( CControl* control );
    UINT32 getRowId()   { return rowId_; }

    virtual float getValue( INT32 columnId ) const { return 0.0f; }
    virtual void getText( INT32 columnId, string& text ) const {}
    virtual void setText( INT32 columnId, const string& text ) {}
    virtual bool isLess( GridBoxItem* ref ) const { return rowId_ < ref->rowId_; }
    virtual void setColors() {}

    void drawRect( CDrawContext* pContext, const CRect& rcUpdate );

    CLASS_METHODS( GridBoxItem, CViewContainer )

protected:
    virtual void init( GridBox* owner, UINT32 rowId );
    virtual void layout( const CRect& rcSize );
    virtual void setState( bool selected, bool focused );
    virtual void valueChanged( CControl* pControl );

    GridBox* owner_;
    UINT32 rowId_;
};


class GridBoxHeader : public CView
{
friend class GridBox;

public: 
    GridBoxHeader( CCoord height );
    void init( const CRect& rcSize, GridBox* owner );

    CMouseEventResult onMouseExited( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseUp( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseMoved( CPoint& pos, const long& buttons );
    void draw( CDrawContext* pContext );

    CColor colText_, colBkgnd_, colFrame_;
    CFontRef font_;
    CHoriTxtAlign hAlign_;

protected:
    INT32 getTracker( const CPoint& pos );
    INT32 getColumnId( const CPoint& pos );
    void setCursor( CursorType type );

    GridBox* owner_;
    INT32 tracker_;
    GridBoxColumnList* columns_;
};


class GridBox : public CViewContainer, public CControlListener
{
friend class GridBoxItem;
friend class GridBoxHeader;

public:
    GridBox( const CRect& rcSize, CFrame* frame, GridBoxListener* listener );
    ~GridBox();

    enum SelStyle { NoSel, SingleSel, MultipleSel };

    void setHeader( GridBoxHeader* header );
    void addColumn( INT16 percent, const string& caption="" );
    void setColumnMetrics( const string& metrics );
    void getColumnMetrics( string& metrics );

    void addItem( GridBoxItem* item );
    void clear( const bool preserveSelection );
    void sort();
    const SortInfo& getSortInfo() const { return sortInfo_; }
    void setSortInfo( INT32 columnId, INT32 type );

    void valueChanged( CControl* pControl );

    UINT32 scrollTo( INT32 index, bool toTop );
    void layout();
    void addToContainer( CViewContainer* parent, GridBoxHeader* header );

    void setViewSize( CRect& rcSize, bool invalid );
    void setItemHeight( CCoord height );
    void setColors( const CColor& colBkgnd, const CColor colFrame, const CColor& colSel, const CColor& colFocus );
    void setSelectionStyle( SelStyle selStyle );
    void setKeyListener( CView* keyListener )   { keyListener_ = keyListener; }

    void setIndex( INT32 index, bool scroll );
    INT32 getIndex() const   { return index_; }
    INT32 getPhysicalIndex( INT32 rowId );
    INT32 getLogicalIndex();
    INT32 getLogicalIndex( INT32 index );
    INT32 getLogicalIndex( const CPoint& pos );
    bool isInLowerHalfOfRow( const CPoint& pos );
    GridBoxItem* getItem( UINT32 rowId );
    INT32 end() { return -1; }
    
    const GridBoxColumnList& getColumns()   { return columns_; }
    const SelectionSet& getSelection();
    void clearSelection();
    void selectAll();
    
    CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );
    CMouseEventResult onMouseMoved( CPoint& pos, const long& buttons );
    bool onWheel( const CPoint& pos, const CMouseWheelAxis& axis, const float& distance, const long& buttons );
    long onKeyDown( VstKeyCode& keyCode );
    
    void drawRect( CDrawContext* pContext, const CRect& updateRect );

    enum Message { Delete, Copy, Cut, Paste };
    
    CLASS_METHODS( GridBox, CViewContainer )
    
protected:
    void valueChanged( INT32 row, INT32 col );
    void resizeColumns( const CRect& rcSize );
    void checkScrollbar();
    INT32 calcIndexFromPosition( const CPoint& pos );
    void select( INT32 index, bool ctrl, bool shift );
    void toggleSelection( INT32 index );
    void removeFromSelection( INT32 index );
    bool isSelected( INT32 index );
    void setItemStates();

    static bool compare( GridBoxItem* first, GridBoxItem* second ) { return first->isLess( second ); }
    
    GridBoxListener* listener_;
    GridBoxColumnList columns_;
    CScrollbar* scrollbar_;
    GridBoxHeader* header_;
    CView* keyListener_;
    
    CColor colBkgnd_, colFrame_, colSel_, colFocus_;
    CRect rcItems_, rcHeader_, rcScrollbar_, rcScrollsize_;
    CCoord itemHeight_, scrollbarWidth_;
    INT32 numItems_, maxVisible_, index_, scrollOffset_;
    SelStyle selStyle_;
    SortInfo sortInfo_;

    SelectionSet selection_, logicalSelection_;
};



#endif // GRIDBOX_H