//-------------------------------------------------------------------------------------------------------
// VST Plug-in Window Sizer
// Version 0.1
// Steven Schulze
// vstsizer@alienworks.com
//
//   This class adds the ability to resize a VST plugin GUI in most hosts.   
//
//   Please	email me at the provided address if you find ways to improve the code.
//
//   To use:
//   ======
//
//   1) Create an instance of this class in your editor class
//   2) Call VstWindowSizer::SetEffect() to set the AudioEffect instance
//   3) In the AEffEditor::open function, right after you create your editor window, 
//      call VstWindowSizer::SetEditorHwnd() with the newly created window handle
//
//   That is it!  You should now be able to resize the plugin window in most hosts.
//
//	 Bonus functionality:
//   ===================
//
//   For any child window on your editor window (say, a fader control), you can let VstWindowSizer
//   automatically move/resize that child window by specifying one or more anchor sides.  
//   
//   For instance, calling... 
//
//     sizer->SetAnchoredWindow(hWndSomeFader, VstWindowSizer::AnchorRight | VstWindowSizer::AnchorBottom);
//
//   ...will cause that control to retain its relative distance from the bottom and right edge while the 
//   plugin GUI	is resized.  To make a child window stretch, specify both the left and right, or the top 
//   and bottom sides as anchor points.
//
//-------------------------------------------------------------------------------------------------------

#include "windows.h"
#include <vector>
#include "AudioEffectx.h"
#include "VstWindowSizer.h"

// Since we cannot use the GWL_USERDATA value of windows we would like to track 
// (seeing as we don't own them), we need to maintain this global list of windows
std::vector<VstWindowSizer::WindowInfo> VstWindowSizer::m_framesTrack;
std::vector<VstWindowSizer::WindowInfo> VstWindowSizer::m_editors;

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------
VstWindowSizer::VstWindowSizer(void)
{
	m_frameHwnd = NULL;
	m_editorInfo = NULL;

	m_hostApp = HostAppUnknown;
	m_trackingInitialized = false;

	m_sizingCursor = HTNOWHERE;
	m_resizeFlags = ResizeFlagNone;
	m_resizeOffset.x = 0;
	m_resizeOffset.y = 0;
	m_startMouseOffset.x = 0;
	m_startMouseOffset.y = 0;

	m_effect = NULL;
	m_hostCanResize = false;
	m_preventChildSize = 0;

	m_rect.left = 0;
	m_rect.top = 0;
	m_rect.right = 800;
	m_rect.bottom = 600;

	m_minSize.cx = 400;
	m_minSize.cy = 300;
	m_maxSize.cx = 1400;
	m_maxSize.cy = 1200;
}


//----------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------
VstWindowSizer::~VstWindowSizer(void)
{
	EndTracking(NULL, true);
}


//----------------------------------------------------------------------
// This must be called before anything else is called
//----------------------------------------------------------------------
void VstWindowSizer::SetEffect(AudioEffectX* pEffect)
{
	m_effect = pEffect;

	if (m_effect)
		m_hostCanResize = m_effect->canHostDo("sizeWindow") > 0;
	else
		m_hostCanResize = false;
}


//----------------------------------------------------------------------
// Specify the main window that contains the editor GUI
//----------------------------------------------------------------------
void VstWindowSizer::SetEditorHwnd(HWND hWndEditor)
{
	if (m_editorInfo != NULL)
		EndTracking(m_editorInfo->hWnd, true);

	if (IsWindow(hWndEditor))
	{
		// Add the editor window to the list of windows to be resized when the frame gets resized
		m_editors.push_back(WindowInfo());

		m_editorInfo = &m_editors[m_editors.size() - 1];
		ZeroMemory(m_editorInfo, sizeof(WindowInfo));
		
		m_editorInfo->hWnd = hWndEditor;
		m_editorInfo->pSizer = this;
		m_editorInfo->prevProc = GetWindowLongPtr(m_editorInfo->hWnd, GWL_WNDPROC);
		
		// Hook into the editor window's procedure
		SetWindowLongPtr(m_editorInfo->hWnd, GWL_WNDPROC, (long)EditorWindowProc);
		
		// Attempt to detect the current host
		DetectHost();
		
		// Since we initialize tracking in the first WM_PAINT event we receive, 
		// we force a repaint in case the window was already painted
		InvalidateRect(hWndEditor, NULL, TRUE);
	}
	else
		m_editorInfo = NULL;
}


//----------------------------------------------------------------------
// Specify the minimum size that the window can be sized to
//----------------------------------------------------------------------
void VstWindowSizer::SetMinSize(int minWidth, int minHeight)
{
	m_minSize.cx = minWidth;
	m_minSize.cy = minHeight;
}


//----------------------------------------------------------------------
// Specify the maximum size that the window can be sized to
//----------------------------------------------------------------------
void VstWindowSizer::SetMaxSize(int maxWidth, int maxHeight)
{
	m_maxSize.cx = maxWidth;
	m_maxSize.cy = maxHeight;
}


