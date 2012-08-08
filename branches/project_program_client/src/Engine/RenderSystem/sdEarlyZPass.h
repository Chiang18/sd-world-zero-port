//*************************************************************************************************
// 内容:	EarlyZ的渲染
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-10
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _EARLYZ_PASS_H__
#define _EARLYZ_PASS_H__
#include "sdRenderPass.h"

namespace RenderSystem
{
	// 仅仅写入Depth-Stencil Buffer,可以获取双倍写入速度
	class sdEarlyZPass : public sdRenderPass
	{
	public:
		sdEarlyZPass(sdRenderObjectAlloc* pkAlloc);
		virtual ~sdEarlyZPass();

		// 虚函数继承
		virtual void Draw();

		// 初始化与销毁
		bool Initialize(uint uiStaticMeshStencilID, uint uiStaticMeshStencilIDMask);
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
		uint	m_uiStaticMeshStencilID;
		uint	m_uiStaticMeshStencilIDMask;

		// 材质
		NiMaterialPtr	m_spSolidMeshMaterial;
	};
	NiSmartPointer(sdEarlyZPass);

}
#endif