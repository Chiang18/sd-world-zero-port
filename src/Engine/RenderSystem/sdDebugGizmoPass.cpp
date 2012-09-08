#include "Stdafx.h"
#include "sdDebugGizmoPass.h"
#include "sdRenderDevice_DX9.h"


namespace RenderSystem
{
//-------------------------------------------------------------------------------------------------
sdDebugGizmoPass::sdDebugGizmoPass(sdRenderObjectAlloc* pkAlloc)
: sdRenderPass(pkAlloc)
{
	
}
//-------------------------------------------------------------------------------------------------
sdDebugGizmoPass::~sdDebugGizmoPass()
{
	if (m_bInitialized)
	{
		Destroy();
		m_bInitialized = false;
	}
}
//-------------------------------------------------------------------------------------------------
bool sdDebugGizmoPass::Initialize()
{
	if (m_bInitialized)
	{
		NIASSERT(0);
		return false;
	}

	return (m_bInitialized = true);
}
//-------------------------------------------------------------------------------------------------
void sdDebugGizmoPass::Destroy()
{
	m_bInitialized = false;
}
//-------------------------------------------------------------------------------------------------
void sdDebugGizmoPass::InsertStaticMesh(NiMesh* spMesh)
{
	NIASSERT(m_bInitialized);
	NIASSERT(spMesh);

	// 加入渲染队列
	InsertStaticObject(spMesh, NULL, NULL, NULL);
}
//-------------------------------------------------------------------------------------------------
void sdDebugGizmoPass::Draw()
{
	if (!m_bInitialized || !m_bActived)
		return;


	D3DPERF_BeginEvent(0xff000000, L"DebugGizmoPass");

	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// 清除数据绑定
	pkRenderDevice->ClearVertexBinding();
	//pkRenderDevice->ClearTextureBinding();

	// 绘制
	__super::Draw();

	D3DPERF_EndEvent();
}
//-------------------------------------------------------------------------------------------------
}