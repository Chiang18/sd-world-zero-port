//*************************************************************************************************
// 内容:	常用宏定义
//---------------------------------------------------------
// 作者:		
// 创建:		
// 最后修改:
//*************************************************************************************************
#pragma once

namespace WorldEditorFramework 
{

// 转换一个Managered String*对象到一个const char*指针
#define MStringToCharPointer(pkObject)	\
	(const char*)System::Runtime::InteropServices::Marshal	\
	::StringToHGlobalAnsi(pkObject).ToPointer()

// 释放指针
#define MFreeCharPointer(pkObject)	\
	System::Runtime::InteropServices::Marshal	\
	::FreeHGlobal(System::IntPtr((void*)pkObject))

// 
#define MAssert	System::Diagnostics::Debug::Assert




}