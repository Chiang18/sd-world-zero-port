#include "Stdafx.h"
#include "sdEarlyZPass.h"
#include "sdRenderDevice_DX9.h"


namespace RenderSystem
{

//-------------------------------------------------------------------------------------------------
bool sdEarlyZPass::Comparator(sdRenderObject* lhs, sdRenderObject* rhs)
{
	NIASSERT(lhs);
	NIASSERT(rhs);
	NIASSERT(sdRenderObject::E_STATIC_MESH == lhs->GetType());
	NIASSERT(sdRenderObject::E_STATIC_MESH == rhs->GetType());

	// 键排序(材质)
	if (lhs->GetType() != rhs->GetType())
		return (uint)lhs->GetType() < (uint)rhs->GetType();
	
	// 距离排序(由近到远,也许并不必要)
	NiCamera kCamera;
	NiRenderer::GetRenderer()->GetCameraData(kCamera);
	const NiPoint3& kCamPos = kCamera.GetWorldLocation();
	const NiPoint3& kCamDir = kCamera.GetWorldDirection();
	const NiPoint3& lhsPos = lhs->GetMesh()->GetTranslate();
	const NiPoint3& rhsPos = rhs->GetMesh()->GetTranslate();
	return kCamDir.Dot(lhsPos - kCamPos) < kCamDir.Dot(rhsPos - kCamPos);
}
//-------------------------------------------------------------------------------------------------
sdEarlyZPass::sdEarlyZPass(sdRenderObjectAlloc* pkAlloc)
: sdRenderPass(pkAlloc)
, m_uiStaticMeshStencilID(0)
, m_uiStaticMeshStencilIDMask(0)
{
	
}
//-------------------------------------------------------------------------------------------------
sdEarlyZPass::~sdEarlyZPass()
{
	if (m_bInitialized)
	{
		Destroy();
		m_bInitialized = false;
	}
}
//-------------------------------------------------------------------------------------------------
bool sdEarlyZPass::Initialize(uint uiStaticMeshStencilID, uint uiStaticMeshStencilIDMask)
{
	if (m_bInitialized)
	{
		NIASSERT(0);
		return false;
	}

	// 设置模版参数
	m_uiStaticMeshStencilID = uiStaticMeshStencilIDMask;
	m_uiStaticMeshStencilIDMask = uiStaticMeshStencilIDMask;

	// 初始化材质
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	m_spSolidMeshMaterial = pkRenderDevice->CreateMaterial("StaticMesh_Solid_EarlyZ");
	NIASSERT(m_spSolidMeshMaterial);

	return (m_bInitialized = true);
}
//-------------------------------------------------------------------------------------------------
void sdEarlyZPass::Destroy()
{
	m_spSolidMeshMaterial = 0;
	m_uiStaticMeshStencilID = 0;
	m_uiStaticMeshStencilIDMask = 0;

	m_bInitialized = false;
}
//-------------------------------------------------------------------------------------------------
void sdEarlyZPass::InsertStaticMesh(NiMesh* spMesh)
{
	NIASSERT(m_bInitialized);
	NIASSERT(spMesh);

	// 检查是否是骨骼动画
	if (spMesh->RequiresMaterialOption(NiRenderObjectMaterialOption::TRANSFORM_SKINNED()))
	{
		//NIASSERT(0);
		return;
	}

	// 检查是否是实例化几何体
	if (spMesh->RequiresMaterialOption(NiRenderObjectMaterialOption::TRANSFORM_INSTANCED()))
	{
		//NIASSERT(0);
		return;
	}
		

	// 加入渲染队列
	InsertStaticObject(spMesh, m_spSolidMeshMaterial, NULL, NULL);
}
//-------------------------------------------------------------------------------------------------
void sdEarlyZPass::Draw()
{
	if (!m_bInitialized || !m_bActived)
		return;

	D3DPERF_BeginEvent(0xff000000, L"EarlyPass");

	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// 清除数据绑定
	pkRenderDevice->ClearVertexBinding();
	//pkRenderDevice->ClearTextureBinding();

	// Alpha
	pkRenderDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,	false);
	pkRenderDevice->SetRenderState(D3DRS_ALPHATESTENABLE,	false);

	// Z
	pkRenderDevice->SetRenderState(D3DRS_ZENABLE,			true);
	pkRenderDevice->SetRenderState(D3DRS_ZWRITEENABLE,		true);
	pkRenderDevice->SetRenderState(D3DRS_ZFUNC,				D3DCMP_LESSEQUAL);

	// Stencil
	if (m_uiStaticMeshStencilID)
	{
		pkRenderDevice->SetRenderState(D3DRS_STENCILENABLE,		true);
		pkRenderDevice->SetRenderState(D3DRS_STENCILFUNC,		D3DCMP_ALWAYS);
		pkRenderDevice->SetRenderState(D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP);
		pkRenderDevice->SetRenderState(D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP);
		pkRenderDevice->SetRenderState(D3DRS_STENCILPASS,		D3DSTENCILOP_KEEP);
		pkRenderDevice->SetRenderState(D3DRS_STENCILREF,		m_uiStaticMeshStencilID);
		pkRenderDevice->SetRenderState(D3DRS_STENCILWRITEMASK,	m_uiStaticMeshStencilIDMask);
	}
	else
	{
		pkRenderDevice->SetRenderState(D3DRS_STENCILENABLE,		false);
	}

	// 禁用第一个颜色缓存,以获取双倍输出
	pkRenderDevice->SetRenderState(D3DRS_COLORWRITEENABLE,	0);

	// 绘制
	__super::Draw();

	// 恢复第一个颜色输出
	uint uiColorChannelMask = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |	D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA;
	pkRenderDevice->SetRenderState(D3DRS_COLORWRITEENABLE,	uiColorChannelMask);

	D3DPERF_EndEvent();
}
//-------------------------------------------------------------------------------------------------
sdRenderPass::RenderObjectComparator sdEarlyZPass::GetComparator()
{
	return sdEarlyZPass::Comparator;
}
//-------------------------------------------------------------------------------------------------
}