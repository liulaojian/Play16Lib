#include "stdafx.h"
#include "PlayerMgr.h"
#include "CommonMacro.h"

CPlayerMgr* CPlayerMgr::s_pIntance = NULL;

CPlayerMgr::CPlayerMgr()
{


}

CPlayerMgr::~CPlayerMgr()
{
	
}



CPlayerMgr* CPlayerMgr::GetInstance()
{
	if (s_pIntance == NULL)
	{
		s_pIntance = new CPlayerMgr();
		CHECKF(s_pIntance);
	}

	return s_pIntance;
}


CPlayerMain *  CPlayerMgr::GetPlayerMainById(int id)
{
	CPlayerMain* pPlayerMain=NULL;
	mLock.Lock();
	for(int i=0;i<vecPlayerMain.size();i++)
	{
		pPlayerMain = vecPlayerMain[i];
		if(pPlayerMain)
		{
			if(pPlayerMain->GetId()==id)
			{
				mLock.UnLock();
				return pPlayerMain;
			}
		}

	}
	mLock.UnLock();
	return NULL;

}

CPlayerMain *  CPlayerMgr:: NewPlayerMain(int newid)
{
	CPlayerMain* pPlayerMain=NULL;
	BOOL bFound=FALSE;
	mLock.Lock();
	for(int i=0;i<vecPlayerMain.size();i++)
	{
		pPlayerMain = vecPlayerMain[i];
		if(pPlayerMain)
		{
			if(pPlayerMain->GetId()==newid)
			{
				bFound=TRUE;
				break;
			}
		}
	}

	if(!bFound)
	{

		pPlayerMain = new CPlayerMain(newid);
		vecPlayerMain.push_back(pPlayerMain);
		mLock.UnLock();
		return pPlayerMain;

	}
	else
	{
		mLock.UnLock();
		return NULL;
	}
}


int  CPlayerMgr::GetRuningNums(void)
{
	int mNums = 0;
	CPlayerMain* pPlayerMain = NULL;
	mLock.Lock();
	for (int i = 0; i < vecPlayerMain.size(); i++)
	{
		pPlayerMain = vecPlayerMain[i];
		if (pPlayerMain)
		{
			if (pPlayerMain->GetPlayerState())
			{
				if (pPlayerMain->GetPlayerState()->bRun)
					mNums++;
			}
		}
	}
	mLock.UnLock();

	return mNums;

}



bool CPlayerMgr::IsMainAudioOpened(void)
{
	CPlayerMain* pPlayerMain = NULL;
	mLock.Lock();
	for (int i = 0; i < vecPlayerMain.size(); i++)
	{
		pPlayerMain = vecPlayerMain[i];
		if (pPlayerMain)
		{
			if (pPlayerMain->GetPlayerState())
			{
				if (pPlayerMain->GetPlayerState()->bMainAudio)
				{
					mLock.UnLock();
					return true;
				}
					
			}
		}
	}
	mLock.UnLock();
	return false;
}



int  CPlayerMgr::SDL_Quit()
{
	if (GetRuningNums() == 0)
	{
		SDL_Quit();
		return 0;

	}

	return -1;
		
}
