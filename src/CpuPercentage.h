#pragma once
#include <windows.h>

class CpuPercentage
{
public:
	CpuPercentage(void);
	
	short getPercentage();
private:
	ULONGLONG subtractTimes(const FILETIME& timeA, const FILETIME& timeB);
	bool enoughTimePassed();
	inline bool isFirstRun() const { return (lastRun_ == 0); }
	
	//system total times
	FILETIME prevSysKernelTime_;
	FILETIME prevSysUserTime_;

	//process times
	FILETIME prevProcKernelTime_;
	FILETIME prevProcUserTime_;

	short cpuPercentage_;
	ULONGLONG lastRun_;
	
	volatile LONG runCount_;
};
