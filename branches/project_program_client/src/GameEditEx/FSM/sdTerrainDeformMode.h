//*************************************************************************************************
// 内容:	地形高度编辑模式
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-27
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_TERRAINDEFORMMODE_H__
#define _SD_GAMEEDITEX_TERRAINDEFORMMODE_H__
#include "sdGameEditEx.h"
#include "sdEditMode.h"

namespace GameEditEx
{
	class sdTerrainDeformMode : public sdEditMode
	{
	public:
		sdTerrainDeformMode();
		~sdTerrainDeformMode();

		// 虚函数继承
		virtual bool Initialize();
		virtual void Destroy();

	protected:
		
	};
}
#endif