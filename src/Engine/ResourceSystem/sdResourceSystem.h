//*************************************************************************************************
// 内容:	资源系统
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-11
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_RESOURCE_SYSTEM_H__
#define _SD_ENGINE_RESOURCE_SYSTEM_H__
#include "sdResourceDefine.h"

// Base
#include <sdSingleton.h>


namespace ResourceSystem
{
	// 资源系统,负责资源加载,管理,销毁
	//
	// 资源路径约定:
	//	1."."起头和".."起头为相对路径
	//	2.其余视作绝对路径
	//
	class sdResourceSystem : public Base::sdTSingleton<sdResourceSystem>
	{
	public:
		sdResourceSystem();
		virtual ~sdResourceSystem();

		// 初始化与销毁
		virtual bool Initialize();
		virtual void Destroy();
		virtual void Update();

		// 异步加载(Asynchronous)
		// @{



		// @}


		// 同步加载(Synchronous)
		// @{
		virtual NiSourceTexturePtr LoadTexture(const std::string& szFileName, bool bMipmap = true);


		// @}

	protected:


	protected:

	};
}
#endif