//----------------------------------------------------------------------
// Get the current size
//----------------------------------------------------------------------
SIZE VstWindowSizer::GetCurrentSize()
{
	SIZE size;

	size.cx = m_rect.right - m_rect.left;
	size.cy = m_rect.bottom - m_rect.top;
	return size;
}


//----------------------------------------------------------------------
// Add a child window that will be automatically moved/sized during the
// main plugin GUI's sizing.  Specify VstWindowSizer::AnchorNone to remove the window
// from the sizing list
//----------------------------------------------------------------------
void VstWindowSizer::SetAnchoredWindow(HWND hWnd, int anchor)
{
	// First try to see if it was already added
	for (int idx = 0; idx < m_anchoredWindows.size(); idx++)
	{
		if (m_anchoredWindows[idx].hWnd == hWnd)
		{
			if (anchor == AnchorNone)
				m_anchoredWindows.erase(m_anchoredWindows.begin() + idx);   // Remove from the list
			else
				m_anchoredWindows[idx].anchor = anchor;						// Update anchor mode

			return;
		}
	}

	// Only add if it is actually anchored to any of the edges
	if (anchor == AnchorNone)
		return;

	RECT parentRect = {0};
	RECT anchorRect;
	HWND parentHwnd = GetParent(hWnd);

	// Retrieve initial position and size info.  Everything here on out will be
	// relative to that
	GetClientRect(parentHwnd, &parentRect);
	GetWindowRect(hWnd, &anchorRect);
	ScreenRectToClient(parentHwnd, &anchorRect);
	
	// Add to the list of windows to move/resize
	m_anchoredWindows.push_back(AnchoredWindow());

	AnchoredWindow* pAnchor = &m_anchoredWindows[m_anchoredWindows.size() - 1];
	pAnchor->hWnd = hWnd;
	pAnchor->anchor = anchor;
	pAnchor->startRect = anchorRect;
	pAnchor->startParentSize.cx = parentRect.right - parentRect.left;
	pAnchor->startParentSize.cy = parentRect.bottom - parentRect.top;
}


//----------------------------------------------------------------------
// This initializes the tracking operations
//----------------------------------------------------------------------
void VstWindowSizer::SetupTracking()
{
	RECT wndRect;
	RECT frameRect;
	int idx;
	
	// Should only do this once
	if (m_trackingInitialized)
		return;

	m_trackingInitialized = true;

	if (!m_resizeFlags & ResizeFlagTrackParentSize)
		return;

	GetWindowRect(m_editorInfo->hWnd, &wndRect);

	// Add ourselves as a window to track
	m_windowsAdjust.push_back(WindowInfo());

	idx = m_windowsAdjust.size() - 1;
	m_windowsAdjust[idx].pSizer = this;
	m_windowsAdjust[idx].hWnd = m_editorInfo->hWnd;
	m_windowsAdjust[idx].prevProc = NULL;
	m_windowsAdjust[idx].deltaWidth = 0;
	m_windowsAdjust[idx].deltaHeight = 0;

	// Locate all the parent windows we need to resize
	HWND parent = GetParent(m_editorInfo->hWnd);

	while (parent)
	{
		LONG style = GetWindowLong(parent, GWL_STYLE);
		LONG_PTR oldProc = NULL;

		// Search up the parent chain until we find the frame window
		if (style & WS_CHILD && ((style & WS_CAPTION) != WS_CAPTION) && !(style & (WS_THICKFRAME | WS_POPUP)))
		{
			style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			SetWindowLongPtr(parent, GWL_STYLE, style);
			
			if (m_resizeFlags & ResizeFlagFilterChildWM_SIZE)
			{
				// Some hosts like Cubase needs special treatment here.  
				// Prevent this parent from telling it's parent that it got resized
				oldProc = (LONG_PTR)GetWindowLongPtr(parent, GWL_WNDPROC);
				SetWindowLongPtr(parent, GWL_WNDPROC, (long)ChildWindowProc);
			}
			
			m_windowsAdjust.push_back(WindowInfo());
			int idx = m_windowsAdjust.size() - 1;

			m_windowsAdjust[idx].pSizer = this;
			m_windowsAdjust[idx].hWnd = parent;
			m_windowsAdjust[idx].prevProc = oldProc;
			m_windowsAdjust[idx].deltaWidth = 0;
			m_windowsAdjust[idx].deltaHeight = 0;
		}
		else
		{
			// This is hopefully the frame window
			POINT topLeft;
			int setWidth, setHeight;
			
			m_frameHwnd = parent;

			// Add to the list of windows to be tracked for size changes
			m_framesTrack.push_back(WindowInfo());
			int idx = m_framesTrack.size() - 1;

			m_framesTrack[idx].pSizer = this;
			m_framesTrack[idx].hWnd = parent;
			m_framesTrack[idx].deltaWidth = 0;
			m_framesTrack[idx].deltaHeight = 0;
			m_framesTrack[idx].prevProc = GetWindowLongPtr(parent, GWL_WNDPROC);
			
			// To intercept mouse events, etc, we hook into the frame window's procedure
			SetWindowLongPtr(m_frameHwnd, GWL_WNDPROC, (long)FrameWindowProc);
			GetWindowRect(m_frameHwnd, &frameRect);

			if (m_resizeFlags & ResizeFlagFillOutParents)
			{
				// Some hosts (like SONAR) puts up an ugly white border around our window plugin GUI, 
				// so adjust all relevant windows to fill out the left, bottom and right edges
				RECT wndRect;
				RECT frameClientRect;
				POINT clientOffset;
				POINT frameClientOffset;

				GetClientRect(m_frameHwnd, &frameClientRect);
				m_preventChildSize++;

				for (int idx = 0; idx < m_windowsAdjust.size(); idx++)
				{
					GetWindowRect(m_windowsAdjust[idx].hWnd, &wndRect);
					
					clientOffset.x = wndRect.left;
					clientOffset.y = wndRect.top;
					ScreenToClient(GetParent(m_windowsAdjust[idx].hWnd), &clientOffset);

					frameClientOffset.x = wndRect.left;
					frameClientOffset.y = wndRect.top;
					ScreenToClient(m_frameHwnd, &frameClientOffset);

					MoveWindow(m_windowsAdjust[idx].hWnd, 0, clientOffset.y, 
							   frameClientRect.right - frameClientRect.left,
						       (frameClientRect.bottom - frameClientRect.top) - frameClientOffset.y, TRUE);

					//UpdateWindow(m_windowsAdjust[idx].hWnd);
				}

				m_preventChildSize--;
			}
				
			// For each window we are going to resize, store the relative width/height info
			for (int idx = 0; idx < m_windowsAdjust.size(); idx++)
			{
				GetWindowRect(m_windowsAdjust[idx].hWnd, &wndRect);

				m_windowsAdjust[idx].deltaWidth = (wndRect.right - wndRect.left) - (frameRect.right - frameRect.left);
				m_windowsAdjust[idx].deltaHeight = (wndRect.bottom - wndRect.top) - (frameRect.bottom - frameRect.top);
			}
			
			break;
		}

		parent = GetParent(parent);
	}
}


