
#include "CpuPercentage.h"


CpuPercentage::CpuPercentage(void) :
	cpuPercentage_(-1),
	lastRun_(0),
	runCount_(0)
{
	ZeroMemory(&prevSysKernelTime_, sizeof(FILETIME));
	ZeroMemory(&prevSysUserTime_, sizeof(FILETIME));

	ZeroMemory(&prevProcKernelTime_, sizeof(FILETIME));
	ZeroMemory(&prevProcUserTime_, sizeof(FILETIME));
}


/**********************************************
* CpuPercentage::getPercentage
* returns the percent of the CPU that this process
* has used since the last time the method was called.
* If there is not enough information, -1 is returned.
* If the method is recalled to quickly, the previous value
* is returned.
***********************************************/
short CpuPercentage::getPercentage()
{
	//create a local copy to protect against race conditions
	short percent = cpuPercentage_;
	if(::InterlockedIncrement(&runCount_) == 1)
	{
		// If this is called too often, the measurement itself will greatly affect the results.
		if(!enoughTimePassed())
		{
			::InterlockedDecrement(&runCount_);
			return percent;
		}

		FILETIME ftSysIdle, ftSysKernel, ftSysUser;
		FILETIME ftProcCreation, ftProcExit, ftProcKernel, ftProcUser;

		if(!GetSystemTimes(&ftSysIdle, &ftSysKernel, &ftSysUser) ||
			!GetProcessTimes(GetCurrentProcess(), &ftProcCreation, &ftProcExit, &ftProcKernel, &ftProcUser))
		{
			::InterlockedDecrement(&runCount_);
			return percent;
		}

		if(!isFirstRun())
		{
			/*
			CPU usage is calculated by getting the total amount of time the system has operated
			since the last measurement (made up of kernel + user) and the total
			amount of time the process has run (kernel + user).
			*/
			ULONGLONG ftSysKernelDiff = subtractTimes(ftSysKernel, prevSysKernelTime_);
			ULONGLONG ftSysUserDiff   = subtractTimes(ftSysUser, prevSysUserTime_);

			ULONGLONG ftProcKernelDiff = subtractTimes(ftProcKernel, prevProcKernelTime_);
			ULONGLONG ftProcUserDiff = subtractTimes(ftProcUser, prevProcUserTime_);

			ULONGLONG totalSys  =  ftSysKernelDiff + ftSysUserDiff;
			ULONGLONG totalProc = ftProcKernelDiff + ftProcUserDiff;

			if (totalSys > 0) {
				cpuPercentage_ = (short)((100.0 * totalProc) / totalSys);
			}
		}
		
		prevSysKernelTime_  = ftSysKernel;
		prevSysUserTime_    = ftSysUser;
		prevProcKernelTime_ = ftProcKernel;
		prevProcUserTime_   = ftProcUser;
		
		lastRun_ = ::GetTickCount64();
		percent  = cpuPercentage_;
	}
	
	::InterlockedDecrement(&runCount_);

	return percent;
}

ULONGLONG CpuPercentage::subtractTimes(const FILETIME& timeA, const FILETIME& timeB)
{
	LARGE_INTEGER a, b;
	a.LowPart = timeA.dwLowDateTime;
	a.HighPart = timeA.dwHighDateTime;

	b.LowPart = timeB.dwLowDateTime;
	b.HighPart = timeB.dwHighDateTime;

	return a.QuadPart - b.QuadPart;
}

bool CpuPercentage::enoughTimePassed()
{
	const int minElapsedMS = 250;//milliseconds

	ULONGLONG dwCurrentTickCount = GetTickCount64();
	return (dwCurrentTickCount - lastRun_) > minElapsedMS; 
}