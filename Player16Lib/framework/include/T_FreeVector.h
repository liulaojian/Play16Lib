#pragma once

#include <vector>
#include "CommonMacro.h"


template<typename T>
class CFreeVector
{
public:
	static bool freeVector(T &vecFree)  
	{ 
		for(unsigned int i=0;i<vecFree.size();i++)
		{
			SAFE_DELETE(vecFree[i]);
		}
		vecFree.clear();
		return true;
	
	}  
};
