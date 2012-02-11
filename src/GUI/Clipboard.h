
#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include "../Definitions.h"


class Clipboard
{
public:
    static void copyText( HWND hwnd, ostringstream& os );
    static bool pasteText( HWND hwnd, istringstream& is );
    static bool isTextAvailable();
};




#endif // CLIPBOARD_H