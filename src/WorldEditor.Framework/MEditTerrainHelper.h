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

		// 笔刷属性
		// @{
		//property float OutterRadius
		//{
		//	void set(float fRadius){ }
		//}
		// @}

		// 地形高度编辑
		// @{
		//void SetOuterRadius(float fRadius);
		//float GetOuterRadius();

		//void SetInnerRadius(float fRadius);
		//float GetInnerRadius();
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