#ifndef RMSDETECTOR_H
#define RMSDETECTOR_H


class RmsScanner
{
public:
	RmsScanner( UINT32 rate ) : 
		threshold_( 0.00001f ), 
		sum_( threshold_ ), 
		previous_( 1.0f ),
		counter_( 0 ) 
	{
		//windowSize_ = (UINT32)(rate * 0.3f);
		windowSize_ = (UINT32)(rate*100);
	}

	bool detectSilence( FLOAT value );

protected:
	void reset()	{ sum_ = 0.0f; counter_ = 0; }

	FLOAT threshold_;
	FLOAT sum_;
	FLOAT previous_;
	UINT32 counter_;
	UINT32 windowSize_;
};


inline bool RmsScanner::detectSilence( FLOAT value )
{	
	if( counter_ >= windowSize_ ) {  
		reset();
	} 
	++counter_ ;
	sum_ += value * value; 

	FLOAT current = abs( sqrt( sum_ / counter_ ));

	if( current < threshold_ && counter_ > 100 ) {
		//if( current < previous_ ) 	// test threshold
		//{
			//previous_ = current;	// does not work correctly
			//TRACE( "previous=%e current=%e threshold=%e\n", previous_, current, threshold_ );
			//TRACE(" RmsScanner current=%E\n", current );
			reset();
			return true;
			//return false;
		//}
	}
	return false;
}

#endif // RMSDETECTOR_H