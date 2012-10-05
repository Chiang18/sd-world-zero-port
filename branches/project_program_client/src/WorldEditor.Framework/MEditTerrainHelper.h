//*************************************************************************************************
// 内容:	地形编辑帮助类
//---------------------------------------------------------
// 作者:		
// 创建:		2012-09-21
// 最后修改:
//*************************************************************************************************
#pragma once

//
namespace GameEditEx
{
	class sdEditTerrainHelper;
}

namespace WorldEditorFramework 
{
	public ref class MEditTerrainHelper
	{
	public:
		MEditTerrainHelper();

		// 地形高度编辑
		// @{
		property int DeformBrushType
		{
			void set(int iType);
			int get();
		}

		property float DeformBrushOutterRadius
		{
			void set(float fRadius);
			float get();
		}

		property float DeformBrushInnerRadius
		{
			void set(float fRadius);
			float get();
		}

		property float DeformBrushHardness
		{
			void set(float fHardness);
			float get();
		}

		property float DeformBrushHeight
		{
			void set(float fHeight);
			float get();
		}
		// @}
		
		// 地形纹理编辑
		// @{
		unsigned int GetLayerMapCount();

		unsigned int GetActiveLayerMap();
		void SetActiveLayerMap(unsigned int uiIndex);

		System::String^ GetLayerMapName(unsigned int uiIndex);
		void SetLayerMapName(unsigned int uiIndex, System::String^ szName);
		// @}

	protected:
		GameEditEx::sdEditTerrainHelper* m_pkEditTerrainHelper;
	};
}