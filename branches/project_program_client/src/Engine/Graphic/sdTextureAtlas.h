//*************************************************************************************************
// 内容:	贴图图集
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-11
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_TEXTURE_ATLAS_H__
#define _SD_ENGINE_TEXTURE_ATLAS_H__
#include "sdTypes.h"
#include "sdTexture.h"

namespace RenderSystem
{
	// 从多张Texture来生成一张大的贴图图集
	//	1.每页贴图都是D3D内存纹理
	//	2.每页贴图与贴图集有相同的格式
	//	3.每页贴图为贴图集指定尺寸(尺寸必须是pow(2,n),且大小相等)
	class sdTextureAtlas : public sdTexture
	{
	public:
		sdTextureAtlas();
		~sdTextureAtlas();

		// 
		bool	Update();

		// 图集所属贴图操作
		// @{
		// 向结尾添加和删除纹理(失败返回-1,即0xffffffff)
		uint	AddTexture(NiTexture* spTexture, float fUVRepeat);
		bool	RemoveTexture();

		// 向任意位置添加和删除纹理
		uint	InsertTexture(uint uiIndex, NiTexture* spTexture, float fUVRepeat);
		bool	RemoveTexture(uint uiIndex);

		// 交换
		bool	SwapTexture(uint uiLIndex, uint uiRIndex);

		// 设置指定位置纹理
		bool	SetTexture(uint uiIndex, NiTexture* spTexture, float fUVRepeat);
		// @}
		
	protected:
		//
		bool	UpdateAtlasAndTable();
		bool	UpdateRenderTexture();

		// 创建与销毁贴图集纹理
		bool	CreateAtlasTexture();
		void	DestroyAtlasTexture();

		// 创建与销毁贴图集查找表
		bool	CreateAtlasTableTexture();
		void	DestroyAtlasTableTexture();

		// 设备回调处理
		virtual bool OnDeviceLost();
		virtual bool OnDeviceReset(bool bBefore);

	protected:
		// 贴图集的一页贴图
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

		// 最终图集
		NiTexturePtr	m_spAtlasTexture;	
		NiTexturePtr	m_spAtlasTable;

		// 图集所属贴图信息
		TexturePageInfoVec	m_kTexturePageInfoVec;
	};
	NiSmartPointer(sdTextureAtlas);
}
#endif