//----------------------------------------------------------------------
// This ends all tracking for the specified window
//----------------------------------------------------------------------
void VstWindowSizer::EndTracking(HWND hWnd, bool all)
{
	if (all)
	{
		if (m_editorInfo != NULL)
		{
			// Restore the previous window proc
			if (IsWindow(m_editorInfo->hWnd) && m_editorInfo->prevProc != NULL)
					SetWindowLongPtr(m_editorInfo->hWnd, GWL_WNDPROC, (long)m_editorInfo->prevProc);
		}

		for (int idx = 0; idx < m_windowsAdjust.size(); idx++)
		{
			// Restore the previous window proc
			if (IsWindow(m_windowsAdjust[idx].hWnd) && m_windowsAdjust[idx].prevProc != NULL)
				SetWindowLongPtr(m_windowsAdjust[idx].hWnd, GWL_WNDPROC, (long)m_windowsAdjust[idx].prevProc);
		}

		// We no longer need to resize anything
		m_windowsAdjust.clear();
		m_anchoredWindows.clear();

		m_editorInfo = NULL;

		for (int idx = 0; idx < m_editors.size(); idx++)
		{
			if (m_editors[idx].pSizer == this)
			{
				m_editors.erase(m_editors.begin() + idx);
				idx--;
			}
		}

		// Ensure we do a re-initialize the next time around
		m_trackingInitialized = false;
		
		for (int idx = 0; idx < m_framesTrack.size(); idx++)
		{
			//if (all || m_framesTrack[idx].hWnd == hWnd)
			if (m_framesTrack[idx].pSizer == this)
			{
				// Restore the previous window proc, if any
				if (IsWindow(m_framesTrack[idx].hWnd) && m_framesTrack[idx].prevProc != NULL)
					SetWindowLongPtr(hWnd, GWL_WNDPROC, (long)m_framesTrack[idx].prevProc);
				
					m_framesTrack.erase(m_framesTrack.begin() + idx);
					idx--;
			}
		}
	}
	else
	{
		if (m_editorInfo != NULL && m_editorInfo->hWnd == hWnd)
		{
			// Restore the previous window proc
			if (IsWindow(m_editorInfo->hWnd) && m_editorInfo->prevProc != NULL)
					SetWindowLongPtr(m_editorInfo->hWnd, GWL_WNDPROC, (long)m_editorInfo->prevProc);

			for (int idx = 0; idx < m_windowsAdjust.size(); idx++)
			{
				// Restore the previous window proc
				if (IsWindow(m_windowsAdjust[idx].hWnd) && m_windowsAdjust[idx].prevProc != NULL)
					SetWindowLongPtr(m_windowsAdjust[idx].hWnd, GWL_WNDPROC, (long)m_windowsAdjust[idx].prevProc);
			}

			// We no longer need to resize anything
			m_windowsAdjust.clear();
			m_anchoredWindows.clear();

			m_editorInfo = NULL;

			for (int idx = 0; idx < m_editors.size(); idx++)
			{
				if (m_editors[idx].hWnd == hWnd)
				{
					m_editors.erase(m_editors.begin() + idx);
					break;
				}
			}

			// Ensure we do a re-initialize the next time around
			m_trackingInitialized = false;
		}
		
		for (int idx = 0; idx < m_framesTrack.size(); idx++)
		{
			if (m_framesTrack[idx].hWnd == hWnd)
			{
				// Restore the previous window proc, if any
				if (IsWindow(m_framesTrack[idx].hWnd) && m_framesTrack[idx].prevProc != NULL)
					SetWindowLongPtr(hWnd, GWL_WNDPROC, (long)m_framesTrack[idx].prevProc);
				
				m_framesTrack.erase(m_framesTrack.begin() + idx);
				break;
			}
		}
	}
}


