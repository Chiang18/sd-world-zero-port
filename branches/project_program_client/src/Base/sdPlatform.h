//*************************************************************************************************
// ����: ���ڶ���һЩ��ϵ�ṹ��غ궨��,���ļ���ȡ��Ogre/OgrePlatform.h)
//---------------------------------------------------------
// ����:	
// ����:		2012-08-04
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_BASE_PLATFORM_H__
#define _SD_BASE_PLATFORM_H__

namespace Base
{
	
	// ������ڵ�����̬��Ͷ�̬��ĺ궨��
	#if defined(SD_STATIC_LIB)
		#define _SDExport
		#define _SDPrivate
	#else
		#define _SDExport	__declspec(dllimport)
		#define _SDPrivate
	#endif

	// ����Debug/Releaseģʽ
	#ifdef _DEBUG
		#define SD_DEBUG_MODE 1
	#else
		#define SD_DEBUG_MODE 0
	#endif

}
#endif