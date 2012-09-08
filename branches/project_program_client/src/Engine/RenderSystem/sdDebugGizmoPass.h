//*************************************************************************************************
// 内容:	使用默认材质渲染对象,用于绘制调试对象
//---------------------------------------------------------
// 作者:		
// 创建:		2012-09-03
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_RENDERSYSTEM_DEBUGGIZMOPPASS_H__
#define _SD_ENGINE_RENDERSYSTEM_DEBUGGIZMOPPASS_H__
#include "sdRenderPass.h"

namespace RenderSystem
{
	// 对插入的Mesh,使用GB的默认对象来绘制到最终缓存
	class sdDebugGizmoPass : public sdRenderPass
	{
	public:
		sdDebugGizmoPass(sdRenderObjectAlloc* pkAlloc);
		virtual ~sdDebugGizmoPass();

		// 虚函数继承
		virtual void Draw();

		// 初始化与销毁
		bool Initialize();
		void Destroy();

		// 添加绘制对象
		void InsertStaticMesh(NiMesh* spMesh);
	};
	NiSmartPointer(sdDebugGizmoPass);

}
#endif