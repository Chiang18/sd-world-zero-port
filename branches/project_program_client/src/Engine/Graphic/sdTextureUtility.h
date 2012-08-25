//*************************************************************************************************
// 内容:	纹理工具,主要是辅助处理D3D纹理
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-11
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_TEXTURE_UTILITY_H__
#define _SD_ENGINE_TEXTURE_UTILITY_H__
#include "sdTypes.h"

namespace RenderSystem
{
	// 纹理工具,依赖于D3D
	class sdTextureUtility : public NiMemObject
	{
	public:
		// 用作输入输出的目标缓存
		struct stBuffer
		{
			uint	uiSize;
			void*	pBits;
		};

	public:
		// 纹理压缩
		//	@kDstBuffer			: 输出目标缓存
		//	@pSrcData			: 输入数据
		//	@uiWidth,uiHeight	: 输入数据数据尺寸
		//	@uiBytesPerPixel	: 输入数据每像素通道数/字节数(8:1 16:2 ...)
		//	@eFormat			: 目标压缩格式,目前只支持DXT1~DXT5
		//static bool Compress(stBuffer& kDstBuffer,void* pSrcData, uint uiWidth, uint uiHeight, uint uiBytesPerPixel, D3DFORMAT eFormat);
		
		// 纹理解压缩
		//	@kDstBuffer			: 输出目标缓存
		//	@kSrcBuffer			: 输入缓存
		//	@uiBytesPerPixel	: 输入数据每像素通道数
		//static bool Uncompress(stBuffer& kDstBuffer, stBuffer& kSrcBuffer, uint uiBytesPerPixel);


		// 检查纹理格式
		static bool	IsDDSFormat(D3DFORMAT eFormat);
		static bool IsZStencilFormat(D3DFORMAT eFormat);
	};
}
#endif