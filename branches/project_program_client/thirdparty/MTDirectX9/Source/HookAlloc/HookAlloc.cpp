#include "HookAlloc.h"

struct HOOKAPI
{
	void* pfOldFunc;
	void* pfNewFunc;
};

//-------------------------------------------------------------------------------------------------
// Dll入口
//-------------------------------------------------------------------------------------------------
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{

	}

	return TRUE;
}
//-------------------------------------------------------------------------------------------------
// 在内存的PE文件镜像中替换dll导入表的指定函数
//-------------------------------------------------------------------------------------------------
int HookProc(const char* szDllName, HOOKAPI* pkFuncArray, int iCount)
{
	int iHookCount = 0;
	HMODULE hModule = ::GetModuleHandleA(szDllName);
	if (!hModule)
		return iHookCount;

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((BYTE*)hModule + pDosHeader->e_lfanew);
	PIMAGE_OPTIONAL_HEADER pOptHeader = (PIMAGE_OPTIONAL_HEADER)&(pNTHeader->OptionalHeader);
	PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)hModule + pOptHeader->DataDirectory[1].VirtualAddress);
	while (pImportDescriptor->FirstThunk)
	{
		int iNum = 1;
		//char* szImportDllName = (char*)((BYTE*)hModule + pImportDescriptor->Name);
		PIMAGE_THUNK_DATA pThunkData = (PIMAGE_THUNK_DATA)((BYTE*)hModule + pImportDescriptor->OriginalFirstThunk);
		while (pThunkData->u1.Function)
		{
			char* szFuncName = (char*)((BYTE*)hModule + (DWORD)pThunkData->u1.AddressOfData + 2);
			PDWORD pAddress = (DWORD*)((BYTE*)hModule + (DWORD)pImportDescriptor->FirstThunk) + (iNum - 1);
			for (int i = 0; i < iCount; ++i)
			{
				// 扫面导入函数表,修改需要修改的地址
				//	::VirutalQuery			查询指定虚拟内存的页面属性
				//	::VirtualProtect		更改指定内存范围的页面属性
				//	::WriteProcessMemory	写入进程内存区域
				if ((*pAddress) == (int)pkFuncArray[i].pfOldFunc)
				{
					DWORD dwOldPageProp;	///< 旧的内存页属性
					DWORD dwNewAddress = 0;
					//MEMORY_BASIC_INFORMATION kMBI;
					//::VirtualQuery(pAddress, &kMBI, sizeof(kMBI));
					::VirtualProtect(pAddress, sizeof(DWORD), PAGE_READWRITE, &dwOldPageProp);	///< 修改内存页属性为可写入
					::WriteProcessMemory(::GetCurrentProcess(), &pAddress, pkFuncArray[i].pfNewFunc, sizeof(DWORD), &dwNewAddress);
					::VirtualProtect(pAddress, sizeof(DWORD), dwOldPageProp, &dwNewAddress);	///< 恢复内存页属性
			
					++iHookCount;
				}
			}

			++iNum;
			++pThunkData;
		}

		++pImportDescriptor;
	}

	return iHookCount;
}
//-------------------------------------------------------------------------------------------------
// Hook指定dll的指定函数为指定函数
//	@param szHookDllName	被Hook的dll的名称
//	@param szFuncDllName	被Hook的函数所在的dll的名称
//	@param szFuncName		被Hook的函数的名称
//	@param pfFunc			要替换的新函数指针
//-------------------------------------------------------------------------------------------------
bool Hook(const char* szHookDllName, const char* szFuncDllName, const char* szFuncName, void* pfFunc)
{
	if (!szHookDllName || !szFuncDllName || !szFuncName || !pfFunc)
		return false;

	HMODULE hFuncDll = ::GetModuleHandleA(szFuncDllName);
	if (!hFuncDll)
		return false;

	FARPROC pfOldFunc = ::GetProcAddress(hFuncDll, szFuncName);
	if (!pfOldFunc)
		return false;

	HOOKAPI kFuncArray = {pfOldFunc, pfFunc};
	if (!HookProc(szHookDllName, &kFuncArray, 1))
		return false;

	return true;
}
//-------------------------------------------------------------------------------------------------
// Hook指定dll的特定函数为特定函数
//	@param szHookDllName	被Hook的dll的名称
//-------------------------------------------------------------------------------------------------
void HookAll(const char* szHookDllName)
{
	HMODULE hKernelDll = ::GetModuleHandleA("kernel32.dll");
	HMODULE hmsvcrtDll = ::GetModuleHandleA("msvcrt.dll");
	HMODULE hmsvcrt80Dll = ::GetModuleHandleA("msvcrt80.dll");

	//HOOKAPI kFuncArray = 
	//{
	//	::GetProcAddress(hKernelDll, "VirtualAlloc"), My_VirtualAlloc,
	//	::GetProcAddress(hKernelDll, "VirtualFree"), My_VirtualFree,
	//	::GetProcAddress(hKernelDll, "HeapAlloc"), My_HeapAlloc,
	//	::GetProcAddress(hKernelDll, "HeaplFree"), My_HeapFree,
	//	::GetProcAddress(hKernelDll, "LocalAlloc"), My_LocalAlloc,
	//	::GetProcAddress(hKernelDll, "LocalFree"), My_LocalFree,

	//	::GetProcAddress(hmsvcrtDll, "malloc"), My_malloc,
	//	::GetProcAddress(hmsvcrtDll, "free"), My_free,
	//	::GetProcAddress(hmsvcrtDll, "_aligned_malloc"), My_aligned_alloc,
	//	::GetProcAddress(hmsvcrtDll, "_aligned_free"), My_aligned_free,

	//	::GetProcAddress(hmsvcrt80Dll, "malloc"), My_malloc,
	//	::GetProcAddress(hmsvcrt80Dll, "free"), My_free,
	//	::GetProcAddress(hmsvcrt80Dll, "_aligned_malloc"), My_aligned_alloc,
	//	::GetProcAddress(hmsvcrt80Dll, "_aligned_free"), My_aligned_free,
	//};


	//HookProc(szHookDllName, &kFuncArray, 14);
}
//-------------------------------------------------------------------------------------------------