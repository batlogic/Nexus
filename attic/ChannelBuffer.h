#ifndef BUFFER_H
#define BUFFER_H

#include "Definitions.h"


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

class Buffer
{
public:

	//! The default constructor initializes the frame data structure to size zero.
	Buffer( UINT16 nFrames = 0, UINT16 nChannels = 0 );

	//! Overloaded constructor that initializes the frame data to the specified size with \c value.
	Buffer( const FLOAT& value, UINT16 nFrames, UINT16 nChannels );

	//! The destructor.
	~Buffer();

	// A copy constructor.
	Buffer( const Buffer& f );

	// Assignment operator that returns a reference to self.
	Buffer& operator= ( const Buffer& f );

	//! Subscript operator that returns a reference to element \c n of self.
	/*!
	The result can be used as an lvalue. This reference is valid
	until the resize function is called or the array is destroyed. The
	index \c n must be between 0 and size less one.  No range checking
	is performed unless _STK_DEBUG_ is defined.
	*/
	FLOAT& operator[] ( size_t n );

	//! Subscript operator that returns the value at element \c n of self.
	/*!
	The index \c n must be between 0 and size less one.  No range
	checking is performed unless _STK_DEBUG_ is defined.
	*/
	FLOAT operator[] ( size_t n ) const;

	//! Assignment by sum operator into self.
	/*!
	The dimensions of the argument are expected to be the same as
	self.  No range checking is performed unless _STK_DEBUG_ is
	defined.
	*/
	void operator+= ( Buffer& f );

	//! Assignment by product operator into self.
	/*!
	The dimensions of the argument are expected to be the same as
	self.  No range checking is performed unless _STK_DEBUG_ is
	defined.
	*/
	void operator*= ( Buffer& f );

	//! Channel / frame subscript operator that returns a reference.
	/*!
	The result can be used as an lvalue. This reference is valid
	until the resize function is called or the array is destroyed. The
	\c frame index must be between 0 and frames() - 1.  The \c channel
	index must be between 0 and channels() - 1.  No range checking is
	performed unless _STK_DEBUG_ is defined.
	*/
	FLOAT& operator() ( size_t frame, UINT16 channel );

	//! Channel / frame subscript operator that returns a value.
	/*!
	The \c frame index must be between 0 and frames() - 1.  The \c
	channel index must be between 0 and channels() - 1.  No range checking
	is performed unless _STK_DEBUG_ is defined.
	*/
	FLOAT operator() ( size_t frame, UINT16 channel ) const;

	//! Return an interpolated value at the fractional frame index and channel.
	/*!
	This function performs linear interpolation.  The \c frame
	index must be between 0.0 and frames() - 1.  The \c channel index
	must be between 0 and channels() - 1.  No range checking is
	performed unless _STK_DEBUG_ is defined.
	*/
	FLOAT interpolate( FLOAT frame, UINT16 channel = 0 ) const;

	//! Returns the total number of audio samples represented by the object.
	size_t size() const { return size_; }; 

	//! Returns \e true if the object size is zero and \e false otherwise.
	bool empty() const;

	//! Resize self to represent the specified number of channels and frames.
	/*!
	Changes the size of self based on the number of frames and
	channels.  No element assignment is performed.  No memory
	deallocation occurs if the new size is smaller than the previous
	size.  Further, no new memory is allocated when the new size is
	smaller or equal to a previously allocated size.
	*/
	void resize( size_t nFrames, UINT16 nChannels = 1 );

	//! Resize self to represent the specified number of channels and frames and perform element initialization.
	/*!
	Changes the size of self based on the number of frames and
	channels, and assigns \c value to every element.  No memory
	deallocation occurs if the new size is smaller than the previous
	size.  Further, no new memory is allocated when the new size is
	smaller or equal to a previously allocated size.
	*/
	void resize( size_t nFrames, UINT16 nChannels, FLOAT value );

	//! Return the number of channels represented by the data.
	UINT16 channels( void ) const { return numChannels_; };

	//! Return the number of sample frames represented by the data.
	UINT16 numFrames( void ) const { return numFrames_; };

protected:
	FLOAT* allocate( UINT16 size );
	void freeBuffer();

private:
	FLOAT *data_;
	size_t numFrames_;
	UINT16 numChannels_;
	size_t size_;
	size_t bufferSize_;
};

//-------------------------------------------------------
// inlines
//-------------------------------------------------------

inline bool Buffer :: empty() const
{
	if ( size_ > 0 ) return false;
	else return true;
}


inline FLOAT& Buffer :: operator[] ( size_t n )
{
	ASSERT( n < size_ );
	return data_[n];
}


inline FLOAT Buffer :: operator[] ( size_t n ) const
{
	ASSERT( n < size_ );
	return data_[n];
}


inline FLOAT& Buffer :: operator() ( size_t frame, UINT16 channel )
{
	ASSERT(( frame >= numFrames_ || channel >= numChannels_ ) == false );
	return data_[ frame * numChannels_ + channel ];
}


inline FLOAT Buffer :: operator() ( size_t frame, UINT16 channel ) const
{
	ASSERT(( frame >= numFrames_ || channel >= numChannels_ ) == false );
	return data_[ frame * numChannels_ + channel ];
}


inline void Buffer :: operator+= ( Buffer& f )
{
	ASSERT(( f.numFrames() != numFrames_ || f.channels() != numChannels_ )  == false );

	FLOAT *fptr = &f[0];
	FLOAT *dptr = data_;
	for ( UINT16 i=0; i<size_; i++ )
		*dptr++ += *fptr++;
}


inline void Buffer :: operator*= ( Buffer& f )
{
	ASSERT(( f.numFrames() != numFrames_ || f.channels() != numChannels_ )  == false );

	FLOAT *fptr = &f[0];
	FLOAT *dptr = data_;
	for ( UINT16 i=0; i<size_; i++ )
		*dptr++ *= *fptr++;
}


#endif // BUFFER_H
