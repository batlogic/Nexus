
#ifndef TRACE_H
#define TRACE_H

#include "Definitions.h"


class Trace
{
public:
	static void trace( const char* f, ... )
    {
	    char s[1000];
	    va_list marker;
	    va_start( marker, f );
	    vsprintf( s, f, marker );

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