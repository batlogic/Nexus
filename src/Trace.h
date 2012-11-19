
#ifndef TRACE_H
#define TRACE_H

#include "Definitions.h"


class Trace
{
public:
	static void trace( const char* f, ... )
    {
	    char s[1000];
	    va_list ptr;
	    va_start( ptr, f );
	    vsprintf( s, f, ptr );
        va_end( ptr );

    #ifdef _CONSOLE
        cerr << s;
    #else 
	    basic_ostringstream< TCHAR > os;
	    os << s;
	    ::OutputDebugString( os.str().c_str() );
    #endif
    }
};


#endif // TRACE_H