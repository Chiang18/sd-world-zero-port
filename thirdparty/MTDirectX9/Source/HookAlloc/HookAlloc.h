//*************************************************************************************************
// ����:	
//
//	1.VirtualAlloc/VirtualFree
//   ������Ļ�����λ���ڴ�ҳ��
//
//	2.HeapAlloc/HeapFree
//	 �Ӷ��Ϸ���һ���ڴ棬�ҷ�����ڴ��ǲ����ƶ��ģ������û�������Ŀռ����������Ĵ�С��
//	�����ܽ�������ɢ�Ŀռ������������Ӷ����·���ʧ�ܣ�
//
//	3.LocalAlloc/LocalFree
//	 �ú������ڴӾֲ����з����ڴ湩����ʹ��
//
//  4.GlobalAlloc
//	 �ú������ڴ�ȫ�ֶ��з�����ڴ湩����ʹ��
//
//	5.malloc/free
//	 C++/C���Եı�׼�⺯�������������붯̬�ڴ���ͷ��ڴ�
//
//	6._alighed_malloc/aligned_free
//
// 
//	7.new/delete
//	 C++�������
//
//	�ο�����:
//		http://msdn.microsoft.com/en-us/library/windows/desktop/aa366781(v=vs.85).aspx
//---------------------------------------------------------
// ����:		
// ����:		
// ����޸�:	
//*************************************************************************************************
#pragma once
#include <windows.h>

//-----------------------------------------------------------------------------
// DLL�������
//-----------------------------------------------------------------------------
#ifndef HOOKALLOC_EXPORT
  #ifdef HOOKALLOC_IMPORT
	#define HOOKALLOC_EXPORT dllexport
  #else
	#define HOOKALLOC_EXPORT __declspec(dllimport)
  #endif
#endif

// �ο���Gamebryo
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