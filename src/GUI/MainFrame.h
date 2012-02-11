
#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "../Definitions.h"
#include "../VST/aeffguieditor.h"


class ChildFrame;
class AppOptions;


class MainFrame
{
friend class RtEditor;
public:
    MainFrame();

    int create( HINSTANCE hInstance, AppOptions* options );
    int show( ChildFrame* childFrame );
    long close();
    long WindowProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

    static long makeButtonCode( UINT wParam, bool doubleClick );
    void getClientRect( CRect& rcSize );

    HWND hwnd_;
    ChildFrame* childFrame_;
    AppOptions* options_;

protected:
    long onChar( WPARAM nChar );
    long onKeyDown( WPARAM nChar );
    long onKeyUp( UINT nChar );
    long onWheel( WPARAM wParam, LPARAM lParam );
    long onSetCursor( WPARAM wParam, LPARAM lParam );
    long onSize( WPARAM wParam, LPARAM lParam );
    long onMove( LPARAM lParam );
    long onGetMinMaxInfo( LPARAM lParam );
    long onPaint( HWND hwnd );
    long onNcPaint();
    long onNcButton( UINT msg, LPARAM lParam );

    void drawNc();
    void storeWindowRect();
    
    WNDCLASSEX wc_;
    static TCHAR className_[];
};






#endif // MAINFRAME_H