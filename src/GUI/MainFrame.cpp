
#include "MainFrame.h"
#include "ChildFrame.h"
#include "Fonts.h"
#include "Images.h"
#include "../AppOptions.h"



TCHAR MainFrame::className_[] = L"nexusMainFrame";


//------------------------------------------------
// CALLBACK WindowProc
//------------------------------------------------

LRESULT CALLBACK NexusWindowProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    MainFrame* frame = (MainFrame*)(LONG_PTR)GetWindowLongPtr( hwnd, GWLP_USERDATA );
    bool doubleClick = false;
    long result      = 0;

    if( frame ) 
        result = frame->WindowProc( frame->hwnd_, msg, wParam, lParam );
    
    if( result == 0 )
    {
        TRACE( "msg=%d\n", msg );
		switch( msg )
        {
            case WM_CLOSE: 
                DestroyWindow( hwnd );  
                return 1;

            case WM_DESTROY: 
                PostQuitMessage( 0 ); 
                return 1;

            default: 
                return DefWindowProc( hwnd, msg, wParam, lParam );
        }
    }
    return result;
}



//------------------------------------------------
// class MainFrame
//------------------------------------------------

MainFrame::MainFrame() :
    hwnd_( NULL ),
    childFrame_( NULL )
    {}


int MainFrame::create( HINSTANCE hInstance, AppOptions* options )
{
    options_ = options;
    
    wc_.cbSize        = sizeof( WNDCLASSEX );
    wc_.style         = CS_OWNDC | CS_DBLCLKS;
    wc_.lpfnWndProc   = NexusWindowProc;
    wc_.cbClsExtra    = 0;
    wc_.cbWndExtra    = 0;
    wc_.hInstance     = hInstance;
    wc_.hIcon         = LoadIcon( hInstance, MAKEINTRESOURCE( NEXUS_LARGE_ICON ));
    wc_.hCursor       = LoadCursor( NULL, IDC_ARROW );
    //wc_.hbrBackground = CreateSolidBrush( RGB( colors.fill2.red, colors.fill2.green, colors.fill2.blue ));
    wc_.hbrBackground = NULL;
    wc_.lpszMenuName  = NULL;
    wc_.lpszClassName = className_;
    wc_.hIconSm       = LoadIcon( hInstance, MAKEINTRESOURCE( NEXUS_SMALL_ICON )); 

    if( RegisterClassEx( &wc_ ) == 0 ) {
        return 0;
    }
    
    hwnd_ = CreateWindow(
        className_,
        L"Nexus",
        WS_SYSMENU | WS_THICKFRAME | WS_CLIPCHILDREN | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
        options_->windowLeft_, 
        options_->windowTop_, 
        options_->windowRight_ - options_->windowLeft_, 
        options_->windowBottom_ - options_->windowTop_,
        NULL, NULL, hInstance, NULL);

    if( hwnd_ == NULL ) {
        return 0;
    }
    SetWindowLongPtr( hwnd_, GWLP_USERDATA, (__int3264)(LONG_PTR)this );
    return 1;
}


int MainFrame::show( ChildFrame* childFrame )
{
    childFrame_ = childFrame;
    ASSERT( childFrame_ );
    MSG msg;

    ShowWindow( hwnd_, options_->windowState_ );
    UpdateWindow( hwnd_ );

    while( GetMessage( &msg, NULL, 0, 0) > 0 )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    return msg.wParam;
}


