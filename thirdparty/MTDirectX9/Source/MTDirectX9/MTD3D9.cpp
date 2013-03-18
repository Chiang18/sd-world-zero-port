#include "MTD3D9.h"
#include "MTDevice9.h"
#include "XWinVer.h"

//#pragma comment(lib,"detours")
#pragma comment(lib,"d3d9")

//-------------------------------------------------------------------------------------------------
// D3D原函数指针
//-------------------------------------------------------------------------------------------------
typedef	IDirect3D9* (STDMETHODCALLTYPE	*	T_Direct3DCreate9)(UINT);
typedef	HRESULT		(STDMETHODCALLTYPE	*	T_Direct3DCreate9Ex)(UINT, IDirect3D9Ex**);

T_Direct3DCreate9	ST_Direct3DCreate9		= NULL;		///< IDirect3DCreate9旧函数指针
T_Direct3DCreate9Ex	ST_Direct3DCreate9Ex	= NULL;		///< IDirect3DCreate9Ex旧函数指针

//extern T_D3DPERF_BeginEvent	ST_D3DPERF_BeginEvent;	///< D3DPERF_BeginEvent旧函数指针
//extern T_D3DPERF_EndEvent	ST_D3DPERF_EndEvent;		///< D3DPERF_EndEvent旧函数指针
//extern T_D3DPERF_SetMarker	ST_D3DPERF_SetMarker;	///< D3DPERF_SetMarker旧函数指针

CXWinVersion g_kVersion;	///< Windows版本信息采集	

