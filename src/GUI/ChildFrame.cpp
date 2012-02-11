
#include "ChildFrame.h"



ChildFrame::ChildFrame( const CRect& rcSize, void* systemWindow, VSTGUIEditorInterface* editor ) :
    CFrame( rcSize, systemWindow, editor ),
    mouseCaptureView_( NULL ),
    editor_( NULL )
{
    hCursor_ = LoadCursor( NULL, IDC_ARROW );
    setAutosizeFlags( kAutosizeAll );
}


bool ChildFrame::close()
{
	bOpenFlag = false;

    // Check if pModalView is in a loop.
    // This loop must end before we can destroy ChildFrame.
    // At the end of ChildFrame::onMouseDown Editor::close will be called, which will close the whole GUI.
    if( pModalView )    
    { 
        removeView( pModalView, false );
        return false;
    }
    return true;
}


void ChildFrame::resetDanglingPointers()  
{	
    pModalView = pFocusView = pMouseOverView = NULL; 
}


long ChildFrame::setCursor()
{
    SetCursor( hCursor_ );
    return hCursor_ ? 1 : 0;
}


void ChildFrame::setCursor( CursorType type )
{
	switch (type)
	{
		case CursorWait:
			hCursor_ = LoadCursor( 0, IDC_WAIT );
			break;
		case CursorHSize:
			hCursor_ = LoadCursor(0, IDC_SIZEWE );
			break;
		case CursorVSize:
			hCursor_ = LoadCursor(0, IDC_SIZENS );
			break;
		case CursorNESWSize:
			hCursor_ = LoadCursor( 0, IDC_SIZENESW );
			break;
		case CursorNWSESize:
			hCursor_ = LoadCursor( 0, IDC_SIZENWSE );
			break;
		case CursorSizeAll:
			hCursor_ = LoadCursor( 0, IDC_SIZEALL );
			break;
		case CursorNotAllowed:
			hCursor_ = LoadCursor( 0, IDC_NO );
			break;
		case CursorHand:
			hCursor_ = LoadCursor( 0, IDC_HAND );
			break;
		case CursorBeam:
			hCursor_ = LoadCursor( 0, IDC_IBEAM );
			break;
		default:
			hCursor_ = LoadCursor( 0, IDC_ARROW );
			break;
	}
    SetCursor( hCursor_ );
}


void ChildFrame::drawRect( CDrawContext* pContext, const CRect& updateRect )
{
    try {
        CFrame::drawRect( pContext, updateRect );
    }
    catch( const exception& e ) {
        TRACE( e.what() );
    }
}


void ChildFrame::redraw()
{
    invalid();
    updateWindow();
}


void ChildFrame::updateWindow()
{
    UpdateWindow( (HWND)getSystemWindow() );
}


bool ChildFrame::setSize( CCoord width, CCoord height )
{
	if( getOpenFlag() == false )
		return false;
	
	if( (width == size.width()) && ( height == size.height()))
		return false;


    bool result = false;

    if( editor_->isStandalone() )
    {
        if( backBuffer ) backBuffer->forget();
	    backBuffer = NULL;
        
        result = SetWindowPos( (HWND)pHwnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE ) != 0;
    }
    else if( pEditor )
	{
		AudioEffectX* effect = (AudioEffectX*)((AEffGUIEditor*)pEditor)->getEffect();

		if(effect->sizeWindow( (long)width, (long)height) == false ) { // if parent window will not resize, force it
            HWND hwndParent = GetParent((HWND)pHwnd );
            MoveWindow( hwndParent, 0, 0, width, height, true );
        }
        if( backBuffer ) backBuffer->forget();
	    backBuffer = NULL;
        result = SetWindowPos( (HWND)pHwnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE ) != 0;
	}
    if( result ) 
    {
        CRect rcSize( size.left, size.top, size.left+width, size.top+height );
        setViewSize( rcSize, true );
        editor_->setSize( rcSize );
        redraw();
    }
    return result;
}


