
#include "Buffer.h"


/***************************************************/
/*! \class Buffer
\brief A class to handle vectorized audio data.

This class can hold single- or multi-channel audio data.  The data
type is always FLOAT and the channel format is always
interleaved. In an effort to maintain efficiency, no
out-of-bounds checks are performed in this class unless
_STK_DEBUG_ is defined.

Internally, the data is stored in a one-dimensional C array.  An
indexing operator is available to set and retrieve data values.
Alternately, one can use pointers to access the data, using the
index operator to get an address for a particular location in the
data:

FLOAT* ptr = &myFrames[0];

Possible future improvements in this class could include functions
to convert to and return other data types.

by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

Buffer::Buffer( UINT16 nFrames, UINT16 nChannels ) : 
	data_( NULL ), 
	numFrames_( nFrames ), 
	numChannels_( nChannels )
{
	size_       = numFrames_ * numChannels_;
	bufferSize_ = size_;
	data_       = allocate( size_ );
}


Buffer::Buffer( const FLOAT& value, UINT16 nFrames, UINT16 nChannels ) :
	data_( NULL ), 
	numFrames_( nFrames ), 
	numChannels_( nChannels )
{
	size_ = numFrames_ * numChannels_;
	bufferSize_ = size_;
	data_ = allocate( size_ );

	if( data_ != NULL ) {
		for( long i=0; i<(long)size_; i++ ) {
			data_[i] = value;
		}
	}
}


Buffer::Buffer( const Buffer& f ) :
	data_( NULL ), 
	bufferSize_( 0 )
{
	resize( f.numFrames(), f.channels() );

	for( UINT16 i=0; i<size_; i++ ) {
		data_[i] = f[i];
	}
}


Buffer::~Buffer()
{
	freeBuffer();
}


Buffer& Buffer::operator= ( const Buffer& f )
{
	size_       = 0;
	bufferSize_ = 0;

	resize( f.numFrames(), f.channels() );

	for( UINT16 i=0; i<size_; i++ ) {
		data_[i] = f[i];
	}
	return *this;
}


FLOAT* Buffer::allocate( UINT16 size )
{
	FLOAT* ptr = NULL;

	if( size > 0 ) 
	{
		//ptr = (FLOAT*)malloc( size_* sizeof( FLOAT ));
		ptr = (FLOAT*)calloc( size, sizeof( FLOAT ));
		ASSERT( ptr != NULL );
	}
	return ptr;
}


void Buffer::freeBuffer()
{
	if( data_ ) {
		::free( data_ );
	}
}


void Buffer::resize( size_t nFrames, UINT16 nChannels )
{
	numFrames_ = nFrames;
	numChannels_ = nChannels;

	size_ = numFrames_ * numChannels_;
	if( size_ > bufferSize_ ) 
	{
		freeBuffer();
		data_ = allocate( size_ );
		bufferSize_ = size_;
	}
}


void Buffer::resize( size_t nFrames, UINT16 nChannels, FLOAT value )
{
	resize( nFrames, nChannels );

	for( size_t i=0; i<size_; i++ ) {
		data_[i] = value;
	}
}


FLOAT Buffer::interpolate( FLOAT frame, UINT16 channel ) const
{
	ASSERT( (frame < 0.0 || frame > (FLOAT) ( numFrames_ - 1 ) || channel >= numChannels_ ) == false );

	size_t iIndex = ( size_t ) frame;                    // integer part of index
	FLOAT output, alpha = frame - (FLOAT) iIndex;  // fractional part of index

	iIndex = iIndex * numChannels_ + channel;
	output = data_[ iIndex ];
	if ( alpha > 0.0 )
		output += ( alpha * ( data_[ iIndex + numChannels_ ] - output ) );

	return output;
}

