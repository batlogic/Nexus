
#include <math.h>
#include "Definitions.h"
#include "TimeInfo.h"

#include "windows.h" // for debugging
#include "tchar.h"


TimeInfo::TimeInfo() : nextTimeCheck_(0), ppq_(0), bpm_(120)
{
}


void TimeInfo::getInfo( AudioEffectX* parent )
{
	nextTimeCheck_--;
		
	if( nextTimeCheck_ < 0 )
	{
		VstTimeInfo* info = parent->getTimeInfo( kVstPpqPosValid | kVstTempoValid );
			
		double ppq = 0.0;
		if( info != NULL ) {
			ppq  = info->ppqPos;
			bpm_ = info->tempo;
		}
			
		double test = fabs( ppq - ppq_ );
		if( fabs( ppq - ppq_ ) < 1 ) {
			nextTimeCheck_ = 20; // No significant tempo changes
			//OutputDebugString(_T("******\n"));
		}
		else {
			nextTimeCheck_ = 0; // tempo changes, check frequently
		}
		ppq_ = ppq;
	}
        
    // duration of this process call in ppq
    //double ppqOffset = ( (double)sampleFrames / sampleRate ) * ( bpm / 60.0 );
}