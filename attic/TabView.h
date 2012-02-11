
#ifndef TABVIEW_H
#define TABVIEW_H

#include "Editor.h"



class TabView : public CTabView
{
public:
	TabView( const CRect& size, 
		     CFrame* parent, 
			 CBitmap* bmTab, 
			 CBitmap* bmBkgnd = NULL, 
			 TabPosition tabPosition = kPositionTop, 
			 Editor* editor = NULL )
	: CTabView( size, parent, bmTab, bmBkgnd, tabPosition, 0 )
	, editor_( editor ) 
	{}

	//CMouseEventResult onMouseDown( CPoint &where, const long& buttons );
	bool selectTab( long index );

protected:
	Editor* editor_;
};





#endif // TABVIEW_H