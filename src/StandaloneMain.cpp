
#ifdef _RT_AUDIO

#include "Synth.h"
#include "GUI/Editor.h"


//-------------------------------------------------------
// WinMain
//-------------------------------------------------------

int WINAPI _tWinMain( HINSTANCE hInst, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
    try 
    {
        RtEditor editor( hInst );
        Synth synth( &editor ); 
        
        synth.open();
        editor.open();
	}
	catch( const exception& e ) {
		TRACE( e.what() );
		return 1;
	}
    return 0;
}

#endif _RT_AUDIO
