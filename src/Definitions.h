
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define UNICODE
#define _UNICODE

//#define WINDOWS 1
//#define WIN32_LEAN_AND_MEAN 1


#include "basetsd.h"
#include "windows.h"
#include <tchar.h>
#include <math.h>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <stack>
#include <iomanip>
#include <fstream>



using std::vector;
using std::map;
using std::queue;
using std::set;
using std::stack;
using std::basic_ostringstream;
using std::ostringstream;
using std::istringstream;
using std::string;
using std::getline;
using std::make_pair;
using std::cout;
using std::cerr;
using std::endl;
using std::dec;
using std::hex;
using std::exception;
using std::runtime_error;
using std::out_of_range;
using std::bind2nd;
using std::equal_to;
using std::numeric_limits;
using std::setprecision;
using std::fixed;
using std::stable_sort;
using std::distance;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::find;


#define SAMPLERATE 44100
#define CONTROLRATE 400
#define NUMPROGRAMS 128
#define NUMPARAMS 0
#define NUMINPUTS 0
#define NUMOUTPUTS 2
#define SKIN_FILENAME "NexusSkins.xml"


#ifdef FLOAT 
#undef FLOAT
#endif

typedef float FLOAT;
typedef float FLOAT32;
typedef double FLOAT64;


// The default real-time audio input and output buffer size.  If
// clicks are occuring in the input and/or output sound stream, a
// larger buffer size may help.  Larger buffer sizes, however, produce
// more latency.
const unsigned int RT_BUFFER_SIZE = 512;

const FLOAT PI           = 3.14159265358979f;
const FLOAT TWO_PI       = 2 * PI;
const FLOAT ONE_OVER_128 = 0.0078125;


enum StringLengths
{
	MAX_PROG_NAME_LEN   = 24,	///< used for #effGetProgramName, #effSetProgramName, #effGetProgramNameIndexed
	MAX_PARAM_STR_LEN   = 8,	///< used for #effGetParamLabel, #effGetParamDisplay, #effGetParamName
	MAX_VENDOR_STR_LEN  = 64,	///< used for #effGetVendorString, #audioMasterGetVendorString
	MAX_PRODUCT_STR_LEN = 64,	///< used for #effGetProductString, #audioMasterGetProductString
	MAX_EFFECT_NAME_LEN = 32	///< used for #effGetEffectName
};


enum Tuning {
	EQUAL
};


enum MonitorFlags {
    MonitorVoices = 1,
    MonitorPitch  = 2,
    MonitorGate   = 4,
    MonitorAll    = 7
};


// Calculates number of elements in array.
//
#define ARRAY_LENGTH( x ) (sizeof( x ) / sizeof(*( x )))	

// Returns the name of the given type as string.
//
#define TYPENAME( t ) #t		


// Strips the path off the __FILE__ macro
//
#define __FILE_ONLY__ \
(strrchr(__FILE__,'\\') \
? strrchr(__FILE__,'\\')+1 \
: __FILE__ \
)



// Throws an exception of the given type.
// The text for the exception can be formatted in printf-style.
//
#define EXCEPTION( type, format, ...) \
	char f[300];\
	char b[300];\
	strcpy( f, format );\
	strcat( f, " [" ); \
	strcat( f, #type );\
	strcat( f, " in %s, %s, line %d]\n" );\
	sprintf( b, f, __VA_ARGS__, __FILE_ONLY__, __FUNCTION__, __LINE__ );\
	throw type( b );


// Throws a std::runtime_error if condition fails.
//
#define VERIFY( x ) if( x==false ) { EXCEPTION( runtime_error, "Assertion failed: %s", #x ); }

#ifdef _DEBUG 
#define ASSERT( x ) VERIFY( x )
#else 
#define ASSERT( x )
#endif


#if defined(__WINDOWS_DS__) || defined(__WINDOWS_ASIO__) || defined(__WINDOWS_MM__)
#define __OS_WINDOWS__
#elif defined(__LINUX_OSS__) || defined(__LINUX_ALSA__) || defined(__UNIX_JACK__)
#define __OS_LINUX__
#elif defined(__MACOSX_CORE__) || defined(__UNIX_JACK__)
#define __OS_MACOSX__
#endif


#include "Trace.h"

// Sends a trace message to OutputDebugString.
// The message can be formatted in printf-style.
//
#ifdef _DEBUG
#define TRACE( ... ) Trace::trace( ##__VA_ARGS__ )
#else
//#define TRACE( arg )
#define TRACE( ... ) Trace::trace( ##__VA_ARGS__ )
#endif


#define NEXUS_SMALL_ICON 10
#define NEXUS_LARGE_ICON 11


#endif // DEFINITIONS_H