//-------------------------------------------------------------------------------------------------
// 1.调用IDirect3D9::CreateDevice函数创建IDirect3DDevice9对象
// 2.Hook IDirect3DDevice9的	
//-------------------------------------------------------------------------------------------------
HRESULT	(STDMETHODCALLTYPE *ST_IDirect3D9_CreateDevice)(IDirect3D9 * This,UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface) = NULL;
HRESULT	STDMETHODCALLTYPE MT_IDirect3D9_CreateDevice(IDirect3D9 * This,UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface)
{
	// 检查设备类型,只有在硬件渲染时才进行Hook
	bool bHook = true;
	if (DeviceType == D3DDEVTYPE_HAL)
	{
		BehaviorFlags	|= D3DCREATE_MULTITHREADED;		///< 设置多线程标记
		//BehaviorFlags &= ~D3DCREATE_PUREDEVICE; 
		//pPresentationParameters->PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
		bHook			= true;
		//Adapter		= 1;
		//DeviceType	= D3DDEVTYPE_REF;
	}
	else
	{
		bHook = false;
	}

	HRESULT	hr = ST_IDirect3D9_CreateDevice(This, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
	if(SUCCEEDED(hr))
	{
		if(bHook)
		{
			Hook_IDirect3DDevice9(*ppReturnedDeviceInterface);
		}
		OutputDebugStringA("MTDiectX9.dll  CreateDevice!\n");
	}

	return	hr;
};
//-------------------------------------------------------------------------------------------------
HRESULT	(STDMETHODCALLTYPE *ST_IDirect3D9_CreateDeviceEx)(IDirect3D9Ex * This,UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,D3DDISPLAYMODEEX* pFullscreenDisplayMode,IDirect3DDevice9Ex** ppReturnedDeviceInterface) =	NULL;
HRESULT	STDMETHODCALLTYPE MT_IDirect3D9_CreateDeviceEx(IDirect3D9Ex * This,UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,D3DDISPLAYMODEEX* pFullscreenDisplayMode,IDirect3DDevice9Ex** ppReturnedDeviceInterface) \
{
	// 检查设备类型,只有在硬件渲染时才进行Hook
	bool bHook = true;
	if (DeviceType == D3DDEVTYPE_HAL)
	{
		BehaviorFlags	|= D3DCREATE_MULTITHREADED;		///< 设置多线程标记
		//BehaviorFlags &= ~D3DCREATE_PUREDEVICE; 
		//pPresentationParameters->PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
		bHook			= true;
		//Adapter		= 1;
		//DeviceType	= D3DDEVTYPE_REF;
	}
	else
	{
		bHook = false;
	}

	HRESULT	hr = ST_IDirect3D9_CreateDeviceEx(This, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface);
	if(SUCCEEDED(hr))
	{
		if(bHook)
		{
			IDirect3DDevice9* pDevice = (IDirect3DDevice9*)(*ppReturnedDeviceInterface);
			Hook_IDirect3DDevice9(pDevice);
		}
		OutputDebugStringA("MTDiectX9.dll  CreateDeviceEx!\n");
	}

	return	hr;
};

//-------------------------------------------------------------------------------------------------
// 1.调用Direct3DCreate9创建IDirect3D9对象
// 2.Hook IDirect3D9的CreateDevice/CreateDeviceEx函数
//-------------------------------------------------------------------------------------------------
bool bHasHookDevice	= false;
//-------------------------------------------------------------------------------------------------
IDirect3D9* WINAPI MT_Direct3DCreate9(UINT SDKVersion)
{
	OutputDebugStringA("MTDiectX9.dll Direct3DCreate9!\n");

	IDirect3D9*	pD3D9 =	ST_Direct3DCreate9(SDKVersion);
	if (pD3D9 != NULL)
	{
		if (!bHasHookDevice)
		{
			ST_IDirect3D9_CreateDevice = pD3D9->lpVtbl->CreateDevice;

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());
			DetourAttach(&(PVOID&)ST_IDirect3D9_CreateDevice, MT_IDirect3D9_CreateDevice);
			DetourTransactionCommit();

			bHasHookDevice = true;
		}
	}

	return	pD3D9;
}
//-------------------------------------------------------------------------------------------------
HRESULT	WINAPI MT_Direct3DCreate9Ex(UINT SDKVersion ,IDirect3D9Ex**	ppD3D9)
{
	HRESULT	hr = ST_Direct3DCreate9Ex(D3D_SDK_VERSION, ppD3D9);
	if ((*ppD3D9)!=NULL)
	{
		if(!bHasHookDevice)
		{
			ST_IDirect3D9_CreateDeviceEx = (*ppD3D9)->lpVtbl->CreateDeviceEx;

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());
			DetourAttach(&(PVOID&)ST_IDirect3D9_CreateDeviceEx, MT_IDirect3D9_CreateDeviceEx);
			DetourTransactionCommit();

			bHasHookDevice = true;
		}
	}

	return	hr;
}
//-------------------------------------------------------------------------------------------------
// Hook kernel32.dll的LoadLibraryW函数
//-------------------------------------------------------------------------------------------------
// LoadLibraryW库函数原地址
typedef	HMODULE	(WINAPI	*T_LoadLibraryW)(LPCWSTR lpLibFileName);
T_LoadLibraryW	ST_LoadLibraryW;	

// LoadLibraryW新函数
HMODULE	WINAPI	MT_LoadLibraryW( LPCWSTR lpLibFileName);