long MainFrame::WindowProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    if( childFrame_ == NULL ) {
        return 0;
    }
    bool doubleClick = false;
    
    switch( msg )
    {
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WM_NCLBUTTONDBLCLK:
        return onNcButton( msg, lParam );

    case WM_MOUSEWHEEL:
        return onWheel( wParam, lParam );

    case WM_CHAR:
        return onChar( wParam );

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        return onKeyDown( wParam );

    case WM_KEYUP:
    case WM_SYSKEYUP:
        return onKeyUp( wParam );

    case WM_NCPAINT:
        return onNcPaint();

    case WM_NCACTIVATE:
        return 1;

	case WM_ERASEBKGND:
        childFrame_->redraw();
        //childFrame_->updateWindow();
        return 1;
        //return 0;
            
    //case WM_PAINT:
	   //onNcPaint();
       //return 0;
	   //return onPaint( hwnd );

    case WM_SETCURSOR:
        return onSetCursor( wParam, lParam );
        
    case WM_SIZE:
        return onSize( wParam, lParam );

    case WM_MOVE:
        return onMove( lParam );

    case WM_GETMINMAXINFO:
        return onGetMinMaxInfo( lParam );

	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
	#if( _WIN32_WINNT >= 0x0500 )
	case WM_XBUTTONDBLCLK:
	#endif
		doubleClick = true;
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONDOWN:
	#if( _WIN32_WINNT >= 0x0500 )
	case WM_XBUTTONDOWN:
	#endif
		{
			long buttons = makeButtonCode( wParam, doubleClick );
			CPoint pos( (CCoord)((int)(short)LOWORD(lParam)), (CCoord)((int)(short)HIWORD(lParam)) );
			if( childFrame_->onMouseDown( pos, buttons) == kMouseEventHandled )
				SetCapture( hwnd );
			return 0;
		}
	case WM_MOUSELEAVE:
		{
            CPoint pos;
			childFrame_->getCurrentMouseLocation( pos );
			childFrame_->onMouseExited( pos, childFrame_->getCurrentMouseButtons () );
			return 0;
		}
	case WM_MOUSEMOVE:
		{
            long buttons = makeButtonCode( wParam, doubleClick );
			CPoint pos( (CCoord)((int)(short)LOWORD(lParam)), (CCoord)((int)(short)HIWORD(lParam)) );
			childFrame_->onMouseMoved( pos, buttons );
			return 0;
		}

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	#if( _WIN32_WINNT >= 0x0500 )
	case WM_XBUTTONUP:
	#endif
		{
			long buttons = makeButtonCode( wParam, doubleClick );
            CPoint pos( (CCoord)((int)(short)LOWORD(lParam)), (CCoord)((int)(short)HIWORD(lParam)) );
			childFrame_->onMouseUp( pos, buttons );
			ReleaseCapture();
			return 0;
		}
    }
    return 0;
}


long MainFrame::onChar( WPARAM nChar )
{
    VstKeyCode keyCode;
    keyCode.virt      = 0;
    keyCode.modifier  = 0;

    if( GetKeyState( VK_SHIFT ) & 0x8000 ) {
        keyCode.modifier |= MODIFIER_SHIFT;
    }
    if( GetKeyState( VK_CONTROL ) & 0x8000 ) {
        keyCode.modifier |= MODIFIER_CONTROL;
    }
    if( GetKeyState( VK_MENU ) & 0x8000 ) {
        keyCode.modifier |= MODIFIER_ALTERNATE;
    }
    
    // send only ascii keys, vkeys and keys with modifiers are handled by onKeyDown
    if( keyCode.modifier == 0 || keyCode.modifier == MODIFIER_SHIFT ) 
    {
        if( nChar > 32 && nChar <= 127 ) {
            keyCode.character = nChar;
            return childFrame_->onKeyDown( keyCode );
        }
    }
    return 0;
}


long MainFrame::onKeyDown( WPARAM nChar )
{
    VstKeyCode keyCode;
    ChildFrame::makeVstKeyCode( nChar, keyCode );

    if( keyCode.virt == VKEY_F4 && keyCode.modifier == MODIFIER_ALTERNATE ) {
        return close();
    }

    // send only vkeys or keys with modifiers pressed, the rest is send by onChar
    if( keyCode.virt || keyCode.modifier == MODIFIER_ALTERNATE || keyCode.modifier == MODIFIER_CONTROL ) {
        return childFrame_->onKeyDown( keyCode );
    }
    return 0;
}


long MainFrame::onKeyUp( UINT nChar )
{
    VstKeyCode keyCode;
    ChildFrame::makeVstKeyCode( nChar, keyCode );

    return childFrame_->onKeyUp( keyCode );
}


long MainFrame::onWheel( WPARAM wParam, LPARAM lParam )
{
	long buttons = 0;
	if (GetKeyState (VK_SHIFT)   < 0)
		buttons |= kShift;
	if (GetKeyState (VK_CONTROL) < 0)
		buttons |= kControl;
	if (GetKeyState (VK_MENU)    < 0)
		buttons |= kAlt;

	CPoint pos( LOWORD( lParam ), HIWORD( lParam ));
	short zDelta = (short)HIWORD( wParam );
	RECT rctWnd;
    HWND hwndChild = (HWND)childFrame_->getSystemWindow();  // translate position to childFrame coordinates
	GetWindowRect( hwndChild, &rctWnd );
	pos.offset( (CCoord)-rctWnd.left, (CCoord)-rctWnd.top );

	return childFrame_->onWheel( pos, kMouseWheelAxisY, (float)(zDelta / WHEEL_DELTA), buttons );
}


long MainFrame::onSetCursor( WPARAM wParam, LPARAM lParam )
{
    if( hwnd_ == (HWND)wParam ) {
        return 0;
    }
    else if( childFrame_->getSystemWindow() == (HWND)wParam ) {
        return childFrame_->setCursor();
    }
    return 0;
}


