#include "stdafx.h"
#include "CommonMacro.h"
#include "MyTimer.h"
#include <MMSystem.h>
CMyTimerMS::CMyTimerMS(int nIntervalSecs)
{
	m_nInterval = nIntervalSecs;
	m_updateTime = 0;
}

void CMyTimerMS::Update()
{
	m_updateTime = timeGetTime();
}

void CMyTimerMS::WaitNextTime()
{
	DWORD nowTime =timeGetTime();

	if (m_updateTime + m_nInterval > nowTime)
	{
		unsigned int waitTime = (unsigned int)(m_updateTime + m_nInterval - nowTime);
		::Sleep(waitTime);
	}

	Update();
}

void CMyTimerMS::SetInterval(int nMilliSecs)
{
	m_nInterval = nMilliSecs;
}

void CMyTimerMS::Startup(int nMilliSecs)
{
	m_nInterval = nMilliSecs;

	Update();
}

void CMyTimerMS::Clear()
{
	m_nInterval = 0;
	m_updateTime = 0;
}

void CMyTimerMS::IncInterval(int nMilliSecs, int nLimit)
{
	m_nInterval = MIN(m_nInterval + nMilliSecs, nLimit);
}

void CMyTimerMS::DecInterval(int nMilliSecs)
{
	m_nInterval = MAX(m_nInterval - nMilliSecs, 0);
}

int	CMyTimerMS::GetRemain()
{
	if (m_updateTime <= 0)
	{
		return 0;
	}

	int timeRemain = m_nInterval;

	if (timeGetTime() > m_updateTime)
	{
		timeRemain -= (int)(timeGetTime() - m_updateTime);
	}

	if (timeRemain > 0)
	{
		return timeRemain;
	}

	return 0;
}

int	CMyTimerMS::GetInterval()
{
	return m_nInterval;
}

unsigned int CMyTimerMS::GetUpdateTime()
{
	return m_updateTime;
}

bool CMyTimerMS::ToNextTick(int nMilliSecs)
{
	if (IsTimeOut(nMilliSecs))
	{
		if (timeGetTime() >= m_updateTime + nMilliSecs*2)
		{
			Update();
		}
		else
		{
			m_updateTime += nMilliSecs;
		}
		return true;
	}

	return false;
}

bool CMyTimerMS::IsTimeOut()
{
	return timeGetTime() >= m_updateTime + m_nInterval;
}

bool CMyTimerMS::ToNextTime()
{
	if (IsTimeOut())
	{
		Update();
		return true;
	}

	return false;
}

bool CMyTimerMS::TimeOver()
{
	if (IsActive() && IsTimeOut())
	{
		Clear();
		return true;
	}

	return false;
}

bool CMyTimerMS::IsActive()
{
	return m_updateTime != 0;
}

bool CMyTimerMS::IsTimeOut(int nMilliSecs)
{
	return timeGetTime() >= m_updateTime + nMilliSecs;
}

bool CMyTimerMS::ToNextTime(int nMilliSecs)
{
	if (IsTimeOut(nMilliSecs))
	{
		Update();
		return true;
	}

	return false;
}

bool CMyTimerMS::TimeOver(int nMilliSecs)
{
	if (IsActive() && IsTimeOut(nMilliSecs))
	{
		Clear();
		return true;
	}

	return false;
}

