//*************************************************************************************************
// 内容:	Terrain着色
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-09
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _TERRAIN_ATLAS_SHADING_PASS_H__
#define _TERRAIN_ATLAS_SHADING_PASS_H__
#include "sdRenderPass.h"

namespace RenderSystem
{

	// 前置声明
	class sdTerrainParams;

	// 地形渲染到几何缓存
	class sdTerrainAtlasShadingPass : public sdRenderPass
	{
	public:
		sdTerrainAtlasShadingPass(sdRenderObjectAlloc* pkAlloc);
		virtual ~sdTerrainAtlasShadingPass();

		// 虚函数继承
		virtual void Draw();

		// 初始化与销毁
		bool Initialize(
			uint uiStencilRef, 
			uint uiStencilMask,
			NiRenderedTexture* spLightTexture, 
			NiRenderedTexture* spGeomTexture);
		void Destroy();

		// 设置地形渲染信息(用于更新内部的一些贴图信息)
		void SetTerrainParams(const sdTerrainParams& kTerrainParams);

	protected:
		// 模版参数
		uint	m_uiStencilRef;
		uint	m_uiStencilMask;
	
		// 材质
		NiMaterialPtr	m_spPlanarShadingMaterial;
		NiMaterialPtr	m_spSeamShadingMaterial;

		// 着色输入纹理
		NiRenderedTexturePtr	m_spLightTexture;
		NiRenderedTexturePtr	m_spGeomTexture;

		// 地形渲染属性
		NiPropertyList			m_kPropertyList;
		NiTexturingPropertyPtr	m_spTexturingProp;
	};
	NiSmartPointer(sdTerrainAtlasShadingPass);
}
#endif