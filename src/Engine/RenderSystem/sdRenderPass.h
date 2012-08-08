//*************************************************************************************************
// 内容:	渲染步骤基础接口
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-04
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _RENDER_PASS_H__
#define _RENDER_PASS_H__
#include <NiRefObject.h>
#include "sdTypes.h"
#include "sdRenderObject.h"

namespace RenderSystem
{
	// 前置申明
	class sdRenderParams;
	class sdRenderObject;
	class sdRenderObjectAlloc;

	// 渲染步骤
	// RenderPass负责将自己的RenderObject队列通过RenderDevice渲染到特定RenderTarget
	class sdRenderPass : public NiRefObject
	{
	public:
		sdRenderPass(sdRenderObjectAlloc* pkAlloc);
		virtual ~sdRenderPass();

		inline bool IsInitialized(){ return m_bInitialized;};

		// 是否激活
		inline bool GetActive() { return m_bActived;};
		inline bool SetActive(bool bActive){ m_bActived = bActive;};

		// 设置渲染参数
		virtual void SetRenderParams(const sdRenderParams* pkRenderParams);

		//@{
		// 绘制接口函数
		virtual void Begin();
		virtual void Draw();
		virtual void End();
		//@}

	protected:
		//@{
		// 添加物队对象到渲染队列
		sdRenderObject::ShaderParamItem* CloneShaderParams(sdRenderObject::ShaderParamItem* pkParams);
		void InsertRenderObject(sdRenderObject* pkObject);
		void InsertStaticObject(NiMesh* spMesh, NiMaterial* spMaterial,
			NiPropertyList* spRenderProps, sdRenderObject::ShaderParamItem* pkShaderParams);
		void ClearObjects();
		//@}

		//@{
		// 默认的排序函数
		typedef bool (*RenderObjectComparator)(sdRenderObject* lhs, sdRenderObject* rhs);
		static bool	Comparator(sdRenderObject* lhs, sdRenderObject* rhs);

		// 获取当前排序函数
		virtual RenderObjectComparator GetComparator();

		// 物件队列排序
		virtual void Sort();
		//@}

	protected:
		bool	m_bInitialized;
		bool	m_bActived;

		typedef std::vector<sdRenderObject*>	RenderObjectVec;
		typedef std::vector<sdRenderObject*>::iterator	RenderObjectVecItr;
		RenderObjectVec			m_kRenderObjectVec;
		sdRenderObjectAlloc*	m_pkAlloc;
		sdRenderParams*			m_pkRenderParams;
	};
	NiSmartPointer(sdRenderPass);
}
#endif