//-------------------------------------------------------------------------------------------------------
// VST Plug-in Window Sizer
// Version 0.1
// Steven Schulze
// vstsizer@alienworks.com

#pragma once
#include <vector>


class VstWindowSizer
{
public:
	struct WindowInfo
	{
	public:
		HWND hWnd;
		int deltaWidth;
		int deltaHeight;
		LONG_PTR prevProc;
		VstWindowSizer* pSizer;
	};

	struct AnchoredWindow
	{
		HWND hWnd;
		int anchor;
		RECT startRect;
		SIZE startParentSize;
	};

	enum HostApp
	{
		HostAppUnknown,
		HostAppSONAR,
		HostAppEnergyXT,
		HostAppProject5,
		HostAppFruityLoops,
		HostAppCantabile,
		HostAppSaviHost,
		HostAppMelodyne,
		HostAppCubase,
		HostAppTracktion,
	};

	enum ResizeFlags
	{
		ResizeFlagNone = 0,
		ResizeFlagTrackParentSize = 1,
		ResizeFlagSimulateDragEdge = 1 << 1,
		ResizeFlagFillOutParents = 1 << 2,
		ResizeFlagFilterWM_SIZING = 1 << 3,
		ResizeFlagFilterChildWM_SIZE = 1 << 4,
		ResizeFlagFilterWM_WINDOWPOSCHANGED = 1 << 5,
		ResizeFlagUpdateSizeOnWM_WINDOWPOSCHANGED = 1 << 6,
	};

	enum AnchorFlags
	{
		AnchorNone = 0,
		AnchorLeft = 1 << 0,
		AnchorRight = 1 << 1,
		AnchorTop = 1 << 2,
		AnchorBottom = 1 << 3,
	};

protected:
	std::vector<WindowInfo> m_windowsAdjust;
	std::vector<AnchoredWindow> m_anchoredWindows;
	HWND m_frameHwnd;
	RECT m_rect;
	WindowInfo* m_editorInfo;
	bool m_trackingInitialized;
	HostApp m_hostApp;
	AudioEffectX* m_effect;
	bool m_hostCanResize;
	long m_sizingCursor;
	int m_resizeFlags;
	POINT m_startMouseOffset;
	POINT m_resizeOffset;
	SIZE m_minSize;
	SIZE m_maxSize;
	int m_preventChildSize;

	static std::vector<WindowInfo> m_editors;
	static std::vector<WindowInfo> m_framesTrack;

public:
	VstWindowSizer(void);
	~VstWindowSizer(void);

	void SetEffect(AudioEffectX* pEffect);			  // Call this to set the editor
	void SetEditorHwnd(HWND hWndEditor);			  // This sets the editor window that needs to be sized
	void SetMinSize(int minWidth, int minHeight);	  // Set the minimum sizing limits
	void SetMaxSize(int maxWidth, int maxHeight);	  // Sets the maximum sizing limits
	SIZE GetMinSize() { return m_minSize; }			  // Get the minimum sizing limits
	SIZE GetMaxSize() { return m_maxSize; }			  // Get the maximum sizing limits
	SIZE GetCurrentSize();							  // Get the current size
	void SetAnchoredWindow(HWND hWnd, int anchor);	  // Add a child window to auto-size during plugin resizing

protected:
	void DetectHost();
	void SetupTracking();
	void EndTracking(HWND hWnd, bool all);
	void FixupWindowSize();
	void ApplyFrameSizeLimits(RECT* pNewRect);
	void TrackParentWindowSize(HWND parentFrame);
	void UpdateAnchoredWindow(AnchoredWindow* pAnchored);
	void ClientRectToScreen(HWND hWnd, RECT* pRect);
	void ScreenRectToClient(HWND hWnd, RECT* pRect);

	long HitTest();
	void UpdateEdgeCursor();
	bool OnMouseMove();
	bool OnButtonDown();
	bool OnButtonUp();

	static VstWindowSizer* GetSizer(HWND hWnd);
	
	static LONG WINAPI EditorWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LONG WINAPI FrameWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LONG WINAPI ChildWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};
