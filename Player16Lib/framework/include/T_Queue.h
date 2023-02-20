#pragma once

#include <deque>

// pop不成功时返回0。

template<typename T>
class CQueue
{
public:
	void			push(T obj)		{ m_set.push_back(obj); }
	T				front()			{ if (size()) { T obj = *m_set.begin(); return obj; } return 0; }
	T				pop()			{ if (size()) { T obj = *m_set.begin(); m_set.pop_front(); return obj; } return 0; }
	unsigned int	size()			{ return static_cast<unsigned int>(m_set.size()); }
	void			clear()			{ m_set.clear(); }

protected:
	typedef	std::deque<T> QUEUE_SET;
	QUEUE_SET m_set;
};