long ChildFrame::onKeyDown( VstKeyCode& keyCode )
{
    try {
        if( editor_->onKeyDown( keyCode ) == 0 )
        {
            if( keyCode.virt == 0 && keyCode.character > 127 )
            {
                switch( keyCode.character ) {
                case VK_OEM_PLUS:   keyCode.character = 43; break;
                case VK_OEM_COMMA:  keyCode.character = 44; break;
                case VK_OEM_MINUS:  keyCode.character = 45; break;
                case VK_OEM_PERIOD: keyCode.character = 46; break;
                }
            }
        }
        return CFrame::onKeyDown( keyCode );
    }
    catch( const exception& e ) {
        TRACE( e.what() );
        editor_->setAppState( Editor::NotReady );
        return 1;
    }
}


bool ChildFrame::onWheel(const CPoint& pos, const CMouseWheelAxis& axis, const float& distance, const long &buttons)
{
    try {
        if( mouseCaptureView_ ) 
        {
		    CPoint p( pos );
		    mouseCaptureView_->frameToLocal( p );
            return mouseCaptureView_->onWheel( p, axis, distance, buttons );
        }
        return CFrame::onWheel( pos, axis, distance, buttons );
    }
    catch( const exception& e ) {
        TRACE( e.what() );
        editor_->setAppState( Editor::NotReady );
        return kMouseEventHandled;
    }
}


CMouseEventResult ChildFrame::onMouseDown( CPoint& pos, const long& buttons )
{
    try 
    {
        if( mouseCaptureView_ ) 
        {
		    CPoint p( pos );
		    mouseCaptureView_->frameToLocal( p );
            return mouseCaptureView_->onMouseDown( p, buttons );
        }
        CMouseEventResult result = CFrame::onMouseDown( pos, buttons );

        if( getOpenFlag() == false ) {  // delayed ctor call, when a ModalDialog was open
            editor_->close();
        }
        return result;
    }
    catch( const exception& e ) {
        TRACE( e.what() );
        editor_->setAppState( Editor::NotReady );
        return kMouseEventHandled;
    }
}


CMouseEventResult ChildFrame::onMouseMoved( CPoint& pos, const long& buttons )
{
    if( buttons & kRButton )
        TRACE( "pos.x=%d pos.y=%d\n", pos.x, pos.y );

    try 
    {
        if( mouseCaptureView_ ) 
        {
		    CPoint p( pos );
		    mouseCaptureView_->frameToLocal( p );
            return mouseCaptureView_->onMouseMoved( p, buttons );
        }
        else return CFrame::onMouseMoved( pos, buttons );
    }
    catch( const exception& e ) {
        TRACE( e.what() );
        editor_->setAppState( Editor::NotReady );
        return kMouseEventHandled;
    }
}


CMouseEventResult ChildFrame::onMouseUp( CPoint& pos, const long& buttons )
{
    try 
    {
        if( mouseCaptureView_ ) 
        {
		    CPoint p( pos );
		    mouseCaptureView_->frameToLocal( p );
            return mouseCaptureView_->onMouseUp( p, buttons );
        }
        else return CFrame::onMouseUp( pos, buttons );
    }
    catch( const exception& e ) {
        TRACE( e.what() );
        editor_->setAppState( Editor::NotReady );
        return kMouseEventHandled;
    }
}


CMouseEventResult ChildFrame::onMouseExited( CPoint& pos, const long& buttons )
{
    try {
        return CFrame::onMouseExited( pos, buttons );
    }
    catch( const exception& e ) {
        TRACE( e.what() );
        editor_->setAppState( Editor::NotReady );
        return kMouseEventHandled;
    }
}


