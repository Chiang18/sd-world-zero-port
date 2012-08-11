//*************************************************************************************************
// 内容:	动态纹理
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-07
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_DYNAMIC_TEXTURE_H__
#define _SD_ENGINE_DYNAMIC_TEXTURE_H__
#include "sdTypes.h"
#include "sdTexture.h"

namespace RenderSystem
{

	// 直接封装D3D纹理
	//	1.解决了GB的NiDynamicTexture纹理的不能部分更新的问题
	//	2.先同步到D3D内存纹理,再同步到D3D显存纹理(同步)
	//	3.设备丢失与重设必须自行处理
	// 
	//	(更优秀解决方案可能必须嵌入到NiDX9Renderer内部去,类似如下)
	//	WZ另一个版本考虑修改NiSourceTexture,用NiPixelData来创建NiSourceTexture,
	//	通过动态更新NiPixelData来处理
	//		NiPixelDataPtr	m_spPixelData;
	//		NiSourceTexturePtr	m_spTexture;
	//
	class sdDynamicTexture : public sdTexture
	{
	public:
		// 当前仅仅支持以下三种格式
		enum eDynamicFormat
		{
			E_FORMAT_UNKNOWN = 0,	// 默认使用32位格式
			E_FORMAT_A8,
			E_FORMAT_A4R4G4B4,
			E_FORMAT_R8G8B8,
			E_FORMAT_A8R8G8B8,
			NUM_FORMATS,
		};

		// 锁定区域的返回值(如D3D)
		// (D3D返回的不是一块连续区域,而是pBits起始的一组数据,其行距为pBits)
		struct sdLockedRect
		{
			uint	uiPitch;
			void*	pBits;
		};
		
	public:
		sdDynamicTexture(uint uiWidth, uint uiHeight, eDynamicFormat eFormat = E_FORMAT_UNKNOWN, bool bMipmap = false);
		~sdDynamicTexture();

		// 
		sdLockedRect LockRegion(uint uiX, uint uiY, uint uiW, uint uiH);
		void UnlockRegion();

		//
		uint	GetWidth() const { return m_uiWidth;}
		uint	GetHeight() const { return m_uiHeight;}
		eDynamicFormat GetFormat() const { return m_eFormat;}

		//
		NiTexture*	GetGBTexture() const { return m_spTexture;}

	protected:
		// 设备回调处理
		virtual bool OnDeviceLost();
		virtual bool OnDeviceReset(bool bBefore);

	protected:
		// 
		uint	m_uiWidth;
		uint	m_uiHeight;
		bool	m_bMipmap;			// 支持2级mipmap
		eDynamicFormat m_eFormat;	// 支持限定格式

		// 纹理
		NiTexturePtr		m_spTexture;		// 用来包装D3D纹理的GB纹理对象
		LPDIRECT3DTEXTURE9	m_spD3DSysTexture;	// D3D内存纹理,直接被更新的纹理
		LPDIRECT3DTEXTURE9	m_spD3DVedTexture;	// D3D显存纹理,最终被更新和使用的纹理

		// 渲染设备
		NiDX9Renderer*		m_spRenderer;
		LPDIRECT3DDEVICE9	m_spD3DDevice;

		// D3D格式查找表格
		static D3DFORMAT ms_kD3DFormatTable[NUM_FORMATS];
	};
	NiSmartPointer(sdDynamicTexture);
}
#endif