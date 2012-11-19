
#ifdef _RT_AUDIO

#include "Synth.h"
#include "GUI/Editor.h"


//DWORD WINAPI CpuPercentageThreadProc(LPVOID lpParam);
/*
#include "CpuPercentage.h"


CpuPercentage percentage;


DWORD WINAPI CpuPercentageThreadProc(LPVOID lpParam)
{
	while (true)
	{
		short percent = percentage.getPercentage();

		TRACE("Thread id %d: %d%% cpu usage\n", ::GetCurrentThreadId(), percent);
		Sleep(1000);
	}
}
*/




//-------------------------------------------------------
// WinMain
//-------------------------------------------------------

int WINAPI _tWinMain( HINSTANCE hInst, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
    try 
    {
        //CreateThread(NULL, 0, CpuPercentageThreadProc, NULL, 0, NULL);
		
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
