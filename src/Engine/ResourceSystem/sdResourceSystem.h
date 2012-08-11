//*************************************************************************************************
// ����:	��Դϵͳ
//---------------------------------------------------------
// ����:		
// ����:		2012-08-11
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_RESOURCE_SYSTEM_H__
#define _SD_ENGINE_RESOURCE_SYSTEM_H__
#include "sdResourceDefine.h"

// Base
#include <sdSingleton.h>


namespace ResourceSystem
{
	// ��Դϵͳ,������Դ����,����,����
	//
	// ��Դ·��Լ��:
	//	1."."��ͷ��".."��ͷΪ���·��
	//	2.������������·��
	//
	class sdResourceSystem : public Base::sdTSingleton<sdResourceSystem>
	{
	public:
		sdResourceSystem();
		virtual ~sdResourceSystem();

		// ��ʼ��������
		virtual bool Initialize();
		virtual void Destroy();
		virtual void Update();

		// �첽����(Asynchronous)
		// @{



		// @}


		// ͬ������(Synchronous)
		// @{
		virtual NiSourceTexturePtr LoadTexture(const std::string& szFileName, bool bMipmap = true);


		// @}

	protected:


	protected:

	};
}
#endif