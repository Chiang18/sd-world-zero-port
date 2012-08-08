//*************************************************************************************************
// 内容:	Geometry-Buffer的渲染
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-14
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _MRT_Z_GEOMETRY_PASS_H__
#define _MRT_Z_GEOMETRY_PASS_H__
#include "sdRenderPass.h"

namespace RenderSystem
{
	class sdMRTZGeometryPass : public sdRenderPass
	{
	public:
		sdMRTZGeometryPass(sdRenderObjectAlloc* pkAlloc);
		virtual ~sdMRTZGeometryPass();

		// 虚函数继承
		virtual void Draw();

		// 初始化与销毁
		bool Initialize(uint uiStaticMeshStencilID, uint uiStaticMeshStencilIDMask);
		void Destroy();

		// 添加绘制对象
		void InsertStaticMesh(NiMesh* spMesh);

	protected:
		// 模版参数
		uint	m_uiStaticMeshStencilID;
		uint	m_uiStaticMeshStencilIDMask;

		// 材质
		std::vector<NiMaterialPtr>	m_kStaticGeometryMaterials;
		std::vector<NiMaterialPtr>	m_kSkinnedGeometryMaterials;

		// 渲染属性
		NiPropertyList				m_kPropertyList;
	};
	NiSmartPointer(sdMRTZGeometryPass);
}
#endif