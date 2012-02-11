#ifndef TIMEINFO_H
#define TIMEINFO_H

#include "VST/audioeffectx.h"


class TimeInfo
{
public:
	TimeInfo();

	void getInfo( AudioEffectX* parent );

protected:
	int nextTimeCheck_;
	double ppq_;	// pulses per quarter
	double bpm_;
};




#endif // TIMEINFO_H