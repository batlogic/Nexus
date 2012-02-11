
#include "Clipboard.h"



void Clipboard::copyText( HWND hwnd, ostringstream& os )
{
    string s   = os.str();
    UINT16 len = s.length() + 1;
    
    if( OpenClipboard( hwnd ) )
    {
        EmptyClipboard();
        HGLOBAL hClipboardData;
        hClipboardData = GlobalAlloc( GMEM_DDESHARE, len );

        char* buf;
        buf = (char*)GlobalLock( hClipboardData );
        strncpy_s( buf, len, LPCSTR( s.c_str() ), len );

        GlobalUnlock( hClipboardData );
        SetClipboardData( CF_TEXT, hClipboardData );
        CloseClipboard();
    }
}


bool Clipboard::pasteText( HWND hwnd, istringstream& is )
{
    bool result = false;

    if( OpenClipboard( hwnd )) 
    {
        if( IsClipboardFormatAvailable( CF_TEXT ) || IsClipboardFormatAvailable( CF_OEMTEXT ))
        {
            HANDLE hClipboardData = GetClipboardData( CF_TEXT );
            char* buf = (char*)GlobalLock( hClipboardData );

            is.str( buf );
            result = true;
        }
        CloseClipboard();
    } 
    return result;
}


bool Clipboard::isTextAvailable()
{
    return IsClipboardFormatAvailable( CF_TEXT ) || IsClipboardFormatAvailable( CF_OEMTEXT );
}

