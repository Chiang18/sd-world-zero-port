//*************************************************************************************************
// ����:	���ú궨��
//---------------------------------------------------------
// ����:		
// ����:		
// ����޸�:
//*************************************************************************************************
#pragma once

namespace WorldEditorFramework 
{

// ת��һ��Managered String*����һ��const char*ָ��
#define MStringToCharPointer(pkObject)	\
	(const char*)System::Runtime::InteropServices::Marshal	\
	::StringToHGlobalAnsi(pkObject).ToPointer()

// �ͷ�ָ��
#define MFreeCharPointer(pkObject)	\
	System::Runtime::InteropServices::Marshal	\
	::FreeHGlobal(System::IntPtr((void*)pkObject))

// 
#define MAssert	System::Diagnostics::Debug::Assert




}