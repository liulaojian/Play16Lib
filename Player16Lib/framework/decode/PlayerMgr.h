#pragma  once
#include <vector>
#include "PlayerMain.h"
#include "Lock.h"

typedef std::vector<CPlayerMain *> Vec_PlayerMain;


class CPlayerMgr
{

public:
	CPlayerMgr();
	virtual ~CPlayerMgr();

public:
	static CPlayerMgr* GetInstance();

	CPlayerMain * GetPlayerMainById(int id);

	CPlayerMain * NewPlayerMain(int newid);

	int   GetRuningNums(void);


	bool IsMainAudioOpened(void);

	
	int  SDL_Quit();

private:
	static CPlayerMgr* s_pIntance;
	Vec_PlayerMain vecPlayerMain;
	CLock mLock;

	
	
};

#define PlayerMgr()	CPlayerMgr::GetInstance()