long MainFrame::onSize( WPARAM wParam, LPARAM lParam )
{
    if( wParam & SIZE_MINIMIZED ) {
        options_->windowState_ = SW_SHOWMINIMIZED;
    }
    else if( wParam & SIZE_MAXIMIZED ) {
        options_->windowState_ = SW_SHOWMAXIMIZED;
    }
    else {
        storeWindowRect();
        options_->windowState_ = SW_SHOW;
    }

    short width  = LOWORD( lParam );
    short height = HIWORD( lParam );
    if( childFrame_ )  {
        childFrame_->setSize( width, height );
    }
    return 0;
}


long MainFrame::onMove( LPARAM lParam )
{
    if( IsZoomed( hwnd_ ) == false )
        storeWindowRect();
    return 0;
}


long MainFrame::onGetMinMaxInfo( LPARAM lParam )
{
	MINMAXINFO* info = (MINMAXINFO*)lParam;
	POINT ptPoint;

	ptPoint.x = 640;	                            
	ptPoint.y = 480;	                            
	info->ptMinTrackSize = ptPoint;

	ptPoint.x = GetSystemMetrics( SM_CXMAXIMIZED );	
	ptPoint.y = GetSystemMetrics( SM_CYMAXIMIZED );	
	info->ptMaxTrackSize = ptPoint;

	return 0;
}


long MainFrame::onPaint( HWND hwnd )
{
    HRGN rgn = CreateRectRgn( 0, 0, 0, 0 );
	if (GetUpdateRgn( hwnd, rgn, false ) == NULLREGION )
	{
		DeleteObject( rgn );
		return 0;
	}

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint( hwnd, &ps );

	CDrawContext* context = childFrame_->getBackBuffer();
	if( context == NULL )
		context = new CDrawContext( childFrame_, hdc, hwnd );
			
	CRect updateRect( (CCoord)ps.rcPaint.left, (CCoord)ps.rcPaint.top, (CCoord)ps.rcPaint.right, (CCoord)ps.rcPaint.bottom );
	childFrame_->drawRect( context, updateRect );

	if( childFrame_->getBackBuffer() )
	{
		CDrawContext localContext( childFrame_, hdc, hwnd );
		childFrame_->getBackBuffer()->copyFrom( &localContext, updateRect, CPoint( (CCoord)ps.rcPaint.left, (CCoord)ps.rcPaint.top) );
	}
	else {
		context->forget ();
    }
	EndPaint( hwnd, &ps );
	DeleteObject( rgn );
	return 0;
}


long MainFrame::onNcPaint()
{
    RECT rc;
	GetWindowRect( hwnd_, &rc );

    CCoord right     = rc.right - rc.left;
    CCoord bottom    = rc.bottom - rc.top;
    CCoord cxFrame   = GetSystemMetrics( SM_CXSIZEFRAME );
    CCoord cyFrame   = GetSystemMetrics( SM_CYSIZEFRAME );
    CCoord cyCaption = GetSystemMetrics( SM_CYCAPTION );

    CBitmap* imgIcon = images.get( Images::iconFrame );

    CRect rcFrame( 0, 0, right, bottom );
    CRect rcTop( 0, 1, right, cyFrame );
    CRect rcBottom( 0, bottom-cyFrame, right, bottom );
    CRect rcLeft( 0, cyFrame, cxFrame, bottom-cyFrame ); 
    CRect rcRight( right-cxFrame-1, cyFrame, right, bottom-cyFrame );
    CRect rcCaptionBar( cxFrame, cyFrame, right-cxFrame, cyFrame+cyCaption );

    CRect rcCaption( 28, cyFrame, 128, cyCaption+cyFrame );
    CRect rcIcon( cxFrame, cxFrame, cxFrame+imgIcon->getWidth(), cxFrame+imgIcon->getHeight());
    CRect rcMini( right-cxFrame*3-36, cxFrame, right-cxFrame*3-24, cxFrame+12 );
    CRect rcMaxi( right-cxFrame*2-24, cxFrame, right-cxFrame*2-12, cxFrame+12 );
    CRect rcClose( right-cxFrame-12, cxFrame, right-cxFrame, cxFrame+12 );

    COffscreenContext context( childFrame_, (long)right, (long)bottom );
        
    // frame
    context.setFillColor( colors.appFrame );
    context.drawRect( rcTop, kDrawFilled );
    context.drawRect( rcBottom, kDrawFilled );
    context.drawRect( rcLeft, kDrawFilled );
    context.drawRect( rcRight, kDrawFilled );
    context.setFillColor( colors.appCaptionBar );
    context.drawRect( rcCaptionBar, kDrawFilled );

    // buttons: minimize, maximize, close
    context.setFillColor( colors.appMin );
    context.drawRect( rcMini, kDrawFilled );
    context.setFillColor( colors.appMax );
    context.drawRect( rcMaxi, kDrawFilled );
    context.setFillColor( colors.appClose );
    context.drawRect( rcClose, kDrawFilled );
    
    context.setFont( fontPirulen14, fontPirulen14->getSize());
	context.setFontColor( colors.appCaption );
	context.drawStringUTF8( "Nexus", rcCaption, kLeftText, true );

    imgIcon->draw( &context, rcIcon );

    HDC hdc = GetWindowDC( hwnd_ );
    CDrawContext localContext( childFrame_, hdc, hwnd_ );

    context.copyFrom( &localContext, rcFrame );
    ReleaseDC( hwnd_, hdc );

    RECT rcUpdate;
    GetUpdateRect( hwnd_, &rcUpdate, false );
    InvalidateRect( hwnd_, &rcUpdate, false );

    return 1;
}