void ChildFrame::makeVstKeyCode( UINT nChar, VstKeyCode& keyCode )
{
    static struct
    {
        UINT vkWin;
        unsigned char vstVirt;
    } VKeys[] =
    {
        { VK_BACK,      VKEY_BACK      },
        { VK_TAB,       VKEY_TAB       },
        { VK_CLEAR,     VKEY_CLEAR     },
        { VK_RETURN,    VKEY_RETURN    },
        { VK_PAUSE,     VKEY_PAUSE     },
        { VK_ESCAPE,    VKEY_ESCAPE    },
        { VK_SPACE,     VKEY_SPACE     },
        //  { VK_NEXT,      VKEY_NEXT      },
        { VK_END,       VKEY_END       },
        { VK_HOME,      VKEY_HOME      },
        { VK_LEFT,      VKEY_LEFT      },
        { VK_UP,        VKEY_UP        },
        { VK_RIGHT,     VKEY_RIGHT     },
        { VK_DOWN,      VKEY_DOWN      },
        { VK_PRIOR,     VKEY_PAGEUP    },
        { VK_NEXT,      VKEY_PAGEDOWN  },
        { VK_SELECT,    VKEY_SELECT    },
        { VK_PRINT,     VKEY_PRINT     },
        { VK_EXECUTE,   VKEY_ENTER     },
        { VK_SNAPSHOT,  VKEY_SNAPSHOT  },
        { VK_INSERT,    VKEY_INSERT    },
        { VK_DELETE,    VKEY_DELETE    },
        { VK_HELP,      VKEY_HELP      },
        { VK_NUMPAD0,   VKEY_NUMPAD0   },
        { VK_NUMPAD1,   VKEY_NUMPAD1   },
        { VK_NUMPAD2,   VKEY_NUMPAD2   },
        { VK_NUMPAD3,   VKEY_NUMPAD3   },
        { VK_NUMPAD4,   VKEY_NUMPAD4   },
        { VK_NUMPAD5,   VKEY_NUMPAD5   },
        { VK_NUMPAD6,   VKEY_NUMPAD6   },
        { VK_NUMPAD7,   VKEY_NUMPAD7   },
        { VK_NUMPAD8,   VKEY_NUMPAD8   },
        { VK_NUMPAD9,   VKEY_NUMPAD9   },
        { VK_MULTIPLY,  VKEY_MULTIPLY  },
        { VK_ADD,       VKEY_ADD,      },
        { VK_SEPARATOR, VKEY_SEPARATOR },
        { VK_SUBTRACT,  VKEY_SUBTRACT  },
        { VK_DECIMAL,   VKEY_DECIMAL   },
        { VK_DIVIDE,    VKEY_DIVIDE    },
        { VK_F1,        VKEY_F1        },
        { VK_F2,        VKEY_F2        },
        { VK_F3,        VKEY_F3        },
        { VK_F4,        VKEY_F4        },
        { VK_F5,        VKEY_F5        },
        { VK_F6,        VKEY_F6        },
        { VK_F7,        VKEY_F7        },
        { VK_F8,        VKEY_F8        },
        { VK_F9,        VKEY_F9        },
        { VK_F10,       VKEY_F10       },
        { VK_F11,       VKEY_F11       },
        { VK_F12,       VKEY_F12       },
        { VK_NUMLOCK,   VKEY_NUMLOCK   },
        { VK_SCROLL,    VKEY_SCROLL    },
        { VK_SHIFT,     VKEY_SHIFT     },
        { VK_CONTROL,   VKEY_CONTROL   },
        { VK_MENU,      VKEY_ALT       },
        //  { VK_EQUALS,    VKEY_EQUALS    },
    };
    
    if( (nChar >= 'A') && (nChar <= 'Z') ) {
        keyCode.character = nChar + ('a' - 'A');
    } else {
        keyCode.character = nChar;
    }

    keyCode.virt = 0;
    for( int i = 0; i < (sizeof( VKeys ) / sizeof( VKeys[0] )); i++ ) {
        if (nChar == VKeys[i].vkWin)
        {
            keyCode.virt = VKeys[i].vstVirt;
            break;
        }

        keyCode.modifier = 0;
        if( GetKeyState( VK_SHIFT ) & 0x8000 ) {
            keyCode.modifier |= MODIFIER_SHIFT;
        }
        if( GetKeyState( VK_CONTROL ) & 0x8000 ) {
            keyCode.modifier |= MODIFIER_CONTROL;
        }
        if( GetKeyState( VK_MENU ) & 0x8000 ) {
            keyCode.modifier |= MODIFIER_ALTERNATE;
        }
    }
}





