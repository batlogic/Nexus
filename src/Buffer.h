#ifndef BUFFER_H
#define BUFFER_H


//------------------------------------------------------------
// A generic buffer class
//
// A buffer is a plain block of memory. For performance reasons no 
// error checking is applied. It can be resized at runtime.
// The template parameter T specifies the data type for the 
// buffer.
//------------------------------------------------------------

template < class T >
class Buffer
{
public:
	//---------------------------------------------------
	// Constructor
	// Creates a buffer and allocates memory, which is left uninitialized.
	//
	// size_t size	the size of memory to be allocated
	//----------------------------------------------------
	Buffer( size_t size = 0 )
	: data_( NULL ), 
	  size_( size )
	{
		data_ = allocate( size_ );
	}

	//---------------------------------------------------
	// Constructor
	// Creates a buffer, allocates memory and initializes the memory with
	// the specified value.
	//
	// const T& value	the value used to initialize the memory
	// size_t size		the size of memory to be allocated
	//----------------------------------------------------
	Buffer( const T& value, size_t size )
	:	data_( NULL ), 
		size_( size )
	{
		data_ = allocate( size_ );

		if( data_ != NULL ) {
			for( size_t i=0; i<size_; i++ ) {
				data_[i] = value;
			}
		}
	}

	//-------------------------------------------------------
	// Destructor
	// Clears all allocated memory.
	//-------------------------------------------------------
	~Buffer()
	{
		freeBuffer();
	}

	//-------------------------------------------------------
	// Returns the size of the currently allocated memory
	// ------------------------------------------------------
	size_t size() const 
	{ 
		return size_; 
	}

	//------------------------------------------------------
	// Returns true, if the size of the allocated memory is zero.
	//------------------------------------------------------
	bool empty() const
	{
		return size_ == 0;
	}


	T* resize( size_t size )
	{
		if( data_ != NULL )		// preserve data
		{
			T* oldData = data_;
			data_      = allocate( size );

			memcpy( data_, oldData, min( size, size_ ) * sizeof( T ));
			free( oldData );
		}
		else {
			data_ = allocate( size );
		}
		size_ = size;

		return data_;
	}


	T* resize( size_t size, T value )
	{
		resize( size );
		set( value );
		
		return data_;
	}


	void set( T value )
	{
		for( size_t i=0; i<size_; i++ ) {
			data_[i] = value;
		}
	}

    T* removeAt( size_t idx )
    {
		if( data_ != NULL )		// preserve data
		{
            ASSERT( idx >= 0 && idx < size_ );
            T* oldData = data_;
			data_      = allocate( size_ - 1 );

            if( data_ ) {
                memcpy( data_,     oldData,       idx * sizeof( T ));
                memcpy( data_+idx, oldData+idx+1, (size_-(idx+1)) * sizeof( T ));
            }

            --size_;
			free( oldData );
            if( size_ == 0 ) {
                ASSERT( data_ == NULL );
            }
		}
        return data_;
    }


    T* removeRange( size_t idx, size_t length )
    {
		ASSERT( length <= size_-idx-1 );
        length = min( size_-idx-1, length );

        if( data_ != NULL )		// preserve data
		{
            ASSERT( idx >= 0 && idx < size_ );
            T* oldData = data_;
			data_      = allocate( size_ - length );

            if( data_ ) {
                memcpy( data_,     oldData,            idx * sizeof( T ));
                memcpy( data_+idx, oldData+idx+length, (size_-(idx+length)) * sizeof( T ));
            }

            size_ -= length;
			free( oldData );
            if( size_ == 0 ) {
                ASSERT( data_ == NULL );
            }
		}
        return data_;
    }

    T* data_;
	size_t size_;


protected:
	T* allocate( size_t size )
	{
		T* ptr = NULL;

		if( size > 0 ) 
		{
			ptr = (T*)calloc( size, sizeof( T ));
			ASSERT( ptr != NULL );
		}
		return ptr;
	}


	void freeBuffer()
	{
		if( data_ ) {
			free( data_ );
			data_ = NULL;
            size_ = 0;
		}
	}
};

#endif // BUFFER_H
