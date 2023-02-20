#pragma once

class CLock
{
public:
	CLock();
	virtual ~CLock();

	inline void TryLock()
	{
		::TryEnterCriticalSection(&lock);
	}
	inline void Lock()
	{
		::EnterCriticalSection(&lock);
	}
	inline void UnLock()
	{
		::LeaveCriticalSection(&lock);
	}

private:
	CRITICAL_SECTION lock;
}; 
