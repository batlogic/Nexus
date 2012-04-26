
#ifndef PROFILER_H
#define PROFILER_H

#include "Definitions.h"
#include <intrin.h>


//-------------------------------------------------------
// class PerformanceCounter
//-------------------------------------------------------
//
class PerformanceCounter
{
public:
	PerformanceCounter() :
		frequency_( 0 ),
		first_( 0 ),
		last_( 0 ),
		diff_( 0 )
	{
		if( !QueryPerformanceFrequency( (LARGE_INTEGER*)&frequency_ )) {
			frequency_ = (LONGLONG)0.0;
		}
		begin();
	}

	void begin() {
		QueryPerformanceCounter( (LARGE_INTEGER*)&first_ );
	}
	
	double end()
	{
		QueryPerformanceCounter( (LARGE_INTEGER*)&last_ );
		diff_    = (((double)( last_ - first_ )) / ((double) frequency_ )); 
		return diff_;
	}

	void print() {
		TRACE( "%f sec\n", diff_ );
	}


	LONGLONG frequency_, first_, last_;
	double diff_;
};


//-------------------------------------------------------
// class TimeStampCounter
//-------------------------------------------------------
//
class TimeStampCounter
{
public:
	TimeStampCounter() :
		first_( 0 ),
		last_( 0 ),
		diff_( 0 )
	{
		begin();
	}

	void begin() {
		first_ = readTSC();
	}
	
	double end()
	{
		last_ = readTSC();
		diff_ = (double)( last_ - first_ );
		return diff_;
	}

	void print() {
		TRACE( "%f sec\n", diff_ );
	}

protected:
	LONGLONG readTSC()            // Returns time stamp counter 
	{
		int dummy[4];             // For unused returns 
		volatile int dontSkip;    // Volatile to prevent optimizing 
		__int64 clock;            // Time 
		__cpuid(dummy, 0);        // Serialize 
		dontSkip = dummy[0];      // Prevent optimizing away cpuid 
		//clock = ReadTimeStampCounter();  // Read clock cycles since last reset
		clock = __rdtsc();  // Read clock cycles since last reset
		__cpuid(dummy, 0);        // Serialize again 
		dontSkip = dummy[0];      // Prevent optimizing away cpuid 
		return clock; 
	} 

	LONGLONG first_, last_;
	double diff_;
};

//-------------------------------------------------------
// class PerformanceAverage
//-------------------------------------------------------
class PerformanceAverage
{
public:
	PerformanceAverage() :
		counter_(0),
		max_(0)
	{}

	void start( UINT32 max )
	{ 
		if( max_ == 0 ) {
			collection_.resize( max ); 
			max_ = max;
			counter_ = 0;
		}
	}

	void add( double value )
	{
		if( counter_ < max_ )	{
			collection_[ counter_++ ] = value;
		}
	}

	void finish()
	{
		double sum = 0;

		for( UINT32 i=0; i<counter_; i++ ) {
			sum += collection_[ i ];
		}
		TRACE( "avg=%g\n", sum / counter_ );
		reset();
	}

	void reset()
	{
		collection_.resize( 0 );
		counter_ = 0;
		max_     = 0;
	}

	vector<double> collection_;
	UINT32 counter_;
	UINT32 max_;
};


#endif PROFILER_H