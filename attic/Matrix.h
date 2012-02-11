
#ifndef MATRIX_H
#define MATRIX_H

#include "Definitions.h"
#include "Buffer.h"


template < class T >
class Matrix : public Buffer<T>
{
public:
	Matrix( size_t numCols=0, size_t numRows=0 )
	: Buffer( numCols*numRows ),
	  numCols_( numCols ),
	  numRows_( numRows )
	{}


	T* layout( size_t numCols, size_t numRows )
	{
		numCols_ = numCols;
		numRows_ = numRows;

		return resize( numCols * numRows );
	}


	T* appendRow()
	{
		numRows_++;
		size_ = numRows_ * numCols_;

		if( data_ == NULL )	{
			resize( size_ );
		} 
		else {
			T* oldData = data_;
			data_      = allocate( size_ );

			memcpy( data_, oldData, (numRows_-1) * numCols_ * sizeof( T ));
			free( oldData );
		}
		return data_;
	}

private:
	size_t numCols_;
	size_t numRows_;
};





//template < class T >
//class Matrix : public vector< vector< T >>
//{
//public:
//	typedef typename vector<T>::size_type size_type;
//
//	Matrix( size_type rows = 0, size_type cols = 0 )
//		: vector< vector< T >>( cols, vector<T>( rows ))
//	{}
//
//	void appendRow();
//	void resizeRows( size_type rows );
//	void resizeRow( size_type rows, size_type col );
//
//	void appendCol( size_type rows );
//	void appendCol( vector<T>* col );
//	void appendElement( size_type col, T value );
//	void reverseCols();
//
//	size_type numCols()    { return size(); }
//};
//
//
//template< class T >
//void Matrix<T>::appendCol( size_type rows )
//{
//	vector<T> v = vector<T>( rows );
//	push_back( v );
//}
//
//
//template< class T >
//void Matrix<T>::appendCol( vector<T>* v )
//{
//	size_type size = v->size();
//	appendCol( size );
//
//	for( size_type i=0; i<size; i++ ) {
//		back().operator[](i) = v->at( i );
//	}
//}
//
//
//template< class T >
//void Matrix<T>::appendElement( size_type col, T value )
//{
//	operator[]( col ).push_back( value );
//}
//
//
//
//template< class T >
//void Matrix<T>::resizeRows( size_type rows )
//{
//	for( UINT16 i = 0; i < size(); i++ ) {
//		resizeRow( rows, i );
//	}
//}
//
//
//template< class T >
//void Matrix<T>::resizeRow( size_type rows, size_type col )
//{
//	vector<T>* v = &at( col );
//	v->resize( rows );
//}
//
//
//template< class T >
//void Matrix<T>::reverseCols()
//{
//	reverse( begin(), end() );
//}

#endif // MATRIX_H