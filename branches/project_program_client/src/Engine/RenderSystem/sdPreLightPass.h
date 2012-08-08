//*************************************************************************************************
// 内容:	Light-Buffer的渲染
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-20
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _PRELIGHT_PASS_H__
#define _PRELIGHT_PASS_H__
#include "sdRenderPass.h"

namespace RenderSystem
{

	class sdPreLightPass : public sdRenderPass
	{
	public:
		sdPreLightPass(sdRenderObjectAlloc* pkAlloc);
		virtual ~sdPreLightPass();

		// 虚函数继承
		virtual void Draw();

		// 初始化与销毁
		bool Initialize(uint uiStaticMeshStencilIDMask, NiRenderedTexture* spGeomTexture);
		void Destroy();

		// 添加绘制对象
		//void InsertLight(NiLight* spMesh);

	protected:
		// 模版参数
		uint	m_uiStaticMeshStencilIDMask;

		// 材质
		NiMaterialPtr	m_spPointLightMaterial;
		NiMaterialPtr	m_spSpotLightMaterial;
		NiMaterialPtr	m_spRectSpotLightMaterial;

		// 着色输入纹理
		NiRenderedTexturePtr	m_spGeomTexture;

		// 渲染属性
		NiPropertyList	m_kPropertyList;
	};
	NiSmartPointer(sdPreLightPass);
}
#endif