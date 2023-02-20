#pragma  once

class CMyTimerMS
{
public:
	CMyTimerMS(int nIntervalSecs = 0);
	virtual ~CMyTimerMS() {}

public:
	void	Update		();
	bool	IsTimeOut	();
	bool	ToNextTime	();
	void	WaitNextTime();
	void	SetInterval	(int nMilliSecs);

	void	Startup		(int nMilliSecs);
	bool	TimeOver	();
	void	Clear		();
	bool	IsActive	();
	void	IncInterval	(int nMilliSecs, int nLimit);
	void	DecInterval	(int nMilliSecs);

	bool	IsTimeOut	(int nMilliSecs);
	bool	ToNextTime	(int nMilliSecs);
	bool	TimeOver	(int nMilliSecs);
	bool	ToNextTick	(int nMilliSecs);

public: // get
	int		GetRemain();
	int		GetInterval();
	unsigned int	GetUpdateTime();

protected:
	int		m_nInterval;
	unsigned int	m_updateTime;
};
