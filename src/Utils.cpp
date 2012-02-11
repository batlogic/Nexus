

#include "Utils.h"

#pragma warning (disable: 4996)


//-------------------------------------------------------------------------------------------------------
// Strings Conversion
//-------------------------------------------------------------------------------------------------------
/*!
	\param value Value to convert
	\param text	String up to length char
	\param maxLen Maximal length of the string
*/
void Utils::dB2string( FLOAT value, char* text, UINT32 maxLen )
{
	if (value <= 0) {
		strncpy(text, "-oo", maxLen);
	} else {
		float2string ((FLOAT)(20. * log10 (value)), text, maxLen);
	}
}


//-------------------------------------------------------------------------------------------------------
/*!
	\param samples Number of samples
	\param text	String up to length char
	\param maxLen Maximal length of the string
	\param sampleRate current sample rate
*/
void Utils::hz2string( FLOAT samples, char* text, UINT32 maxLen, FLOAT sampleRate )
{
	if( !samples ) {
		float2string( 0, text, maxLen );
	} else {
		float2string( sampleRate / samples, text, maxLen );
	}
}

//-------------------------------------------------------------------------------------------------------
/*!
	\param samples Number of samples
	\param text	String up to length char
	\param maxLen Maximal length of the string
	\param sampleRate current sample rate
*/
void Utils::ms2string( FLOAT samples, char* text, UINT32 maxLen, FLOAT sampleRate )
{
	float2string( (FLOAT)(samples * 1000. / sampleRate), text, maxLen );
}

//-------------------------------------------------------------------------------------------------------
/*!
	\param value Value to convert
	\param text	String up to length char
	\param maxLen Maximal length of the string
*/
void Utils::float2string( FLOAT value, char* text, UINT32 maxLen )
{
	UINT32 c = 0, neg = 0;
	char string[32];
	char* s;
	double v, integ, i10, mantissa, m10, ten = 10.;
	
	v = (double)value;
	if (v < 0)
	{
		neg = 1;
		value = -value;
		v = -v;
		c++;
		if (v > 9999999.)
		{
			strncpy( string, "Huge!", 31 );
			return;
		}
	}
	else if (v > 99999999.)
	{
		strncpy( string, "Huge!", 31 );
		return;
	}

	s = string + 31;
	*s-- = 0;
	*s-- = '.';
	c++;
	
	integ = floor (v);
	i10 = fmod (integ, ten);
	*s-- = (char)((UINT32)i10 + '0');
	integ /= ten;
	c++;
	while (integ >= 1. && c < 8)
	{
		i10 = fmod (integ, ten);
		*s-- = (char)((UINT32)i10 + '0');
		integ /= ten;
		c++;
	}
	if (neg)
		*s-- = '-';
	strncpy( text, s + 1, maxLen);
	if (c >= 8)
		return;

	s = string + 31;
	*s-- = 0;
	mantissa = fmod (v, 1.);
	mantissa *= pow (ten, (double)(8 - c));
	while (c < 8)
	{
		if (mantissa <= 0)
			*s-- = '0';
		else
		{
			m10 = fmod (mantissa, ten);
			*s-- = (char)((UINT32)m10 + '0');
			mantissa /= 10.;
		}
		c++;
	}
	strncat (text, s + 1, maxLen);
}

//-------------------------------------------------------------------------------------------------------
/*!
	\param value Value to convert
	\param text	String up to length char
	\param maxLen Maximal length of the string
*/
void Utils::int2string( INT32 value, char* text, UINT32 maxLen )
{
	if( value >= 100000000 )
	{
		strncpy( text, "Huge!", maxLen );
		return;
	}

	if( value < 0 )
	{
		strncpy( text, "-", maxLen );
		value = -value;
	}
	else
		strncpy( text, "", maxLen );

	bool state = false;
	for( UINT32 div = 100000000; div >= 1; div /= 10 )
	{
		UINT32 digit = value / div;
		value -= digit * div;
		if( state || digit > 0 )
		{
			char temp[2] = {'0' + (char)digit, '\0'};
			strncat( text, temp, maxLen );
			state = true;
		}
	}
}


int Utils::str2int( const string& str ) 
{
	istringstream iss( str );
	int temp;
	iss >> temp;
	return temp;
}


void Utils::tokenize( const string& input, const string& delimiters, vector< string >& tokens )
{
    string::size_type last_pos = 0;
    string::size_type pos = 0; 

    while( true )
    {
        pos = input.find_first_of( delimiters, last_pos );
        if( pos == string::npos )
        {
            string token = input.substr( last_pos );
            if( token.empty() == false )
                tokens.push_back( token );
            break;
        }
        else
        {
            tokens.push_back( input.substr( last_pos, pos - last_pos ));
            last_pos = pos + 1;
        } 
    }
}


string Utils::tchar2string( LPCTSTR t )
{
	string result;

	int len = 1 + wcstombs( NULL, t, 0 );
	if( len == 0 ) return result;

	char* c = new char[ len ];
	if( c == NULL ) throw std::bad_alloc();
	c[0] = '\0';

	wcstombs( c, t, len );
	result = c;
	delete[] c;
	return result;
}


//-----------------------------------------------------------------------
//  Numeric
//------------------------------------------------------------------------

void Utils::swap16(unsigned char* ptr)
{
	register unsigned char val;

	// Swap 1st and 2nd bytes
	val = *(ptr);
	*(ptr) = *(ptr+1);
	*(ptr+1) = val;
}


void Utils::swap32(unsigned char* ptr)
{
	register unsigned char val;

	// Swap 1st and 4th bytes
	val = *(ptr);
	*(ptr) = *(ptr+3);
	*(ptr+3) = val;

	//Swap 2nd and 3rd bytes
	ptr += 1;
	val = *(ptr);
	*(ptr) = *(ptr+1);
	*(ptr+1) = val;
}


void Utils::swap64(unsigned char* ptr)
{
	register unsigned char val;

	// Swap 1st and 8th bytes
	val = *(ptr);
	*(ptr) = *(ptr+7);
	*(ptr+7) = val;

	// Swap 2nd and 7th bytes
	ptr += 1;
	val = *(ptr);
	*(ptr) = *(ptr+5);
	*(ptr+5) = val;

	// Swap 3rd and 6th bytes
	ptr += 1;
	val = *(ptr);
	*(ptr) = *(ptr+3);
	*(ptr+3) = val;

	// Swap 4th and 5th bytes
	ptr += 1;
	val = *(ptr);
	*(ptr) = *(ptr+1);
	*(ptr+1) = val;
}


UINT16 Utils::combine14Bits( UINT16 value1, UINT16 value2 )
{
    UINT16 _14bit;
    _14bit = value2;
    _14bit <<= 7;
    _14bit |= value1;

    return _14bit;
}

