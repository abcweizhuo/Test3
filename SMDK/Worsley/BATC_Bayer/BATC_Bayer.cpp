//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================



#include "stdafx.h"
#define __BATC_BAYER_CPP
#include "BATC_Bayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#include "scdmacros.h" 
#include "md_headers.h" 
#ifdef GLADDY
#pragma LIBCOMMENT("..\\Bin\\", "\\DevLib" )
#pragma LIBCOMMENT("..\\Bin\\", "\\scdlib" )
#else
#pragma LIBCOMMENT("..\\..\\Bin\\", "\\DevLib" )
#pragma LIBCOMMENT("..\\..\\Bin\\", "\\scdlib" )
#endif
//===========================================================================

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:

   		/*if (!MakeVersionOK("BATC_Bayer.DLL", _MAKENAME, SCD_VERINFO_V0, SCD_VERINFO_V1, SCD_VERINFO_V2, SCD_VERINFO_V3))
        return FALSE;*/

		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

//===========================================================================

extern "C" __declspec(dllexport) BOOL IsSMDKLibDLL()
  {
  return TRUE;
  }


//===========================================================================

