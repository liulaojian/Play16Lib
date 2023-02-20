#pragma once
#include "zbTypeDef.h"
#include "CommonMacro.h"

template<typename T>
struct FinderByNwkEp : public std::binary_function<T, T, bool>
{
	FinderByNwkEp(T p)
		: m_p(p)
		, mNwkAddr(0),uEndPoint(0)
	{
		CHECK(m_p);
	}
	FinderByNwkEp(uint16 NwkAddr,uint8 EndPoint)
		:  mNwkAddr(NwkAddr),uEndPoint(EndPoint)
		, m_p(NULL)
	{
	}
	bool operator()(T p)
	{
		DEBUG_TRY

		CHECKF(p);

		if (m_p)
		{
			return ( ( m_p->mNwkAddr == p->mNwkAddr) && ( m_p->uEndPoint == p->uEndPoint ) );
		}
		else
		{
			return (  ( p->mNwkAddr == mNwkAddr ) && ( p->uEndPoint == uEndPoint ) );
		}

		return true;
		DEBUG_CATCH_DEFUALT
		return false;
	}
	T m_p;
	uint16 mNwkAddr;
	uint8  uEndPoint;
};