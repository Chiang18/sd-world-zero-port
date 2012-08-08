#include "Stdafx.h"
#include "sdTerrainDepthPass.h"
#include "sdRenderDevice_DX9.h"

namespace RenderSystem
{

//-------------------------------------------------------------------------------------------------
bool sdTerrainDepthPass::Comparator(sdRenderObject* lhs, sdRenderObject* rhs)
{
	NIASSERT(lhs);
	NIASSERT(rhs);
	NIASSERT(sdRenderObject::E_STATIC_MESH == lhs->GetType());
	NIASSERT(sdRenderObject::E_STATIC_MESH == rhs->GetType());

	// 距离排序(由近到远)
	// (这里貌似无用,有待改进,因为TileMesh没有被位移,位移在Shader里面进行)
	NiCamera kCamera;
	NiRenderer::GetRenderer()->GetCameraData(kCamera);
	const NiPoint3& kCamPos = kCamera.GetWorldLocation();
	const NiPoint3& kCamDir = kCamera.GetWorldDirection();
	const NiPoint3& lhsPos = lhs->GetMesh()->GetTranslate();
	const NiPoint3& rhsPos = rhs->GetMesh()->GetTranslate();
	return kCamDir.Dot(lhsPos - kCamPos) < kCamDir.Dot(rhsPos - kCamPos);
}
//-------------------------------------------------------------------------------------------------
sdTerrainDepthPass::sdTerrainDepthPass(sdRenderObjectAlloc* pkAlloc)
: sdRenderPass(pkAlloc)
, m_uiStencilRef(0)
, m_uiStencilWriteMask(0)
{

}
//-------------------------------------------------------------------------------------------------
sdTerrainDepthPass::~sdTerrainDepthPass()
{
	if (m_bInitialized)
	{
		Destroy();
		m_bInitialized = false;
	}
}
//-------------------------------------------------------------------------------------------------
bool sdTerrainDepthPass::Initialize(uint uiStencilRef, uint miStencilWriteMask)
{
	NIASSERT(uiStencilRef & miStencilWriteMask);

	if (m_bInitialized)
	{
		NIASSERT(0);
		return false;
	}

	//
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// 设置模版参数

	m_uiStencilRef = uiStencilRef;
	m_uiStencilWriteMask = miStencilWriteMask;

	// 初始化材质
	m_spMaterial = pkRenderDevice->CreateMaterial("Terrain_Depth");
	NIASSERT(m_spMaterial);

	return (m_bInitialized = true);
}
//-------------------------------------------------------------------------------------------------
void sdTerrainDepthPass::Destroy()
{
	m_spMaterial = 0;
	m_uiStencilRef = 0;
	m_uiStencilWriteMask = 0;

	m_bInitialized = false;
}
//-------------------------------------------------------------------------------------------------
void sdTerrainDepthPass::InsertStaticMesh(NiMesh* spMesh)
{
	NIASSERT(m_bInitialized);
	NIASSERT(spMesh);

	// 加入渲染队列
	InsertStaticObject(spMesh, m_spMaterial, NULL, NULL);
}
//-------------------------------------------------------------------------------------------------
void sdTerrainDepthPass::Draw()
{
	if (!m_bInitialized || !m_bActived)
		return;

	D3DPERF_BeginEvent(0xff000000, L"TerrainDepthPass");

	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// 清除数据绑定
	pkRenderDevice->ClearVertexBinding();
	//pkRenderDevice->ClearTextureBinding();

	// CullMode
	pkRenderDevice->SetRenderState(D3DRS_CULLMODE,			D3DCULL_NONE);

	// FillMode
	pkRenderDevice->SetRenderState(D3DRS_FILLMODE,			D3DFILL_SOLID);

	// Alpha
	pkRenderDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,	false);
	pkRenderDevice->SetRenderState(D3DRS_ALPHATESTENABLE,	false);

	// Z
	pkRenderDevice->SetRenderState(D3DRS_ZENABLE,			true);
	pkRenderDevice->SetRenderState(D3DRS_ZWRITEENABLE,		true);
	pkRenderDevice->SetRenderState(D3DRS_ZFUNC,				D3DCMP_LESSEQUAL);

	// Stencil
	pkRenderDevice->SetRenderState(D3DRS_STENCILENABLE,		true);
	pkRenderDevice->SetRenderState(D3DRS_STENCILFUNC,		D3DCMP_ALWAYS);
	pkRenderDevice->SetRenderState(D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP);
	pkRenderDevice->SetRenderState(D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP);
	pkRenderDevice->SetRenderState(D3DRS_STENCILPASS,		D3DSTENCILOP_REPLACE);
	pkRenderDevice->SetRenderState(D3DRS_STENCILREF,		m_uiStencilRef);
	pkRenderDevice->SetRenderState(D3DRS_STENCILMASK,		0xffffffff);
	pkRenderDevice->SetRenderState(D3DRS_STENCILWRITEMASK,	m_uiStencilWriteMask);

	// 允许第一个颜色缓存输出
	uint uiColorChannelMask = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |	D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA;
	pkRenderDevice->SetRenderState(D3DRS_COLORWRITEENABLE,	uiColorChannelMask);

	// 绘制
	__super::Draw();

	D3DPERF_EndEvent();
}
//-------------------------------------------------------------------------------------------------
sdRenderPass::RenderObjectComparator sdTerrainDepthPass::GetComparator()
{
	return sdTerrainDepthPass::Comparator;
}
//-------------------------------------------------------------------------------------------------
}