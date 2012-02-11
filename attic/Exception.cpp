
#include "Exception.h"


bool Exception::showWarnings_ = true;
bool Exception::printErrors_ = true;



void Exception::process( Exception::Type type )
{
	process( "", type );
}


void Exception::process( const char* message, Exception::Type type )
{
	string msg( message );
	process( msg, type );
}


void Exception::process( const string& message, Exception::Type type )
{
	if( type == WARNING || type == STATUS ) {
		if ( showWarnings_ ) {
			cerr << message << endl;
		}
	}
	else if (type == DEBUG_WARNING) 
	{
#if defined(_STK_DEBUG_)
		cerr << message << endl;
#endif
	}
	else {
		if( printErrors_ ) {
			cerr << message << endl;
		}
		throw Exception(message, type);
	}
}



