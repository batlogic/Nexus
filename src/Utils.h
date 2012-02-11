#ifndef UTILS_H
#define UTILS_H

#include "Definitions.h"


class Utils
{
public:
	
    // Converts pitch number to frequency
	static double pitch2freq( double pitch ) 
    {
        return (440.0 * pow( 2.0, (pitch - 69.0) / 12.0 ));
        //return pow( 1.0594630943593, pitch ) * 8.17742;  
        //return pow( 1.05946, pitch ) * 8.17742;  

        //return pow( 2, pitch/12 );
        //return log10( pitch / 12.0 ) / log10( 2.0 );
	}

    // Converts frequency to pitch number
    static double freq2pitch( double freq )
    {
        //return 12 * ( log10( freq / 440.0 ) / log10( 2.0 )) + 69;
        return log10( freq / 8.17742 ) / log10( 1.0594630943593 );
    }

	///< Stuffs \e text with an amplitude on the [0.0, 1.0] scale converted to its value in decibels.
	static void dB2string( FLOAT value, char* text, UINT32 maxLen );		

	///< Stuffs \e text with the frequency in Hertz that has a period of \e samples.
	static void hz2string( FLOAT samples, char* text, UINT32 maxLen, FLOAT sampleRate );		
	
	///< Stuffs \e text with the duration in milliseconds of \e samples frames.
	static void ms2string( FLOAT samples, char* text, UINT32 maxLen, FLOAT sampleRate );		

	///< Stuffs \e text with a string representation on the floating point \e value.
	static void float2string( FLOAT value, char* text, UINT32 maxLen );	

	///< Stuffs \e text with a string representation on the integer \e value.
	static void int2string( INT32 value, char* text, UINT32 maxLen );	

    static int str2int( const string& str );
    static string tchar2string( LPCTSTR t );
    static void tokenize( const string& input, const string& delimiters, vector< string >& tokens );
        
	template< typename T >
	static bool isInfPos( T value );

	template< typename T >
	static bool isNAN( T value );

	// byte-swaps a 16-bit data type.
	static void swap16( unsigned char* ptr );

	// byte-swaps a 32-bit data type.
	static void swap32( unsigned char* ptr );

	// byte-swaps a 64-bit data type.
	static void swap64( unsigned char* ptr );

    // Combines the first 14 bits of value1 with the first 14 bits of value2,
    // as used by pitchbend messages.
    static UINT16 combine14Bits( UINT16 value1, UINT16 value2 );
};


template< typename T >
inline bool Utils::isInfPos( T value )
{
	return numeric_limits<T>::has_infinity && value == numeric_limits<T>::infinity();
}


template< typename T >
inline bool Utils::isNAN( T value )
{
	return value != value;
}



#endif // UTILS_H