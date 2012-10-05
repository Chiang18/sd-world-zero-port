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

		// ���θ߶ȱ༭
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