// 标记LoadLibraryW是否被Hook
bool bHasHookKernel	= false;
//-------------------------------------------------------------------------------------------------
void Hook_LoadLibrary()
{
	if(bHasHookKernel)
		return;

	HMODULE	hLibrary = ::GetModuleHandle(L"kernel32.dll");
	ST_LoadLibraryW	= (T_LoadLibraryW)::GetProcAddress(hLibrary, "LoadLibraryW");
	DetourTransactionBegin();
	DetourUpdateThread(::GetCurrentThread());
	DetourAttach(&(PVOID&)ST_LoadLibraryW,  MT_LoadLibraryW);
	DetourTransactionCommit();

	bHasHookKernel = true;
}
//-------------------------------------------------------------------------------------------------
void UnHook_LoadLibrary()
{
	if(!bHasHookKernel)
		return;

	DetourTransactionBegin();
	DetourUpdateThread(::GetCurrentThread());
	DetourDetach(&(PVOID&)ST_LoadLibraryW, MT_LoadLibraryW);
	DetourTransactionCommit();

	bHasHookKernel = false;
}
//-------------------------------------------------------------------------------------------------
//T_Direct3DCreate9	FindDirect3DCreate9(){
//	T_Direct3DCreate9 pIAT	=	Direct3DCreate9;
//	U8*	p	=	(U8*)pIAT;
//
//	if(p[0]==0xE9){
//		DWORD	offset	=	*(DWORD*)(p+1);
//
//		DWORD*	pJumpAddr	=	(DWORD*)( p+5+offset);
//		U8*		pJumpAddrCode	=	(U8*)pJumpAddr;
//		if(	pJumpAddrCode[0]	==	0xFF && 
//			pJumpAddrCode[1]	==	0x25)
//		{
//			DWORD offset1		=	*(DWORD*)(pJumpAddrCode+2);
//			DWORD*	pJumpAddr1	=	(DWORD*)(*(DWORD*)( offset1));
//			U8*	pJumpAddrCode1	=	(U8*)pJumpAddr1;
//			if(	pJumpAddrCode1[0]	==	0x8B	&&
//				pJumpAddrCode1[1]	==	0xFF	&&
//				pJumpAddrCode1[2]	==	0x55	&&
//				pJumpAddrCode1[3]	==	0x8B	&&
//				pJumpAddrCode1[4]	==	0xEC)
//			{
//				return	(T_Direct3DCreate9)pJumpAddr1;
//			}
//		}
//	}
//
//	return NULL;
//};
//-------------------------------------------------------------------------------------------------
// Hook d3d9.dll的Direct3DCreate9/Direct3DCreate9Ex/D3DPERF_BeginEvent/D3DPERF_EndEvent/
// D3DPERF_SetMarker函数
//-------------------------------------------------------------------------------------------------
bool bHasHookD3D9 =	false;
//-------------------------------------------------------------------------------------------------
void Hook_D3D9(HMODULE hLibrary)
{
	if(hLibrary==NULL)
		return;

	if(bHasHookD3D9)
		return;

	//T_Direct3DCreate9 tTest =	FindDirect3DCreate9();
	//void* p =	Direct3DCreate9(D3D_SDK_VERSION);

	// 获取函数地址
	ST_Direct3DCreate9		= (T_Direct3DCreate9)::GetProcAddress(hLibrary,"Direct3DCreate9");
	ST_Direct3DCreate9Ex	= (T_Direct3DCreate9Ex)::GetProcAddress(hLibrary,"Direct3DCreate9Ex");
	//ST_D3DPERF_BeginEvent	= (T_D3DPERF_BeginEvent)::GetProcAddress(hLibrary,"D3DPERF_BeginEvent");
	//ST_D3DPERF_EndEvent		= (T_D3DPERF_EndEvent)::GetProcAddress(hLibrary,"D3DPERF_EndEvent");
	//ST_D3DPERF_SetMarker	= (T_D3DPERF_SetMarker)::GetProcAddress(hLibrary,"D3DPERF_SetMarker");

	//void* p =	tTest(D3D_SDK_VERSION);

	// Hook函数
    DetourTransactionBegin();
    DetourUpdateThread(::GetCurrentThread());
	DetourAttach(&(PVOID&)ST_Direct3DCreate9,		MT_Direct3DCreate9);
	if (ST_Direct3DCreate9Ex != NULL)
		DetourAttach(&(PVOID&)ST_Direct3DCreate9Ex,	MT_Direct3DCreate9Ex);
	//DetourAttach(&(PVOID&)ST_D3DPERF_BeginEvent,	MT_PIX_D3DPERF_BeginEvent);
	//DetourAttach(&(PVOID&)ST_D3DPERF_EndEvent,	MT_PIX_D3DPERF_EndEvent);
	//DetourAttach(&(PVOID&)ST_D3DPERF_SetMarker,	MT_PIX_D3DPERF_SetMarker);
    DetourTransactionCommit();

	bHasHookD3D9 = true;

	UnHook_LoadLibrary();
};
//-------------------------------------------------------------------------------------------------
void UnHook_D3D9()
{
	if (!bHasHookD3D9)
		return;

	// UnHook函数
    DetourTransactionBegin();
    DetourUpdateThread(::GetCurrentThread());
	DetourDetach(&(PVOID&)ST_Direct3DCreate9,		MT_Direct3DCreate9);
	if (ST_Direct3DCreate9Ex != NULL)
		DetourDetach(&(PVOID&)ST_Direct3DCreate9Ex,	MT_Direct3DCreate9Ex);
	//DetourDetach(&(PVOID&)ST_D3DPERF_BeginEvent,	MT_PIX_D3DPERF_BeginEvent);
	//DetourDetach(&(PVOID&)ST_D3DPERF_EndEvent,	MT_PIX_D3DPERF_EndEvent);
	//DetourDetach(&(PVOID&)ST_D3DPERF_SetMarker,	MT_PIX_D3DPERF_SetMarker);
    DetourTransactionCommit();

	bHasHookD3D9	=	false;
};
//-------------------------------------------------------------------------------------------------
// LoadLibrary的拦截函数,对D3D9.dll进行Detour Hook
//-------------------------------------------------------------------------------------------------
HMODULE	WINAPI MT_LoadLibraryW( LPCWSTR lpLibFileName)
{
	HMODULE	hLibrary = ST_LoadLibraryW(lpLibFileName);
	if (hLibrary!=NULL)
	{
		if (wcscmp(lpLibFileName, L"d3d9.dll") == 0)
		{
			Hook_D3D9(hLibrary);
		}
	}
	return	hLibrary;	
};
//-------------------------------------------------------------------------------------------------
// 动态链接库入口, Hook IDirect3DCreate9/IDirect3DCreate9Ex函数
//-------------------------------------------------------------------------------------------------
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
#if DETOURS_VERSION	== 30000
     if (DetourIsHelperProcess()) 
	 {
		 ::OutputDebugStringA("MTDiectX9.dll DetourIsHelperProcess!\n");
         return TRUE;
     }
