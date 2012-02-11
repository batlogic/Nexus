
#ifndef PERFORMANCETEST_H
#define PERFORMANCETEST_H

#include "Profiler.h"

#define SIZE 1000

float a[SIZE][SIZE],**aa;
float b[SIZE][SIZE];
std::vector< std::vector< FLOAT >> d;

//-------------------------------------------
// Matrix
//-------------------------------------------

// allocate a float matrix with subscript range m[nrl..nrh][ncl..nch] 


class Matrix
{
public:
	Matrix( UINT32 numRows, UINT32 numCols );
	~Matrix();
	FLOAT** getPointer() { return memory_; }

protected:
	FLOAT** memory_;
};


inline Matrix::Matrix(UINT32 numRows, UINT32 numCols)
{
	UINT32 nrow = numRows+1, ncol = numCols+1;

	/* allocate pointers to rows */
	memory_ = (float**)malloc( (size_t)( nrow * sizeof(float*) ));
	if( memory_ == NULL ) {
		throw std::runtime_error( "allocation failure 1" );
	}

	/* allocate rows and set pointers to them */
	memory_[0] = (float*)malloc( (size_t)((nrow * ncol) * sizeof(float) ));
	if( memory_[0] == NULL ) {
		throw std::runtime_error( "allocation failure 2" );
	}

	for( UINT32 i=1; i<=numRows; i++) {
		memory_[i] = memory_[i-1]+ncol;
	}
}


inline Matrix::~Matrix()
{
	free( (char*)memory_[0] );
	free( (char*)memory_ );
}



void performanceTest()
{
	Profiler profiler;

	aa=(float **) malloc((unsigned) SIZE*sizeof(float*));
	for(int i=0; i<SIZE; i++) {
		aa[i]=a[i];							//a[i] is a pointer to a[i][0]
	}

	std::cout << "write:\n";

	profiler.begin();
	for( int i=0; i<SIZE; i++ ) {
		for( int j=0; j<SIZE; j++ )
		{
			aa[i][j] = 0;
		}
	}
	profiler.endAndPrint( "pointer to rows:    " );

	Matrix matrix( SIZE, SIZE );
	FLOAT** c = matrix.getPointer();
	profiler.begin();
	for( int i=0; i<SIZE; i++ ) {
		for( int j=0; j<SIZE; j++ )
		{
			c[i][j] = 0;
		}
	}
	profiler.endAndPrint( "matrix:             " );
	
	profiler.begin();
	for( int i=0; i<SIZE; i++ ) {
		for( int j=0; j<SIZE; j++ )
		{
			b[i][j] = 0;
		}
	}
	profiler.endAndPrint( "fixed size:         " );

	profiler.begin();
	for( int i=0; i<SIZE; i++ ) 
	{
		std::vector<FLOAT> v;
		d.push_back( v );

		for( int j=0; j<SIZE; j++ )
		{
			d[i].push_back( 0 );
		}
	}
	profiler.endAndPrint( "stl vector:         " );

	std::cout << "\nread:\n";
	float test;

	profiler.begin();
	for( int i=0; i<SIZE; i++ ) {
		for( int j=0; j<SIZE; j++ )
		{
			test = aa[i][j];
		}
	}
	profiler.endAndPrint( "pointer to rows:    " );

	profiler.begin();
	for( int i=0; i<SIZE; i++ ) {
		for( int j=0; j<SIZE; j++ )
		{
			test = c[i][j];
		}
	}
	profiler.endAndPrint( "matrix:             " );
		
	profiler.begin();
	for( int i=0; i<SIZE; i++ ) {
		for( int j=0; j<SIZE; j++ )
		{
			test = b[i][j];
		}
	}
	profiler.endAndPrint( "fixed size:         " );
	
	profiler.begin();
	for( int i=0; i<SIZE; i++ ) {
		for( int j=0; j<SIZE; j++ )
		{
			test = d[i][j];
		}
	}
	profiler.endAndPrint( "stl vector:         " );
}


//float** matrix(long nrh, long nch)
//{
//	long i, nrow = nrh+1, ncol = nch+1;
//	float **m;
//
//	/* allocate pointers to rows */
//	m = (float**)malloc( (size_t)( nrow * sizeof(float*) ));
//	if( m == NULL ) return NULL;
//
//	/* allocate rows and set pointers to them */
//	m[0] = (float*)malloc( (size_t)((nrow * ncol) * sizeof(float) ));
//	if( m[0] == NULL ) return NULL;
//
//	for( i=1; i<=nrh; i++) {
//		m[i] = m[i-1]+ncol;
//	}
//
//	/* return pointer to array of pointers to rows */
//	return m;
//}



//float** matrix(long nrl, long nrh, long ncl, long nch)
//{
//	long i, nrow = nrh-nrl+1, ncol = nch-ncl+1;
//	float **m;
//	/* allocate pointers to rows */
//	m = (float**)malloc( (size_t)( nrow * sizeof(float*) ));
//	if( m == NULL ) return NULL;
//
//	m -= nrl;
//	/* allocate rows and set pointers to them */
//	m[nrl] = (float*)malloc( (size_t)((nrow * ncol) * sizeof(float) ));
//	if( m[nrl] == NULL ) return NULL;
//
//	m[nrl] -= ncl;
//
//	for( i=nrl+1; i<=nrh; i++) {
//		m[i] = m[i-1]+ncol;
//	}
//
//	/* return pointer to array of pointers to rows */
//	return m;
//}
//
//



#endif // PERFORMANCETEST_H