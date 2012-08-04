#include "Stdafx.h"
#include "sdMRTZGeometryPass.h"
#include "sdRenderSystem.h"
#include "sdRenderDevice_DX9.h"

namespace RenderSystem
{
//-------------------------------------------------------------------------------------------------
sdMRTZGeometryPass::sdMRTZGeometryPass(sdRenderObjectAlloc* pkAlloc)
: sdRenderPass(pkAlloc)
, m_uiStaticMeshStencilID(0)
, m_uiStaticMeshStencilIDMask(0)
{

}
//-------------------------------------------------------------------------------------------------
sdMRTZGeometryPass::~sdMRTZGeometryPass()
{
	if (m_bInitialized)
	{
		Destroy();
		m_bInitialized = false;
	}
}
//-------------------------------------------------------------------------------------------------
bool sdMRTZGeometryPass::Initialize(uint uiStaticMeshStencilID, uint uiStaticMeshStencilIDMask)
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
	NIASSERT(pkRenderDevice);

	uint uiNum = (uint)pow(2.0f, 7.0f);
	char szMaterialName[512];
	m_kStaticGeometryMaterials.resize(uiNum);
	m_kSkinnedGeometryMaterials.reserve(uiNum);
	for (uint i = 0; i < uiNum; ++i)
	{
		sprintf(szMaterialName, "MRT3Geometry0x%08X", i);
		m_kStaticGeometryMaterials[i] = pkRenderDevice->CreateMaterial(szMaterialName);
		
		//sprintf(szMaterialName, "MRT3SkinnedGeometry0x%08X", i);
		//m_kSkinnedGeometryMaterials[i] = pkRenderDevice->CreateMaterial(szMaterialName);
	}

	// 
	NiTexturingPropertyPtr spTexturingProp  = NiNew NiTexturingProperty;
	NIASSERT(spTexturingProp);
	spTexturingProp->SetShaderMap(0, NiNew NiTexturingProperty::ShaderMap(NULL, 0));
	spTexturingProp->SetShaderMap(1, NiNew NiTexturingProperty::ShaderMap(NULL, 0));
	m_kPropertyList.AddTail((NiTexturingProperty*)spTexturingProp);

