
#ifndef PROGRAMVIEW_H
#define PROGRAMVIEW_H

#include "Editor.h"
#include "Wire.h"
#include "Widgets.h"

class ModuleBox;
class Program;
class ModuleData;
class CtrlView;
class ModulePort;
class Wire;



typedef map< UINT16, ModuleBox* > ModuleBoxMap;
typedef vector< ModuleBox* > ModuleBoxList;


class ProgramView : public CViewContainer
{
public:
	ProgramView( const CRect& size, Editor* editor, CtrlView* ctrlView );
	
	long onKeyDown( VstKeyCode& keyCode );
    CMouseEventResult onMouseDown( CPoint& pos, const long& buttons );
	CMouseEventResult onMouseUp( CPoint& pos, const long& buttons );
	CMouseEventResult onMouseMoved( CPoint& pos, const long& buttons );

	void setColors();
    bool attached( CView* parent );
    void drawBackgroundRect( CDrawContext *pContext, CRect& rcUpdate );
    
	bool programMessage( Editor::ProgramMessage msg, CView* sender ); 
	bool removeAll (const bool& withForget = true)  { mouseDownView = NULL; return CViewContainer::removeAll( withForget ); }

    void setBoxLabel( UINT16 id, const string& label );
    void setBoxPolyphony( UINT16 id );
	
    ModuleBox* getBox( UINT16 index );
    ModulePort* getPort( const LinkInfo& linkInfo, PortAlignment portAlign );
    void getDockPosition( UINT16 moduleId, UINT16 portId, PortAlignment portAlign, CPoint& pos );
    Wire* getWire( const LinkInfo& linkInfo, PortAlignment portAlign );
    void onBoxResized( ModuleBox* box );
    void setPreferredSize( const CRect& rcSize );
    ScrollView* getScrollView()  { return scrollView_; }

	CLASS_METHODS( ProgramView, CViewContainer )

protected:
    struct SelectionData
    {
        SelectionData() {
            clear();
        }

        void clear();
        void getBoundingRect( CRect& rcUnion );

        ModuleBox* focus_;
        ModuleBoxMap map_;
        WireMap wires_;
        Rubberband rubberband_;
        CPoint offset_;
        CPoint lastTrackPos_;
        CPoint lastClickPos_;
        ModuleBox* anchor_;
        bool isSelecting_;
        bool isTracking_;
    };

	struct WireData 
	{
		WireData();		
		void reset();	
		void cleanup();
        void removeAutoExpanded( ModuleBox* box );

		Wire* wire_;
		bool isWiring_;
		ModulePort* source_;
		ModulePort* target_;
        LinkInfo linkInfo_;
        ModuleBoxList autoExpanded_;
        CPoint lastPos_;
	};

	enum MenuTags { 
        MenuMain, 
        MenuModuleNew, MenuDelete, 
        MenuCopy, MenuCut, MenuPaste,
        MenuCollapse, MenuExpand, 
        MenuSelectAll, MenuUnselectAll,
        MenuPreferences
    };

    enum Direction {
        North = 0,
        East  = 1,
        South = 2,
        West  = 3
    };

	void setProgram();
    ModuleBox* createBox( ModuleData* data );
	bool removeBox( ModuleBox* box );	
	bool removeAllBoxes();	

    void beginSelecting( const CPoint& pos, ModuleBox* box, const long& buttons );
    void continueSelecting( const CPoint& pos );
    void endSelecting();
    void addToSelection( ModuleBox* box, bool toggle );
    void removeFromSelection( ModuleBox* box );
    void selectAll();
    void unselectAll();
    void advanceFocus( bool forward=true );
    void selectBox( ModuleBox* box, bool select, bool toggleSelection=false );
    void focusBox( ModuleBox* box, bool focus );
    void showPrefsDialog();
    void newModule( const CPoint& pos, Catalog catalog );
    void deleteSelection();
    void clipboardCopy();
    void clipboardCut();
    void clipboardPaste( const CPoint& pos );
    void trackSelection( Direction dir, bool wide );   // move by key
    void trackSelection( const CPoint& pos );         // move by mouse
    bool trackSelection( const CPoint& pos, bool testing );
    
	void beginWiring( const CPoint& pos, long buttons );
	void continueWiring( const CPoint& pos );
	void endWiring( const CPoint& pos );
	
	void createWires();
    void removeWires( UINT16 targetId );
	void positionWires( ModuleBox* movedBox );
    void hitTestWires( const CRect& rc );

	void handleMenu( const CPoint& pos, ModuleBox* box );
	COptionMenu* createMenu( const CPoint& pos, ModuleBox* box );

    void openPortDialog( const CPoint& pos, ModulePort* port );
    ModulePort* hitTestPort( const CPoint& pos );
    ModuleBox* hitTestBox( const CPoint& pos );

    void collapseOrExpand( bool collapse );
	
    void rescale();
	void bringToFront( CView* view );
    void getUsedArea( CRect& rcUnion );

	Synth* synth_;
    Editor* editor_;
	CtrlView* ctrlView_;
    ScrollView* scrollView_;
    SelectionData selection_;
    ModuleBoxList boxes_;
	WireData wireData_;
    CRect rcPreferred_;
};


#endif // PROGRAMVIEW_H