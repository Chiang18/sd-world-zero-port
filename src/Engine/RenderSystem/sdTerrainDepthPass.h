//*************************************************************************************************
// 内容:	Terrain深度预渲染到ZBuffer
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-04
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _TERRAIN_DEPTH_PASS_H__
#define _TERRAIN_DEPTH_PASS_H__
#include "sdRenderPass.h"

namespace RenderSystem
{
	// 1.渲染地形深度到一张纹理
	// 2.渲染地形到模板缓存
	class sdTerrainDepthPass : public sdRenderPass
	{
	public:
		sdTerrainDepthPass(sdRenderObjectAlloc* pkAlloc);
		virtual ~sdTerrainDepthPass();

		// 虚函数继承
		virtual void Draw();

		// 初始化与销毁
		bool Initialize(uint uiStencilRef, uint uiStencilWriteMask);
		void Destroy();

		// 添加绘制对象
		void InsertStaticMesh(NiMesh* spMesh);

	protected:
		//@{
		// 虚函数继承
		virtual RenderObjectComparator GetComparator();

		// 定制的排序比较器
		static bool	Comparator(sdRenderObject* lhs, sdRenderObject* rhs);
		//@}

	protected:
		// 模版参数
		uint	m_uiStencilRef;
		uint	m_uiStencilWriteMask;

		// 材质
		NiMaterialPtr	m_spMaterial;
	};
	NiSmartPointer(sdTerrainDepthPass);
}
#endif