	return (m_bInitialized = true);
}
//-------------------------------------------------------------------------------------------------
void sdMRTZGeometryPass::Destroy()
{
	m_kPropertyList.RemoveAll();

	for (uint i = 0; i < m_kStaticGeometryMaterials.size(); ++i)
		m_kStaticGeometryMaterials[i] = 0;
	m_kStaticGeometryMaterials.clear();

	for (uint i = 0; i < m_kSkinnedGeometryMaterials.size(); ++i)
		m_kSkinnedGeometryMaterials[i] = 0;
	m_kSkinnedGeometryMaterials.clear();

	m_uiStaticMeshStencilID = 0;
	m_uiStaticMeshStencilIDMask = 0;

	m_bInitialized = false;
}
//-------------------------------------------------------------------------------------------------
void sdMRTZGeometryPass::InsertStaticMesh(NiMesh* spMesh)
{
	typedef sdRenderObject::ShaderParamItem	ShaderParamItem;

	NIASSERT(m_bInitialized);
	NIASSERT(spMesh);

	// 检查输入Geometry的NiProperty
	NiPropertyState* spPropState = spMesh->GetPropertyState();
	if (!spPropState)
	{
		NIASSERT(0);
		return;
	}

	NiTexturingProperty* spTexProp = spPropState->GetTexturing();
	NiMaterialProperty* spMatProp = spPropState->GetMaterial();
	NiAlphaProperty* spAlphaProp = spPropState->GetAlpha();
	if (!spTexProp || !spMatProp || !spAlphaProp)
	{
		NIASSERT(0);
		return;
	}

	bool bSkinned = spMesh->RequiresMaterialOption(NiRenderObjectMaterialOption::TRANSFORM_SKINNED());

	// 构建ShaderParamItem数组,传入对象Material参数
	// @{
	static const sdFixedString s_szDiffuseName("g_vDiffuseMaterial",		true);
	static const sdFixedString s_szSpecularName("g_vSpeculatMaterial",		true);
	static const sdFixedString s_szEmissiveName("g_vEmissiveMaterial",		true);
	static const sdFixedString s_szAlphaTestName("g_fAlphaTestRef",			true);
	static const sdFixedString s_szEdgeEnhancementName("g_fEdgeEnhancement",	true);
 
	NiColor kDiffuseColor = spMatProp->GetDiffuseColor();
	NiColor kSpecularColor = spMatProp->GetSpecularColor();
	NiColor kEmissiveColor = spMatProp->GetEmittance();
	float fShiness = spMatProp->GetShineness();

	NiPoint4 kEmissive(kEmissiveColor.r, kEmissiveColor.g, kEmissiveColor.b, 1.0f);
	NiPoint4 kDiffuse(kDiffuseColor.r, kDiffuseColor.g, kDiffuseColor.b, 0);
	NiPoint4 kSpecular(kSpecularColor.r, kSpecularColor.g, kSpecularColor.b, fShiness);

	float fEdge = 0.0f;
	float fAlphaTestRef = 0.0f;
	if (spAlphaProp->GetAlphaTesting())
	{
		fAlphaTestRef = 128.0f / 255.0f + 0.001f;	// 不知道为什么加上0.001f
	}

	ShaderParamItem kShaderParams[] = {
		{&s_szDiffuseName,			sizeof(float) * 4,		(void*)&kDiffuse},
		{&s_szSpecularName,			sizeof(float) * 4,		(void*)&kSpecular},
		{&s_szEmissiveName,			sizeof(float) * 4,		(void*)&kEmissive},
		{&s_szEdgeEnhancementName,	sizeof(float),			(void*)&fEdge},
		{&s_szAlphaTestName,		sizeof(float),			(void*)&fAlphaTestRef},
		{NULL,						0,						NULL}};
	// }@

	// 根据输入纹理情况,确定所使用的着色器掩码(最大2^7种组合)
	// base
	// normal
	// gloss
	// drakmap		/ 
	// detail		/
	// alphatest
	// vertexcolor
	// @{
	bool bEffectType[7] = {false, false, false, false, false, false, false};

	if (spTexProp->GetBaseMap())	bEffectType[0] = true;
	if (spTexProp->GetNormalMap())	bEffectType[1] = true;
	if (spTexProp->GetGlossMap())	bEffectType[3] = true;

	if (bSkinned)
	{
		
	}
	else
	{
		// 只有静态物件才有DrakMap/LightMap和ParallaxMap
		if (spTexProp->GetDarkMap())		bEffectType[3] = true;
		if (spTexProp->GetParallaxMap())	bEffectType[4] = true;
	}

	if (NiAlphaProperty::TEST_GREATER == spAlphaProp->GetTestMode() ||
		NiAlphaProperty::TEST_GREATEREQUAL == spAlphaProp->GetTestMode())
	{
		bEffectType[5] = true;
	}

	if (spMesh->FindStreamRef(NiCommonSemantics::COLOR())) bEffectType[6] = true;

	// 计算对应着色器ID
	uint uiTypeFlag = 0;
	for (uint i = 0; i < 7; ++i)
	{
		if (bEffectType[i])
			uiTypeFlag |= 1 << i;
	}
	// @}

	// 插入Mesh到绘制队列
	if (bSkinned)
	{
		//uiTypeFlag &= m_pkRenderParams->actorChannelMask;
		//InsertStaticObject(spMesh, m_kSkinnedGeometryMaterials[uiTypeFlag], &m_kPropertyList, kShaderParams);
	}
	else
	{
		uiTypeFlag &= m_pkRenderParams->buildingChannelMask;
		InsertStaticObject(spMesh, m_kStaticGeometryMaterials[uiTypeFlag], &m_kPropertyList, kShaderParams);
	}
}
//-------------------------------------------------------------------------------------------------
void sdMRTZGeometryPass::Draw()
{
	D3DPERF_BeginEvent(0xff000000, L"MRTZGeometryPass");

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
	pkRenderDevice->SetRenderState(D3DRS_STENCILENABLE,		true);
	pkRenderDevice->SetRenderState(D3DRS_STENCILFUNC,		D3DCMP_ALWAYS);
	pkRenderDevice->SetRenderState(D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP);
	pkRenderDevice->SetRenderState(D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP);
	pkRenderDevice->SetRenderState(D3DRS_STENCILPASS,		D3DSTENCILOP_REPLACE);
	pkRenderDevice->SetRenderState(D3DRS_STENCILREF,		m_uiStaticMeshStencilID);
	pkRenderDevice->SetRenderState(D3DRS_STENCILWRITEMASK,	m_uiStaticMeshStencilIDMask);

	// 设置Texture Sample状态


	// 绘制
	__super::Draw();

	// 恢复Texture Sample状态


	D3DPERF_EndEvent();
}
//-------------------------------------------------------------------------------------------------
}