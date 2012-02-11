
#include "TabView.h"


//CMouseEventResult TabView::onMouseDown( CPoint &where, const long& buttons )
//{
//	if(( buttons & kRButton ) > 0 ) 
//	{
//		CView* view = getViewAt( where, true );
//		if( view && view->isTypeOf( "CTabButton" ))
//		{
//			CRect r;
//			localToFrame( where );
//			r.offset( where.x, where.y );
//			r.offset( -size.left, -size.top );
//
//			COptionMenu* menu = new COptionMenu( r, NULL, 0 );
//			menu->addEntry( "Tabs Left" );
//			menu->addEntry( "Tabs Right" );
//			menu->addEntry( "Tabs Top" );
//			menu->addEntry( "Tabs Bottom" );
//			menu->addEntry( "-" );
//			menu->addEntry( "Align Tabs Centered" );
//			menu->addEntry( "Align Tabs Left/Top" );
//			menu->addEntry( "Align Tabs Right/Bottom" );
//			getFrame ()->addView( menu );
//			menu->takeFocus();
//
//			long menuResult = menu->getLastResult();
//			getFrame()->removeView( menu );
//
//			if( menuResult != -1 )
//			{
//				if( menuResult < 4 )
//				{
//					r = size;
//					editor_->setTabView( (TabPosition)menuResult );
//					return kMouseEventNotHandled;
//				}
//				else
//				{
//					alignTabs( kAlignCenter );
//				}
//			}
//		}
//	}
//	return CTabView::onMouseDown( where, buttons );
//}
//

bool TabView::selectTab( long index )
{
	bool result = CTabView::selectTab( index );
	#if DEBUG
	//getFrame()->dumpHierarchy ();
	#endif
	return result;
}



