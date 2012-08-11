//*************************************************************************************************
// ����:	��ͼͼ��
//---------------------------------------------------------
// ����:		
// ����:		2012-08-11
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_TEXTURE_ATLAS_H__
#define _SD_ENGINE_TEXTURE_ATLAS_H__
#include "sdTypes.h"
#include "sdTexture.h"

namespace RenderSystem
{
	// �Ӷ���Texture������һ�Ŵ����ͼͼ��
	//	1.ÿҳ��ͼ����D3D�ڴ�����
	//	2.ÿҳ��ͼ����ͼ������ͬ�ĸ�ʽ
	//	3.ÿҳ��ͼΪ��ͼ��ָ���ߴ�(�ߴ������pow(2,n),�Ҵ�С���)
	class sdTextureAtlas : public sdTexture
	{
	public:
		sdTextureAtlas();
		~sdTextureAtlas();

		// 
		bool	Update();

		// ͼ��������ͼ����
		// @{
		// ���β��Ӻ�ɾ������(ʧ�ܷ���-1,��0xffffffff)
		uint	AddTexture(NiTexture* spTexture, float fUVRepeat);
		bool	RemoveTexture();

		// ������λ����Ӻ�ɾ������
		uint	InsertTexture(uint uiIndex, NiTexture* spTexture, float fUVRepeat);
		bool	RemoveTexture(uint uiIndex);

		// ����
		bool	SwapTexture(uint uiLIndex, uint uiRIndex);

		// ����ָ��λ������
		bool	SetTexture(uint uiIndex, NiTexture* spTexture, float fUVRepeat);
		// @}
		
	protected:
		//
		bool	UpdateAtlasAndTable();
		bool	UpdateRenderTexture();

		// ������������ͼ������
		bool	CreateAtlasTexture();
		void	DestroyAtlasTexture();

		// ������������ͼ�����ұ�
		bool	CreateAtlasTableTexture();
		void	DestroyAtlasTableTexture();

		// �豸�ص�����
		virtual bool OnDeviceLost();
		virtual bool OnDeviceReset(bool bBefore);

	protected:
		// ��ͼ����һҳ��ͼ
		class TexturePageInfo : public NiMemObject
		{
		public:
			NiTexture*	m_spTexture;
			float		m_fUVRepeat;
			float		m_ffMipmapBias;
		};
		typedef std::vector<TexturePageInfo*> TexturePageInfoVec;
		typedef std::vector<TexturePageInfo*>::iterator	TexturePageInfoVecItr;

	protected:
		D3DFORMAT	m_eFormat;
		uint		m_uiSize;

		// ����ͼ��
		NiTexturePtr	m_spAtlasTexture;	
		NiTexturePtr	m_spAtlasTable;

		// ͼ��������ͼ��Ϣ
		TexturePageInfoVec	m_kTexturePageInfoVec;
	};
	NiSmartPointer(sdTextureAtlas);
}
#endif