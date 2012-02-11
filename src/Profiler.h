
#ifndef PROFILER_H
#define PROFILER_H

#include "Definitions.h"


class Profiler
{
public:
	Profiler();
	void begin();
	double end();
	void print();

	void beginCollecting( UINT32 max );
	void collect( double value );
	void endCollecting();
	void average();

	LONGLONG frequency_, first_, last_;
	double diff_;
    bool running_;

	vector<double> collection_;
	UINT32 collectCounter_;
	UINT32 collectMax_;
};


inline Profiler::Profiler() :
    frequency_( 0 ),
    first_( 0 ),
    last_( 0 ),
    diff_( 0 ),
    running_( false ),
    collectCounter_( 0 ),
    collectMax_( 0 )
{
    if( !QueryPerformanceFrequency( (LARGE_INTEGER*)&frequency_ )) {
		frequency_ = (LONGLONG)0.0;
	}
	begin();
}


inline void Profiler::begin() 
{
    running_ = true;
	QueryPerformanceCounter( (LARGE_INTEGER*)&first_ );
}


inline double Profiler::end() 
{
	QueryPerformanceCounter( (LARGE_INTEGER*)&last_ );
    running_ = false;
	diff_    = (((double)( last_ - first_ )) / ((double) frequency_ )); 
	return diff_;
}


inline void Profiler::print()
{
	TRACE( "%f sec\n", diff_ );
}


inline void Profiler::beginCollecting( UINT32 max ) 
{ 
	if( collectMax_ == 0 ) {
        collection_.resize( max ); 
	    collectMax_ = max;
    }
}



inline void Profiler::collect( double value )
{
	if( collectCounter_ < collectMax_ )	{
		collection_[ collectCounter_++ ] = value;
	}
}


inline void Profiler::endCollecting()
{
	for( UINT32 i=0; i<collectCounter_; i++ )
	{
		TRACE( "%d: %d\n", i, collection_[i ] );
	}
	collection_.resize( 0 );
	collectMax_ = 0;
}


inline void Profiler::average()
{
	double sum = 0;

	for( UINT32 i=0; i<collectCounter_; i++ ) {
		sum += collection_[ i ];
	}
	TRACE( "avg=%g\n", sum / collectCounter_ );

	collection_.resize( 0 );
	collectCounter_ = 0;
	collectMax_     = 0;
}





#endif PROFILER_H