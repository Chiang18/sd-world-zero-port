//*************************************************************************************************
// 内容:	
//
//	1.VirtualAlloc/VirtualFree
//   其操作的基本单位是内存页面
//
//	2.HeapAlloc/HeapFree
//	 从堆上分配一块内存，且分配的内存是不可移动的（即如果没有连续的空间能满足分配的大小，
//	程序不能将其他零散的空间利用起来，从而导致分配失败）
//
//	3.LocalAlloc/LocalFree
//	 该函数用于从局部堆中分配内存供程序使用
//
//  4.GlobalAlloc
//	 该函数用于从全局堆中分配出内存供程序使用
//
//	5.malloc/free
//	 C++/C语言的标准库函数，可用于申请动态内存和释放内存
//
//	6._alighed_malloc/aligned_free
//
// 
//	7.new/delete
//	 C++的运算符
//
//	参考链接:
//		http://msdn.microsoft.com/en-us/library/windows/desktop/aa366781(v=vs.85).aspx
//---------------------------------------------------------
// 作者:		
// 创建:		
// 最后修改:	
//*************************************************************************************************
#pragma once
#include <windows.h>

//-----------------------------------------------------------------------------
// DLL导出标记
//-----------------------------------------------------------------------------
#ifndef HOOKALLOC_EXPORT
  #ifdef HOOKALLOC_IMPORT
	#define HOOKALLOC_EXPORT dllexport
  #else
	#define HOOKALLOC_EXPORT __declspec(dllimport)
  #endif
#endif

// 参考自Gamebryo
//#ifdef HOOKALLOC_EXPORT
//	#define HOOKALLOC_ENTRY __declspec(dllexport)
//#else
//  #ifdef HOOKALLOC_IMPORT
//	#define HOOKALLOC_ENTRY __declspec(dllimport)
//  #else
//	#define HOOKALLOC_ENTRY
//  #endif
//#endif
//-----------------------------------------------------------------------------