//----------------------------------------------------------------------
// This ensures that the editor window size stays in sync with the parent
//----------------------------------------------------------------------
void VstWindowSizer::TrackParentWindowSize(HWND parentFrame)
{
	RECT frameRect;
	int setWidth;
	int setHeight;

	if (!m_resizeFlags & ResizeFlagTrackParentSize)
		return;

	if (m_preventChildSize > 0)
		return;

	m_preventChildSize++;

	// The parent window's size changed, so adjust all window sizes here
	GetWindowRect(parentFrame, &frameRect);

	for (int idx = 0; idx < m_windowsAdjust.size(); idx++)
	{
		//if (idx == 0 || idx == 1 || idx == 2 || idx == 3)
		//if (idx == 1)
		//	continue;
		
		SetWindowPos(m_windowsAdjust[idx].hWnd, NULL, 0, 0, 
					 (frameRect.right - frameRect.left) + m_windowsAdjust[idx].deltaWidth, 
					 (frameRect.bottom - frameRect.top) + m_windowsAdjust[idx].deltaHeight, 
					 SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	}

	m_preventChildSize--;
}


//----------------------------------------------------------------------
// Keep the m_rect member up to date, and resize all anchored windows
//----------------------------------------------------------------------
void VstWindowSizer::FixupWindowSize()
{
	RECT newRect;
	RECT oldRect;
	RECT setRect;
	int dx;
	int dy;

	GetWindowRect(m_editorInfo->hWnd, &newRect);

	dx = (newRect.right - newRect.left) - (m_rect.right - m_rect.left);
	dy = (newRect.bottom - newRect.top) - (m_rect.bottom - m_rect.top);

	// Now allow the anchored windows to be moved/resized
	for (int idx = 0; idx < m_anchoredWindows.size(); idx++)
		UpdateAnchoredWindow(&m_anchoredWindows[idx]);

	UpdateWindow(m_frameHwnd);
	UpdateWindow(m_editorInfo->hWnd);

	m_rect.right += dx;
	m_rect.bottom += dy;
}


//----------------------------------------------------------------------
// Limit the GUI size the the specified min and max values
//----------------------------------------------------------------------
void VstWindowSizer::ApplyFrameSizeLimits(RECT* pNewRect)
{
	RECT curRect;
	int newWidth, newHeight;

	GetWindowRect(m_frameHwnd, &curRect);

	newWidth = pNewRect->right - pNewRect->left;
	newHeight = pNewRect->bottom - pNewRect->top;

	if (newWidth > m_maxSize.cx)
	{
		if (pNewRect->left != curRect.left)
			pNewRect->left = pNewRect->right - m_maxSize.cx;
		else
			pNewRect->right = pNewRect->left + m_maxSize.cx;
	}
	else if (newWidth < m_minSize.cx)
	{
		if (pNewRect->left != curRect.left)
			pNewRect->left = pNewRect->right - m_minSize.cx;
		else
			pNewRect->right = pNewRect->left + m_minSize.cx;
	}

	if (newHeight > m_maxSize.cy)
	{
		if (pNewRect->top != curRect.top)
			pNewRect->top = pNewRect->bottom - m_maxSize.cy;
		else
			pNewRect->bottom = pNewRect->top + m_maxSize.cy;
	}
	else if (newHeight < m_minSize.cy)
	{
		if (pNewRect->top != curRect.top)
			pNewRect->top = pNewRect->bottom - m_minSize.cy;
		else
			pNewRect->bottom = pNewRect->top + m_minSize.cy;
	}
}


//----------------------------------------------------------------------
// Move and/or resize an anchored window, based on the initial and current parent sizes
//----------------------------------------------------------------------
void VstWindowSizer::UpdateAnchoredWindow(AnchoredWindow* pAnchored)
{
	RECT parentRect;
	RECT newRect = pAnchored->startRect;

	GetClientRect(GetParent(pAnchored->hWnd), &parentRect);

	int dx = (parentRect.right - parentRect.left) - pAnchored->startParentSize.cx;
	int dy = (parentRect.bottom - parentRect.top) - pAnchored->startParentSize.cy;

	if (pAnchored->anchor & AnchorLeft && pAnchored->anchor & AnchorRight)
		newRect.right += dx;
	else if (pAnchored->anchor & AnchorRight)
	{
		newRect.right += dx;
		newRect.left += dx;
	}

	if (pAnchored->anchor & AnchorTop && pAnchored->anchor & AnchorBottom)
		newRect.bottom += dy;
	else if (pAnchored->anchor & AnchorBottom)
	{
		newRect.top += dy;
		newRect.bottom += dy;
	}

	// Not sure if this saves any CPU time or not, but do a check and early exit if nothing changed
	if (newRect.left == pAnchored->startRect.left && 
		newRect.top == pAnchored->startRect.top && 
		newRect.right == pAnchored->startRect.right && 
		newRect.bottom == pAnchored->startRect.bottom)
		return;

	MoveWindow(pAnchored->hWnd, newRect.left, newRect.top, newRect.right - newRect.left, newRect.bottom - newRect.top, TRUE);
	UpdateWindow(pAnchored->hWnd);
}


//----------------------------------------------------------------------
// Detect the current host
//----------------------------------------------------------------------
void VstWindowSizer::DetectHost()
{
	if (m_editorInfo == NULL)
		return;
	
	char szProductName[256];
	
	m_hostApp = HostAppUnknown;
	szProductName[0] = 0;
	
	m_effect->getHostProductString(szProductName);

	// Some hosts return a string that clearly identifies them...
	if (strstr(szProductName, "Cantabile") != NULL)
		m_hostApp = HostAppCantabile;
	else if (strstr(szProductName, "energyXT") != NULL)
		m_hostApp = HostAppEnergyXT;
	else if (strstr(szProductName, "SAVIHost") != NULL)
		m_hostApp = HostAppSaviHost;
	else if (strstr(szProductName, "Melodyne") != NULL)
		m_hostApp = HostAppMelodyne;
	else if (strstr(szProductName, "Cubase") != NULL)
		m_hostApp = HostAppCubase;
	else if (strstr(szProductName, "Tracktion") != NULL)
		m_hostApp = HostAppTracktion;
	else
	{
		// ...others don't. SONAR, Project 5 and FL return the name of the Cakewalk VST wrapper for all 3
		char szClassName[256];
		HWND curHwnd = GetParent(m_editorInfo->hWnd);
		
		m_hostApp = HostAppUnknown;

		while (curHwnd)
		{
			LONG style = GetWindowLong(curHwnd, GWL_STYLE);
			HWND curParent = GetParent(curHwnd);
			szClassName[0] = 0;

			if (!curParent || !(style & WS_CHILD) || ((style & WS_CAPTION) == WS_CAPTION) || (style & (WS_THICKFRAME | WS_POPUP)))
			{
				if (GetClassNameA(curHwnd, (LPSTR)szClassName, 256) > 0)
				{
					if (strstr(szClassName, "Fruity") != NULL)
					{
						m_hostApp = HostAppFruityLoops;
						break;
					}
					
					if (strstr(szClassName, "Project5") != NULL)
					{
						m_hostApp = HostAppProject5;
						break;
					}

					if (strstr(szClassName, "SONAR") != NULL)
					{
						m_hostApp = HostAppSONAR;
						break;
					}
				}
			}

			curHwnd = curParent;
		}
	}

	// Set various flags based on how we need to enable sizing for different hosts
	switch (m_hostApp)
	{
	case HostAppSONAR:
		//m_resizeFlags = ResizeFlagTrackParentSize | ResizeFlagFillOutParents;
		m_resizeFlags = ResizeFlagTrackParentSize | ResizeFlagFillOutParents | ResizeFlagFilterChildWM_SIZE;
		//m_resizeFlags = ResizeFlagTrackParentSize | ResizeFlagFilterChildWM_SIZE;
		break;

	case HostAppProject5:
		m_resizeFlags = ResizeFlagTrackParentSize;
		break;

	case HostAppEnergyXT:
		m_resizeFlags = ResizeFlagTrackParentSize | ResizeFlagSimulateDragEdge;
		break;

	case HostAppFruityLoops:
		m_resizeFlags = ResizeFlagTrackParentSize | ResizeFlagSimulateDragEdge;
		break;

	case HostAppCantabile:
		m_resizeFlags = ResizeFlagTrackParentSize | ResizeFlagSimulateDragEdge;
		break;

	case HostAppSaviHost:
		m_resizeFlags = 0;
		break;

	case HostAppMelodyne:
		m_resizeFlags = ResizeFlagTrackParentSize | ResizeFlagSimulateDragEdge;
		break;

	case HostAppCubase:
		m_resizeFlags = ResizeFlagTrackParentSize | ResizeFlagSimulateDragEdge | ResizeFlagFilterChildWM_SIZE;
		break;

	case HostAppTracktion:
		m_resizeFlags = ResizeFlagTrackParentSize | ResizeFlagSimulateDragEdge | 
						ResizeFlagFilterWM_WINDOWPOSCHANGED | ResizeFlagUpdateSizeOnWM_WINDOWPOSCHANGED;
		break;

	case HostAppUnknown:
		// This is for unknown hosts. Eventually resizing needs to be disabled completely for
		// all unknown hosts, but for now allow it in order to test this on new hosts.
		m_resizeFlags = ResizeFlagTrackParentSize | ResizeFlagSimulateDragEdge;
		break;
	}

	if (m_hostApp == HostAppUnknown)
	{
		// Could not detect the host.  Add a test case for this host!
	}
}


//----------------------------------------------------------------------
// Determine if the mouse is on an edge or corner that can be used for sizing
//----------------------------------------------------------------------
long VstWindowSizer::HitTest()
{
	if (!(m_resizeFlags & ResizeFlagSimulateDragEdge))
		return HTNOWHERE;

	RECT frameRect;
	RECT insideRect;
	POINT mousePos;
	bool left = false;
	bool top = false;
	bool right = false;
	bool bottom = false;

	GetWindowRect(m_frameHwnd, &frameRect);
	GetCursorPos(&mousePos);

	insideRect = frameRect;
	InflateRect(&insideRect, -6, -6);

	if (mousePos.x >= frameRect.left && mousePos.x <= insideRect.left)
		left = true;

	if (mousePos.x <= frameRect.right && mousePos.x >= insideRect.right)
		right = true;

	if (mousePos.y >= frameRect.top && mousePos.y <= insideRect.top)
		top = true;

	if (mousePos.y <= frameRect.bottom && mousePos.y >= insideRect.bottom)
		bottom = true;

	if (!left && !right && !top && !bottom)
		return HTNOWHERE;

	if (left && top)
		return HTTOPLEFT;

	if (left && bottom)
		return HTBOTTOMLEFT;

	if (right && top)
		return HTTOPRIGHT;

	if (right && bottom)
		return HTBOTTOMRIGHT;

	if (left)
		return HTLEFT;

	if (top)
		return HTTOP;

	if (right)
		return HTRIGHT;

	if (bottom)
		return HTBOTTOM;

	return HTNOWHERE;
}


//----------------------------------------------------------------------
// Show the correct cursor, based on the detected edge the mouse is over
//----------------------------------------------------------------------
void VstWindowSizer::UpdateEdgeCursor()
{
	if (!(m_resizeFlags & ResizeFlagSimulateDragEdge))
		return;

	switch (m_sizingCursor)
	{
	case HTTOPLEFT:
		SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
		break;

	case HTTOPRIGHT:
		SetCursor(LoadCursor(NULL, IDC_SIZENESW));
		break;

	case HTBOTTOMLEFT:
		SetCursor(LoadCursor(NULL, IDC_SIZENESW));
		break;

	case HTBOTTOMRIGHT:
		SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
		break;

	case HTLEFT:
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		break;

	case HTTOP:
		SetCursor(LoadCursor(NULL, IDC_SIZENS));
		break;

	case HTRIGHT:
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		break;

	case HTBOTTOM:
		SetCursor(LoadCursor(NULL, IDC_SIZENS));
		break;

	case HTNOWHERE:
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		break;
	}
}

//----------------------------------------------------------------------
// If the mouse was captured, resize the window based on the current edge/corner being dragged
//----------------------------------------------------------------------
bool VstWindowSizer::OnMouseMove()
{
	if (!(m_resizeFlags & ResizeFlagSimulateDragEdge))
		return false;

	if (GetCapture() == m_frameHwnd)
	{
		POINT parentClientOffset;
		HWND frameParent = GetParent(m_frameHwnd);

		RECT frameRect;
		RECT newRect;
		POINT mousePos;
		int dxLeft = 0;
		int dxRight = 0;
		int dyTop = 0;
		int dyBottom = 0;
		
		GetCursorPos(&mousePos);
		parentClientOffset.x = 0;
		parentClientOffset.y = 0;

		if (frameParent)
			ClientToScreen(frameParent, &parentClientOffset);
		
		GetWindowRect(m_frameHwnd, &frameRect);
		newRect = frameRect;

		// Must resize the window here
		switch (m_sizingCursor)
		{
		case HTLEFT:
			newRect.left = mousePos.x - m_resizeOffset.x;
			break;

		case HTTOP:
			newRect.top = mousePos.y - m_resizeOffset.y;
			break;

		case HTRIGHT:
			newRect.right = mousePos.x + m_resizeOffset.x;
			break;

		case HTBOTTOM:
			newRect.bottom = mousePos.y + m_resizeOffset.y;
			break;
		
		case HTTOPLEFT:
			newRect.left = mousePos.x - m_resizeOffset.x;
			newRect.top = mousePos.y - m_resizeOffset.y;
			break;
		
		case HTTOPRIGHT:
			newRect.right = mousePos.x + m_resizeOffset.x;
			newRect.top = mousePos.y - m_resizeOffset.y;
			break;
		
		case HTBOTTOMLEFT:
			newRect.left = mousePos.x - m_resizeOffset.x;
			newRect.bottom = mousePos.y + m_resizeOffset.y;
			break;
		
		case HTBOTTOMRIGHT:
			newRect.right = mousePos.x + m_resizeOffset.x;
			newRect.bottom = mousePos.y + m_resizeOffset.y;
			break;
		}

		// Ensure we do not go over the limits set for the UI
		ApplyFrameSizeLimits(&newRect);
		
		if (!(GetWindowLong(m_frameHwnd, GWL_STYLE) & WS_POPUP))
			ScreenRectToClient(frameParent, &newRect);

		if (newRect.left != frameRect.left || 
			newRect.top != frameRect.top || 
			newRect.right != frameRect.right || 
			newRect.bottom != frameRect.bottom)
		{
			MoveWindow(m_frameHwnd, newRect.left, newRect.top, newRect.right - newRect.left, newRect.bottom - newRect.top, TRUE);
		}
	}
	else
		m_sizingCursor = HitTest();
	
	UpdateEdgeCursor();

	return m_sizingCursor != HTNOWHERE;
}


//----------------------------------------------------------------------
// Enter sizing mode if the mouse is on a drag edge/corner
//----------------------------------------------------------------------
bool VstWindowSizer::OnButtonDown()
{
	if (!(m_resizeFlags & ResizeFlagSimulateDragEdge))
		return false;

	RECT frameRect;
	POINT mousePos;

	m_sizingCursor = HitTest();

	if (m_sizingCursor == HTNOWHERE)
		return false;
	
	GetCursorPos(&mousePos);
	GetWindowRect(m_frameHwnd, &frameRect);
	SetCapture(m_frameHwnd);
	UpdateEdgeCursor();
	
	switch (m_sizingCursor)
	{
	case HTTOPLEFT:
		m_resizeOffset.x = mousePos.x - frameRect.left;
		m_resizeOffset.y = mousePos.y - frameRect.top;
		break;

	case HTTOPRIGHT:
		m_resizeOffset.x = frameRect.right - mousePos.x;
		m_resizeOffset.y = mousePos.y - frameRect.top;
		break;

	case HTBOTTOMLEFT:
		m_resizeOffset.x = mousePos.x - frameRect.left;
		m_resizeOffset.y = frameRect.bottom - mousePos.y;
		break;

	case HTBOTTOMRIGHT:
		m_resizeOffset.x = frameRect.right - mousePos.x;
		m_resizeOffset.y = frameRect.bottom - mousePos.y;
		break;

	case HTLEFT:
		m_resizeOffset.x = mousePos.x - frameRect.left;
		break;

	case HTTOP:
		m_resizeOffset.y = mousePos.y - frameRect.top;
		break;

	case HTRIGHT:
		m_resizeOffset.x = frameRect.right - mousePos.x;
		break;

	case HTBOTTOM:
		m_resizeOffset.y = frameRect.bottom - mousePos.y;
		break;
	}

	return m_sizingCursor != HTNOWHERE;
}


//----------------------------------------------------------------------
// Exit sizing mode
//----------------------------------------------------------------------
bool VstWindowSizer::OnButtonUp()
{
	if (!(m_resizeFlags & ResizeFlagSimulateDragEdge) || GetCapture() != m_frameHwnd)
		return false;

	ReleaseCapture();
	m_sizingCursor = HitTest();
	UpdateEdgeCursor();

	return m_sizingCursor != HTNOWHERE;
}


//----------------------------------------------------------------------
// Since we cannot use GWL_USERDATA with windows we do not own, instead we use a static
// list of objects to determine the VstWindowSizer instance
//----------------------------------------------------------------------
VstWindowSizer* VstWindowSizer::GetSizer(HWND hWnd)
{
	for (int idx = 0; idx < m_editors.size(); idx++)
	{
		if (m_editors[idx].hWnd == hWnd)
			return m_editors[idx].pSizer;
	}

	return NULL;
}


//----------------------------------------------------------------------
// Convert a client rect to a screen rect
//----------------------------------------------------------------------
void VstWindowSizer::ClientRectToScreen(HWND hWnd, RECT* pRect)
{
	POINT clientOffset = {0};

	ClientToScreen(hWnd, &clientOffset);
	OffsetRect(pRect, clientOffset.x, clientOffset.y);
}


//----------------------------------------------------------------------
// Convert a screen rect to a client rect
//----------------------------------------------------------------------
void VstWindowSizer::ScreenRectToClient(HWND hWnd, RECT* pRect)
{
	POINT screenOffset = {0};

	ScreenToClient(hWnd, &screenOffset);
	OffsetRect(pRect, screenOffset.x, screenOffset.y);
}


//----------------------------------------------------------------------
// We hook into the editor window procedure to intercept the messages we need to make this work
//----------------------------------------------------------------------
LONG WINAPI VstWindowSizer::EditorWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	VstWindowSizer* pSizer = GetSizer(hwnd);
	
	switch(message)
	{
	case WM_SIZE:
		if (pSizer)
			pSizer->FixupWindowSize();
		break;

	case WM_LBUTTONDOWN:
		if (pSizer && pSizer->OnButtonDown())
			return 0;
		break;

	case WM_LBUTTONUP:
		if (pSizer && pSizer->OnButtonUp())
			return 0;
		break;
	
	case WM_MOUSEMOVE:
		if (pSizer && pSizer->OnMouseMove())
			return 0;
		break;
	
	case WM_CAPTURECHANGED:
		if (pSizer)
		{
			// We are no longer sizing, but ensure we display the correct cursor
			pSizer->m_sizingCursor = pSizer->HitTest();
			pSizer->UpdateEdgeCursor();
		}
		break;

	case WM_PAINT:
		if (pSizer && !pSizer->m_trackingInitialized)
		{
			// We use the initial WM_PAINT message to set up tracking since 
			// window sizes/positions have "stabilized" at this point
			pSizer->SetupTracking();
			pSizer->m_trackingInitialized = true;
		}
		break;

	case WM_DESTROY:
		if (pSizer)
			pSizer->EndTracking(hwnd, false);
		break;
	}
	
	if (pSizer && pSizer->m_editorInfo && pSizer->m_editorInfo->prevProc)
		return CallWindowProc((WNDPROC)pSizer->m_editorInfo->prevProc, hwnd, message, wParam, lParam);
	else
		return DefWindowProc(hwnd, message, wParam, lParam);
}


