#include "HookAlloc.h"

struct HOOKAPI
{
	void* pfOldFunc;
	void* pfNewFunc;
};

//-------------------------------------------------------------------------------------------------
// Dll���
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
// ���ڴ��PE�ļ��������滻dll������ָ������
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
				// ɨ�浼�뺯����,�޸���Ҫ�޸ĵĵ�ַ
				//	::VirutalQuery			��ѯָ�������ڴ��ҳ������
				//	::VirtualProtect		����ָ���ڴ淶Χ��ҳ������
				//	::WriteProcessMemory	д������ڴ�����
				if ((*pAddress) == (int)pkFuncArray[i].pfOldFunc)
				{
					DWORD dwOldPageProp;	///< �ɵ��ڴ�ҳ����
					DWORD dwNewAddress = 0;
					//MEMORY_BASIC_INFORMATION kMBI;
					//::VirtualQuery(pAddress, &kMBI, sizeof(kMBI));
					::VirtualProtect(pAddress, sizeof(DWORD), PAGE_READWRITE, &dwOldPageProp);	///< �޸��ڴ�ҳ����Ϊ��д��
					::WriteProcessMemory(::GetCurrentProcess(), &pAddress, pkFuncArray[i].pfNewFunc, sizeof(DWORD), &dwNewAddress);
					::VirtualProtect(pAddress, sizeof(DWORD), dwOldPageProp, &dwNewAddress);	///< �ָ��ڴ�ҳ����
			
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
// Hookָ��dll��ָ������Ϊָ������
//	@param szHookDllName	��Hook��dll������
//	@param szFuncDllName	��Hook�ĺ������ڵ�dll������
//	@param szFuncName		��Hook�ĺ���������
//	@param pfFunc			Ҫ�滻���º���ָ��
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
// Hookָ��dll���ض�����Ϊ�ض�����
//	@param szHookDllName	��Hook��dll������
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