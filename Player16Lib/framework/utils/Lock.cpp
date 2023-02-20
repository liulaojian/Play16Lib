#include "stdafx.h"
#include "Lock.h"


CLock::CLock()
{
	::InitializeCriticalSection(&lock);

}

CLock::~CLock()
{
	::DeleteCriticalSection(&lock);
}