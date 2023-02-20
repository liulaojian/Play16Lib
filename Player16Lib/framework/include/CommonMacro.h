#pragma  once

#include <assert.h>

#include <cstdio>
#include <ctime>
#include <stdarg.h>

#include "Log.h"

inline void log_assert(const char *type, const char *str)
{
	
}

#define ASSERT_LOG(x, y)	do { assert(x); (x) || (log_assert("ASSERT", #x), 0); } while (0)

#define MYASSERT(x)			ASSERT_LOG(x, "ASSERT")
#define CHECK(x)			do { if(!(x)) { assert(!"CHECK(" #x ")"), log_assert("CHECK", #x); return; } } while (0)
#define CHECKF(x)			do { if(!(x)) { assert(!"CHECKF(" #x ")"), log_assert("CHECKF", #x); return 0; } } while (0)
#define CHECKRET(x, y)		do { if(!(x)) { assert(!"CHECKF(" #x ")"), log_assert("CHECKF", #x); return y; } } while (0)
#define IF_NOT(x)			if ((!(x)) ? (assert(!"IF_NOT(" #x ")"), log_assert("IF_NOT", #x), 1) : 0)
#define IF_OK(x)			if ((x) ? 1 : (assert("!IF_OK(" #x ")"), log_assert("IF_OK", #x), 0))

#define PURE_VIRTUAL_FUNCTION		= 0;
#define DEAD_LOOP_BREAK(x, n)	{ if (++(x) > (n)) { ASSERT_LOG(x, !"DEAD_LOCK_BREAK"); break; } }




#define DEBUG_TRY	try {
#define DEBUG_CATCH(s)		} catch (...) { PrintfDebug("%s \n", s); }
#define DEBUG_CATCH2(s, x)	} catch (...) {   PrintfDebug("%s \n", x);}
#define	DEBUG_CATCH3(s, x, y)	} catch (...) {  PrintfDebug("%s %s \n", x,y); }
#define	DEBUG_CATCH4(s, x, y, z)	} catch (...) { PrintfDebug("%s %s %s\n", x,y,z); }
#define	DEBUG_CATCH5(s, x, y, z, h)	} catch (...) {  PrintfDebug("%s %s %s %s \n", x,y,z,h); }
#define DEBUG_CATCH_DEFUALT		DEBUG_CATCH("***CRASH***")



	

//----------------------------------------------------------
#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) \
	do \
{ \
	try \
{ \
	safe_delete(ptr); \
} \
	catch(...) { \
	; \
} \
} while (0)
#endif

//----------------------------------------------------------
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(ptr) \
	do \
{ \
	try \
{ \
	safe_release(ptr); \
} \
	catch(...) { \
	LOG_SYS_CATCH("*** SAFE_RELEASE("#ptr")"); \
} \
} while (0)
#endif

// ============================================================================
// ==============================================================================
template<typename T>
inline void safe_delete(T * &pT)
{
	if (NULL != pT) {
		delete pT;
		pT = NULL;
	}
}

// ============================================================================
// ==============================================================================
template<typename T>
inline void safe_release(T * &pT)
{
	if (NULL != pT) {
		pT->Release();
		pT = NULL;
	}
}

#ifndef MIN
#define MIN(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b)  (((a) > (b)) ? (a) : (b))
#endif

#ifndef INFINITE
#define INFINITE 0xFFFFFFFF
#endif

// Useful pixel color manipulation macros
#define GET_A(color)            ((color >> 24) & 0xFF)
#define GET_R(color)            ((color >> 16) & 0xFF)
#define GET_G(color)            ((color >>  8) & 0xFF)
#define GET_B(color)            ((color >>  0) & 0xFF)