//----------------------------------------------------------------------
// We hook into the editor's frame window procedure to intercept needed messages
//----------------------------------------------------------------------
LONG WINAPI VstWindowSizer::FrameWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//char szMessage[256];
	//sprintf (szMessage, "Message: %0X\n", message);
	//OutputDebugString(szMessage);

	for (int idx = 0; idx < m_framesTrack.size(); idx++)
	{
		if (m_framesTrack[idx].hWnd == hwnd)
		{
			WindowInfo* pTrack = &m_framesTrack[idx];

			//if (pTrack->pSizer->m_preventChildSize)
			//	return TRUE;

			switch (message)
			{
			case WM_SIZING:
				pTrack->pSizer->ApplyFrameSizeLimits((RECT*)lParam);
				return TRUE;
				break;

			case WM_SIZE:
				pTrack->pSizer->TrackParentWindowSize(hwnd);
				if (pTrack->pSizer->m_resizeFlags & ResizeFlagFilterWM_SIZING)
					return TRUE;
				break;

			case WM_LBUTTONDOWN:
			case WM_NCLBUTTONDOWN:
				if (pTrack->pSizer->OnButtonDown())
					return 0;
				break;

			case WM_LBUTTONUP:
			case WM_NCLBUTTONUP:
				if (pTrack->pSizer->OnButtonUp())
					return 0;
				break;
			
			case WM_MOUSEMOVE:
			case WM_NCMOUSEMOVE:
				if (pTrack->pSizer->OnMouseMove())
					return 0;
				break;

			case WM_CLOSE:
				//pTrack->pSizer->EndTracking(hwnd, false);
				pTrack->pSizer->EndTracking(NULL, true);
				break;

			case WM_DESTROY:
				//pTrack->pSizer->EndTracking(hwnd, false);
				pTrack->pSizer->EndTracking(NULL, true);
				break;

			case WM_WINDOWPOSCHANGING:
				return 0;

			case WM_WINDOWPOSCHANGED:
				if (pTrack->pSizer->m_resizeFlags & ResizeFlagUpdateSizeOnWM_WINDOWPOSCHANGED)
					pTrack->pSizer->TrackParentWindowSize(hwnd);
				if (pTrack->pSizer->m_resizeFlags & ResizeFlagFilterWM_WINDOWPOSCHANGED)
					return 0;
				break;
			}
			
			return CallWindowProc((WNDPROC)pTrack->prevProc, hwnd, message, wParam, lParam);
		}
	}
	
	// Should hopefully never get here
	return DefWindowProc(hwnd, message, wParam, lParam);
}


//----------------------------------------------------------------------
// Some hosts like Cubase has one of the intermediate parent windows of the editor resize it's 
// parent when it gets resized.  This interferes with our method here, so we stop it from doing that
//----------------------------------------------------------------------
LONG WINAPI VstWindowSizer::ChildWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	for (int idx = 0; idx < m_framesTrack.size(); idx++)
	{
		WindowInfo* pTrack = &m_framesTrack[idx];

		for (int childIdx = 0; childIdx < pTrack->pSizer->m_windowsAdjust.size(); childIdx++)
		{
			if (pTrack->pSizer->m_windowsAdjust[childIdx].hWnd == hwnd)
			{
				switch (message)
				{
					case WM_SIZE:
						if (pTrack->pSizer->m_preventChildSize > 0)
							return 0;
						break;

					case WM_CLOSE:
						pTrack->pSizer->EndTracking(hwnd, false);
						break;

					case WM_DESTROY:
						pTrack->pSizer->EndTracking(hwnd, false);
						break;
				}

				if (pTrack->prevProc)
					return CallWindowProc((WNDPROC)pTrack->prevProc, hwnd, message, wParam, lParam);
				else
					return DefWindowProc(hwnd, message, wParam, lParam);
			}
		}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
