
#include "Buffer.h"


//template < class T >
//Buffer<T>::Buffer( UINT16 size ) : 
//	data_( NULL ), 
//	size_( size ),
//	bufferSize_( size )
//{
//	data_ = allocate( size_ );
//}


//template < class T >
//Buffer<T>::Buffer( const FLOAT& value, UINT16 size ) :
//	data_( NULL ), 
//	size_( size ),
//	bufferSize_( size )
//{
//	data_ = allocate( size_ );
//
//	if( data_ != NULL ) {
//		for( long i=0; i<(long)size_; i++ ) {
//			data_[i] = value;
//		}
//	}
//}


//template < class T >
//Buffer<T>::~Buffer()
//{
//	freeBuffer();
//}


//template < class T >
//T* Buffer<T>::allocate( UINT16 size )
//{
//	//FLOAT* ptr = NULL;
//	T* ptr = NULL;
//
//	if( size > 0 ) 
//	{
//		//ptr = (FLOAT*)malloc( size_* sizeof( FLOAT ));
//		//ptr = (FLOAT*)calloc( size, sizeof( FLOAT ));
//		ptr = (T*)calloc( size, sizeof( T ));
//		ASSERT( ptr != NULL );
//	}
//	return ptr;
//}


//template < class T >
//void Buffer<T>::freeBuffer()
//{
//	if( data_ ) {
//		::free( data_ );
//	}
//}


//template < class T >
//T* Buffer<T>::resize( size_t size )
//{
//	size_ = size;
//	if( size_ > bufferSize_ ) 
//	{
//		freeBuffer();
//		data_ = allocate( size_ );
//		bufferSize_ = size_;
//	}
//	return data_;
//}


//template < class T >
//T* Buffer<T>::resize( size_t size, FLOAT value )
//{
//	resize( size );
//
//	for( size_t i=0; i<size_; i++ ) {
//		data_[i] = value;
//	}
//	return data_;
//}


//template < class T >
//bool Buffer<T>::empty() const
//{
//	if ( size_ > 0 ) return false;
//	else return true;
//}