long MainFrame::onNcButton( UINT msg, LPARAM lParam )
{
	RECT rc;
	GetWindowRect( hwnd_, &rc );
    CPoint pos( LOWORD( lParam ), HIWORD( lParam ));

    CCoord cxButton = GetSystemMetrics( SM_CXSMSIZE );

    CCoord cx = GetSystemMetrics( SM_CXFRAME );
    CRect rcSys( rc.left+cx, rc.top+cx, rc.left+cx+12, rc.top+cx+12 );
    CRect rcMini( rc.right-cx*3-36, rc.top+cx, rc.right-cx*3-24, rc.top+cx+12 );
    CRect rcMaxi( rc.right-cx*2-24, rc.top+cx, rc.right-cx*2-12, rc.top+cx+12 );
    CRect rcClose(  rc.right-cx-13, rc.top+cx-1, rc.right-cx+1, rc.top+cx+13 );

    switch( msg ) 
    {
    case WM_NCLBUTTONDOWN:
        if( rcMini.pointInside( pos ))  return 1;
        if( rcMaxi.pointInside( pos ))  return 1;
        if( rcClose.pointInside( pos )) return 1;
        break;

    case WM_NCLBUTTONUP:
        if( rcMini.pointInside( pos )) {
            DefWindowProc( hwnd_, WM_SYSCOMMAND, SC_MINIMIZE, 0 );
            return 1;
        }
        if( rcMaxi.pointInside( pos )) {
            long cmd = IsZoomed( hwnd_ ) ? SC_RESTORE : SC_MAXIMIZE;
            DefWindowProc( hwnd_, WM_SYSCOMMAND, cmd, 0 );
            return 1;
        }
        if( rcClose.pointInside( pos )) {
            return close();
        }
        break;
    case WM_NCLBUTTONDBLCLK:
        if( rcSys.pointInside( pos )) {
            return close();
        }
    }
    return 0;
}


long MainFrame::close()
{
    return DefWindowProc( hwnd_, WM_CLOSE, 0, 0 );
}


long MainFrame::makeButtonCode( UINT wParam, bool doubleClick )
{
    long buttons = 0;

	if( wParam & MK_LBUTTON )
		buttons |= kLButton;
	if( wParam & MK_RBUTTON )
		buttons |= kRButton;
	if( wParam & MK_MBUTTON )
		buttons |= kMButton;
	#if( _WIN32_WINNT >= 0x0500 )
	if( wParam & MK_XBUTTON1 )
		buttons |= kButton4;
	if( wParam & MK_XBUTTON2 )
		buttons |= kButton5;
	#endif
	if( wParam & MK_CONTROL )
		buttons |= kControl;
	if( wParam & MK_SHIFT )
		buttons |= kShift;
	if( GetKeyState( VK_MENU ) < 0 )    // added to achieve information from the ALT button
		buttons |= kAlt;
	if( doubleClick )
		buttons |= kDoubleClick;

    return buttons;
}


void MainFrame::getClientRect( CRect& rcSize )
{
    CCoord cxFrame   = GetSystemMetrics( SM_CXSIZEFRAME );
    CCoord cyFrame   = GetSystemMetrics( SM_CYSIZEFRAME );
    CCoord cyCaption = GetSystemMetrics( SM_CYCAPTION );

    rcSize.left   = options_->windowLeft_   + cxFrame;
    rcSize.top    = options_->windowTop_    + cyFrame + cyCaption;
    rcSize.right  = options_->windowRight_  - cxFrame;
    rcSize.bottom = options_->windowBottom_ - cyFrame;

    rcSize.offset( -rcSize.left, -rcSize.top );
}


void MainFrame::storeWindowRect()
{
    RECT rc;
	GetWindowRect( hwnd_, &rc );  
    options_->windowLeft_   = rc.left; 
    options_->windowTop_    = rc.top;
    options_->windowRight_  = rc.right;
    options_->windowBottom_ = rc.bottom;
}







