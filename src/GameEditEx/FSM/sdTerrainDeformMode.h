//*************************************************************************************************
// ����:	���θ߶ȱ༭ģʽ
//---------------------------------------------------------
// ����:		
// ����:		2012-08-27
// ����޸�:
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

		// �麯���̳�
		virtual bool Initialize();
		virtual void Destroy();

	protected:
		
	};
}
#endif