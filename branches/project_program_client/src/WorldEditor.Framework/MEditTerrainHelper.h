//*************************************************************************************************
// ����:	���α༭������
//---------------------------------------------------------
// ����:		
// ����:		2012-09-21
// ����޸�:
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

		// ��ˢ����
		// @{
		//property float OutterRadius
		//{
		//	void set(float fRadius){ }
		//}
		// @}

		// ���θ߶ȱ༭
		// @{
		//void SetOuterRadius(float fRadius);
		//float GetOuterRadius();

		//void SetInnerRadius(float fRadius);
		//float GetInnerRadius();
		// @}

		
		// ��������༭
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