#endif

    if (dwReason == DLL_PROCESS_ATTACH) 
	{
		// Win8暂时不支持
		if (g_kVersion.GetWinVersion() == WIN8)
		{
			::OutputDebugStringA("MTDiectX9.dll  is not support Windows8!\n");
			return TRUE;
		}

		// 日志
		::OutputDebugStringA("MTDiectX9.dll  DLL_PROCESS_ATTACH!\n");

        DetourRestoreAfterWith();
		
		// Hook d3d9.dll入口函数
		HMODULE	hD3D9 =	::GetModuleHandle(L"d3d9.dll");
		if (hD3D9 == NULL)
		{
			Hook_LoadLibrary();	///< d3d9.dll还没有加载,Hook LoadLibrary()函数
		}
		else
		{
			Hook_D3D9(hD3D9);	///< d3d9.dll已经加载,Hook Direct3DCreate9函数
		}

		// 检查D3D版本与D3DX版本是否匹配
 		//if (FALSE == D3DXCheckVersion(D3D_SDK_VERSION, D3DX_SDK_VERSION))
		//{
 		//	OutputDebugStringA("D3DX_SDK_VERSION ERROR!\n");
 		//}
    }
    else if (dwReason == DLL_PROCESS_DETACH) 
	{
		// Win8暂时不支持
		if (g_kVersion.GetWinVersion() == WIN8)
		{
			::OutputDebugStringA("MTDiectX9.dll  is not support Windows8!\n");
			return TRUE;
		}

		ReleaseAllResource();
		UnHook_LoadLibrary();
		UnHook_D3D9();
		UnHook_IDirect3DDevice9();
    }
    return TRUE;
}
//-------------------------------------------------------------------------------------------------