//*************************************************************************************************
// 内容:	Geometry-Buffer的渲染
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-14
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _MRT_SHADING_PASS_H__
#define _MRT_SHADING_PASS_H__
#include "sdRenderPass.h"

namespace RenderSystem
{

	class sdMRTShadingPass : public sdRenderPass
	{
	public:
		sdMRTShadingPass(sdRenderObjectAlloc* pkAlloc);
		virtual ~sdMRTShadingPass();

		// 虚函数继承
		virtual void Draw();

		// 初始化与销毁
		bool Initialize(uint uiStaticMeshStencilID, 
			uint uiStaticMeshStencilIDMask,
			NiRenderedTexture* spLightTexture, 
			NiRenderedTexture* spGeomTexture,
			NiRenderedTexture* spAlbedoTexture,	
			NiRenderedTexture* spGlossTexture);
		void Destroy();

	protected:
		// 模版参数
		uint	m_uiStaticMeshStencilID;
		uint	m_uiStaticMeshStencilIDMask;

		// 材质
		NiMaterialPtr	m_spStaticShadingMaterial;
		NiMaterialPtr	m_spSkinnedShadingMaterial;

		// 着色输入纹理
		NiRenderedTexturePtr	m_spLightTexture;
		NiRenderedTexturePtr	m_spGeomTexture;
		NiRenderedTexturePtr	m_spAlbedoTexture;
		NiRenderedTexturePtr	m_spGlossTexture;

		// 渲染属性
		NiPropertyList	m_kPropertyList;
	};
	NiSmartPointer(sdMRTShadingPass);
}
#endif