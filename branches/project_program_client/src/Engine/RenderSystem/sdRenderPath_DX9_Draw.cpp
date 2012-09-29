#include "Stdafx.h"
#include "sdRenderPath_DX9.h"
#include "sdRenderDevice_DX9.h"
#include "sdRenderSystem.h"

//
#include <sdMatrix3.h>
#include <sdMatrix4.h>

//
#include "sdMap.h"
#include "sdLight.h"

//
using namespace Base::Math;

namespace RenderSystem
{
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::Draw()
{
	// 禁用渲染则直接返回
	if (!m_bEnableRendering)
		return;

	// 保存所有渲染状态
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);
	pkRenderDevice->SaveAllRenderStates();

	// 设置初始渲染状态
	PrepareRenderStates();

	// 设置变换矩阵(貌似这里最好延迟一下等待RenderTarget被设置完毕)
	pkRenderDevice->SetCameraData(m_spCurCam);

	// 设置着色器全局变量
	PrepareShaderConstants();

	// 渲染
	//@{
	// 预绘制深度到DepthStencilBuffer
	//DrawEarlyZPass();

	// 绘制地形Depth到特定纹理,并渲染到模板缓存
	DrawTerrainDepthPass();

	// 绘制物件到GeometryBuffer,并渲染到模板缓存(包括Building/Tree/Doodads/Terrain)
	DrawGeometryPasses();

	// 光照
	//DrawPreLightPass();

	// 最终着色合成
	DrawShadingPasses();

	// AO?

	// 全局雾
	//DrawFogPass();

	// 水下透明物件

	// 水

	// 水上透明物件

	// 后期特效
	//DrawPostProcessEffect();

	// 勾边之类的

	// 调试信息
	m_pkDebugGizmoPass->Draw();

	//@}

	// 恢复所有渲染状态
	pkRenderDevice->RestoreAllRenderStates();
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::PrepareRenderStates()
{
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// 光栅化模式(初始值)
	//pkRenderState->SetRenderState(D3DRS_FILLMODE,	D3DFILL_SOLID);

	// 雾
	// @{
	// 禁用硬件全局雾(全局值)
	// D3DRS_FOGENABLE
	// D3DRS_RANGEFOGENABLE
	// D3DRS_FOGCOLOR
	// D3DRS_FOGTABLEMODE
	// D3DRS_FOGSTART
	// D3DRS_FOGEND
	// D3DRS_FOGDENSITY
	pkRenderDevice->SetRenderState(D3DRS_FOGENABLE, false);
	// @}

	// 禁用硬件sRGB矫正(全局值)
	pkRenderDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, 0);

	// 深度
	// @{
	// 深度缓存(初始值)
	//pkRenderDevice->SetRenderState(D3DRS_ZENABLE,		true);
	//pkRenderDevice->SetRenderState(D3DRS_ZWRITEENABLE,	true);
	//pkRenderDevice->SetRenderState(D3DRS_ZFUNC,			D3DCMP_LESSEQUAL);

	// 深度偏置(初始值)
	// D3DRS_DEPTHBIAS
	// D3DRS_SLOPESCALEDEPTHBIAS
	//pkRenderDevice->SetRenderState(D3DRS_DEPTHBIAS,				0);
	//pkRenderDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,	0)
	// @}

	// 模版
	// @{
	// 模板测试
	// D3DRS_STENCILENABLE
	// D3DRS_STENCILFAIL
	// D3DRS_STENCILZFAIL
	// D3DRS_STENCILPASS
	// D3DRS_STENCILFUNC
	// D3DRS_STENCILREF
	// D3DRS_STENCILMASK
	// D3DRS_STENCILWRITEMASK
	//
	// D3DRS_TWOSIDEDSTENCILMODE
	// D3DRS_CCW_STENCILFAIL
	// D3DRS_CCW_STENCILZFAIL
	// D3DRS_CCW_STENCILPASS
	// D3DRS_CCW_STENCILFUNC
	//pkRenderDevice->SetRenderState(D3DRS_STENCILENABLE,		false);
	// @}

	// 透明
	// @{
	// 透明测试
	// D3DRS_ALPHATESTENABLE
	// D3DRS_ALPHAREF
	// D3DRS_ALPHAFUNC
	//pkRenderDevice->SetRenderState(D3DRS_ALPHATESTENABLE,	false);

	// 透明混合
	// D3DRS_ALPHABLENDENABLE
	// D3DRS_BLENDOP
	// D3DRS_SRCBLEND
	// D3DRS_DESTBLEND
	//
	// D3DRS_SEPARATEALPHABLENDENABLE
	// D3DRS_BLENDOPALPHA
	// D3DRS_SRCBLENDALPHA
	// D3DRS_DESTBLENDALPHA
	//pkRenderDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,	false);
	// @}

	// 颜色
	// @{
	// 允许颜色输出(初始值)
	// D3DRS_COLORWRITEENABLE
	// D3DRS_COLORWRITEENABLE1
	// D3DRS_COLORWRITEENABLE2
	// D3DRS_COLORWRITEENABLE3
	pkRenderDevice->SetRenderState(D3DRS_COLORWRITEENABLE,	D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |	D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
	// @}
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::PrepareShaderConstants()
{
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	uint uiWidth = pkRenderDevice->GetDefaultRenderTargetGroup()->GetWidth(0);
	uint uiHeight = pkRenderDevice->GetDefaultRenderTargetGroup()->GetHeight(0);

	// 相机
	// @{
	// 相机姿态
	NiPoint3 kCamDir = m_spCurCam->GetWorldDirection();
	NiPoint3 kCamUp	 = m_spCurCam->GetWorldUpVector();
	NiPoint3 kCamRight = m_spCurCam->GetWorldRightVector();
	kCamDir.Unitize();
	kCamUp.Unitize();
	kCamRight.Unitize();
	pkRenderDevice->SetGlobalShaderConstant("g_vCameraUp", sizeof(NiPoint3), &kCamUp);
	pkRenderDevice->SetGlobalShaderConstant("g_vCameraRight", sizeof(NiPoint3), &kCamRight);

	// 相机视锥体(视锥体左右是对称的)
	const NiFrustum& kCamFrustum = m_spCurCam->GetViewFrustum();
	NiPoint4 kFrustumData(kCamFrustum.m_fRight, kCamFrustum.m_fTop, kCamFrustum.m_fNear, kCamFrustum.m_fFar);
	pkRenderDevice->SetGlobalShaderConstant("g_fCameraFarClip", sizeof(kCamFrustum.m_fFar), &kCamFrustum.m_fFar);
	pkRenderDevice->SetGlobalShaderConstant("g_vCameraFrustum", sizeof(NiPoint4), &kFrustumData);
	
	// 观察矩阵
	const NiPoint3& kCamPos = m_spCurCam->GetWorldLocation();
	sdMatrix4 kViewMatrix(
		kCamRight.x, kCamUp.x, kCamDir.x, 0.0f,
		kCamRight.y, kCamUp.y, kCamDir.y, 0.0f,
		kCamRight.z, kCamUp.z, kCamDir.z, 0.0f,
		-kCamPos.Dot(kCamRight), -kCamPos.Dot(kCamUp), -kCamPos.Dot(kCamDir), 1.0f);

	// 深度逆变换矩阵
	NiPoint3 kRight, kUp, kDir;
	if (kCamFrustum.m_bOrtho)
	{
		kRight = kCamRight * kCamFrustum.m_fRight;
		kUp = kCamUp * kCamFrustum.m_fTop;
		kDir = kCamDir * kCamFrustum.m_fFar;
	}
	else
	{
		kRight = kCamRight * kCamFrustum.m_fRight * kCamFrustum.m_fFar;
		kUp = kCamUp * kCamFrustum.m_fTop * kCamFrustum.m_fFar;
		kDir = kCamDir * kCamFrustum.m_fFar;
	}

	sdMatrix4 kDepthToWorldMatrix(
		kRight.x,	kRight.y,	kRight.z,	0.0f,
		kUp.x,		kUp.y,		kUp.z,		0.0f,
		kDir.x,		kDir.y,		kDir.z,		0.0f,
		kCamPos.x,	kCamPos.y,	kCamPos.z,	1.0f);
	
	sdMatrix4 kDepthToViewMatrix = kDepthToWorldMatrix * kViewMatrix;

	pkRenderDevice->SetGlobalShaderConstant("g_mDepthToWorld", sizeof(kDepthToWorldMatrix), &kDepthToWorldMatrix);
	pkRenderDevice->SetGlobalShaderConstant("g_mDepthToView", sizeof(kDepthToViewMatrix), &kDepthToViewMatrix);
	// @}

	
	// 灯光
	// @{
	bool bMainLight		= m_kRenderParams.IsEnableChannel(sdRenderParams::E_BUILDING, sdRenderParams::E_MAINLIGHT);
	bool bAssistLight	= m_kRenderParams.IsEnableChannel(sdRenderParams::E_BUILDING, sdRenderParams::E_ASSISTLIGHT);
	bool bAmbientLight	= m_kRenderParams.IsEnableChannel(sdRenderParams::E_BUILDING, sdRenderParams::E_AMBIENTLIGHT);
	bool bLocalLight	= m_kRenderParams.IsEnableChannel(sdRenderParams::E_BUILDING, sdRenderParams::E_LOCALLIGHT);

	// 观察矩阵的旋转矩阵(D3D观察坐标系,向右为X,向上为Y,向前为Z)
	sdMatrix3 kViewRotationMatrix(
		kCamRight.x, kCamUp.x, kCamDir.x,
		kCamRight.y, kCamUp.y, kCamDir.y,
		kCamRight.z, kCamUp.z, kCamDir.z);

	// 主光
	if (bMainLight)
	{
		sdLight* pkMainLight = m_pkCurMap->GetMainLight();
		NIASSERT(pkMainLight);

		NiDirectionalLight* pkMainGBLight = (NiDirectionalLight*)pkMainLight->GetGBLight();
		NIASSERT(pkMainGBLight);

		const NiPoint3& kDir = pkMainGBLight->GetWorldDirection();
		//sdVector3 kMainLightDir(kDir.x, kDir.y, kDir.z);
		sdVector3 kMainLightDir(-1, 1, -1);
		sdVector3 kMainLightViewDir = kMainLightDir * kViewRotationMatrix;
		kMainLightViewDir.Normalise();
		pkRenderDevice->SetGlobalShaderConstant("g_vMainLightDir", sizeof(kMainLightViewDir), &kMainLightViewDir);
		
		NiColor kMainLightColor = pkMainGBLight->GetDiffuseColor() * pkMainGBLight->GetDimmer();
		NiColor kMainLightAmbient = pkMainGBLight->GetAmbientColor() * pkMainGBLight->GetDimmer();
		pkRenderDevice->SetGlobalShaderConstant("g_vMainLightColor", sizeof(kMainLightColor), &kMainLightColor);
		pkRenderDevice->SetGlobalShaderConstant("g_vMainLightAmbientColor", sizeof(kMainLightAmbient), &kMainLightAmbient);
	}

	// 辅助光
	if (bAssistLight)
	{
		sdLight* pkAssistLight = m_pkCurMap->GetAssistantLight();
		NIASSERT(pkAssistLight);

		NiDirectionalLight* pkAssistGBLight = (NiDirectionalLight*)pkAssistLight->GetGBLight();
		NIASSERT(pkAssistGBLight);

		const NiPoint3& kDir = pkAssistGBLight->GetWorldDirection();
		sdVector3 kAssistLightDir(kDir.x, kDir.y, kDir.z);
		sdVector3 kAssistLightViewDir = kAssistLightDir * kViewRotationMatrix;
		kAssistLightViewDir.Normalise();
		pkRenderDevice->SetGlobalShaderConstant("g_vAssistLightDir", sizeof(kAssistLightViewDir), &kAssistLightViewDir);
		
		NiColor kAssistLightColor = pkAssistGBLight->GetDiffuseColor() * pkAssistGBLight->GetDimmer();
		NiColor kAssistLightAmbient = pkAssistGBLight->GetAmbientColor() * pkAssistGBLight->GetDimmer();	
		pkRenderDevice->SetGlobalShaderConstant("g_vAssistLightColor", sizeof(kAssistLightColor), &kAssistLightColor);
		pkRenderDevice->SetGlobalShaderConstant("g_vAssistLightAmbientColor", sizeof(kAssistLightAmbient), &kAssistLightAmbient);
	}

	// 环境光
	if (bAmbientLight)
	{
		sdLight* pkAmbientLight = m_pkCurMap->GetAmbientLight();
		NIASSERT(pkAmbientLight);

		NiAmbientLight* pkAssistGBLight = (NiAmbientLight*)pkAmbientLight->GetGBLight();
		NIASSERT(pkAssistGBLight);

		NiColor kTerrainAmbientColor = pkAssistGBLight->GetDiffuseColor() * pkAssistGBLight->GetDimmer();
		NiColor kSkyAmbientColor = pkAssistGBLight->GetAmbientColor() * pkAssistGBLight->GetDimmer();
		pkRenderDevice->SetGlobalShaderConstant("g_vTerraimAmbientColor", sizeof(kTerrainAmbientColor), &kTerrainAmbientColor);
		pkRenderDevice->SetGlobalShaderConstant("g_vSkyAmbientColor", sizeof(kSkyAmbientColor), &kSkyAmbientColor);
	}

	//
	NiPoint4 kLightFactor(1.0f, 1.0f, 1.0f, 1.0f);
	pkRenderDevice->SetGlobalShaderConstant("g_vLightFactor", sizeof(kLightFactor), &kLightFactor);
	// }@

	// 地形参数
	// @{
	// 地形尺寸倒数
	float fRecipTerrainSize[2] = { 1.0f / m_kTerrainParams.terrainSize.m_kX, 1.0f / m_kTerrainParams.terrainSize.m_kY};
	pkRenderDevice->SetGlobalShaderConstant("g_vRecipTerrainSize", sizeof(fRecipTerrainSize), &(fRecipTerrainSize[0]));

	// 地形材质
	pkRenderDevice->SetGlobalShaderConstant("g_vTerrainDiffuseMaterial", sizeof(m_kTerrainParams.diffuseMaterial), &(m_kTerrainParams.diffuseMaterial.m_fX));
	pkRenderDevice->SetGlobalShaderConstant("g_vTerrainSpecularMaterial", sizeof(m_kTerrainParams.specularMaterial), &(m_kTerrainParams.specularMaterial.m_fX));
	pkRenderDevice->SetGlobalShaderConstant("g_fTerrainShiness", sizeof(m_kTerrainParams.shiness), &(m_kTerrainParams.shiness));
	
	// 地形DiffuseAtlas/NormalAtlas信息
	pkRenderDevice->SetGlobalShaderConstant("g_vDiffuseAtlasTableParam", sizeof(m_kTerrainParams.diffuseAtlasTableParam), &(m_kTerrainParams.diffuseAtlasTableParam));
	pkRenderDevice->SetGlobalShaderConstant("g_vNormalAtlasTableParam", sizeof(m_kTerrainParams.normalAtlasTableParam), &(m_kTerrainParams.normalAtlasTableParam));
	
	// 屏幕每个像素对应的世界空间中远裁剪面的尺寸(从双曲面近似到平面)
	sdVector2 kFarPlanePixelSize;
	if (kCamFrustum.m_bOrtho)
	{
		kFarPlanePixelSize.m_fX = (kCamFrustum.m_fRight - kCamFrustum.m_fLeft) / (float)uiWidth;
		kFarPlanePixelSize.m_fY = (kCamFrustum.m_fTop - kCamFrustum.m_fBottom) / (float)uiHeight;
	}
	else
	{
		kFarPlanePixelSize.m_fX = (kCamFrustum.m_fRight - kCamFrustum.m_fLeft) * kCamFrustum.m_fFar / (float)uiWidth;
		kFarPlanePixelSize.m_fY = (kCamFrustum.m_fTop - kCamFrustum.m_fBottom) * kCamFrustum.m_fFar / (float)uiHeight;
	}
	pkRenderDevice->SetGlobalShaderConstant("g_vFarPixelSize", sizeof(kFarPlanePixelSize), &kFarPlanePixelSize);
	// @}


	// 环境
	// @{


	// @}

	// 后期特效
	// @{


	// @}

	// @{
	//
	NiPoint4 kNormalScale(1.0f, 1.0f, 1.0f, 1.0f);
	pkRenderDevice->SetGlobalShaderConstant("g_vNormalScale", sizeof(kNormalScale), &kNormalScale);

	// 编辑器状态下的一些默认值
	sdVector3 kVertexColorMask = sdVector3::ZERO;
	sdVector3 kDiffuseMask = sdVector3::ZERO;
	float fGlossMapMask = 0.0f;
	float fLightMapMask = 0.0f;
	
	if (!m_kRenderParams.IsEnableChannel(sdRenderParams::E_BUILDING, sdRenderParams::E_DIFFUSEMAP))	kDiffuseMask = sdVector3::UNIT_SCALE;
	if (!m_kRenderParams.IsEnableChannel(sdRenderParams::E_BUILDING, sdRenderParams::E_GLOSSMAP))	fGlossMapMask = 1.0f;
	if (!m_kRenderParams.IsEnableChannel(sdRenderParams::E_BUILDING, sdRenderParams::E_LIGHTMAP))	fLightMapMask = 1.0f;

	pkRenderDevice->SetGlobalShaderConstant("g_vVertexColorMask", sizeof(kVertexColorMask), &kVertexColorMask);
	pkRenderDevice->SetGlobalShaderConstant("g_vDiffuseMapMask", sizeof(kDiffuseMask), &kDiffuseMask);
	pkRenderDevice->SetGlobalShaderConstant("g_fGlossMapMask", sizeof(fGlossMapMask), &fGlossMapMask);
	pkRenderDevice->SetGlobalShaderConstant("g_fLightMapMask", sizeof(fLightMapMask), &fLightMapMask);
	// @}
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::DrawEarlyZPass()
{
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// 设置目标缓存
	pkRenderDevice->SetRenderTargetGroup(m_spCurFinalRenderTarget);

	// 清空缓存(深度缓存和模板缓存)
	float	fFarDepth		= 1.0f;
	uint	uiBlackStencil	= 0x0;
	pkRenderDevice->Clear(NULL, &fFarDepth, &uiBlackStencil);

	// 绘制
	m_pkEarlyZPass->Draw();
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::DrawTerrainDepthPass()
{
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// 设置目标缓存(地形深度缓存)
	pkRenderDevice->SetRenderTargetGroup(m_spTerrainDepthTarget);

	// 清空缓存(颜色缓存,即地形深度缓存)
	uint	uiBlackColor	= 0x00000000;
	float	fFarDepth		= 1.0f;
	uint	uiBlackStencil	= 0x0;
	pkRenderDevice->Clear(&uiBlackColor, &fFarDepth, &uiBlackStencil);

	// 绘制TerrainMesh
	m_pkTerrainDepthPass->Draw();
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::DrawGeometryPasses()
{
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// 设置目标缓存(几何缓存)
	pkRenderDevice->SetRenderTargetGroup(m_spGeometryAlbedoMRT);

	// 清空缓存(颜色缓存)
	uint uiBlackColor = 0x00000000;
	pkRenderDevice->Clear(&uiBlackColor, NULL, NULL);

	// 绘制StaticMesh
	m_pkMRTZGeometryPass->Draw();
	
	// 绘制SkinnedMesh

	// 绘制Doodads

	// 绘制Terrain
	m_pkTerrainAtlasGeometryPass->Draw();
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::DrawPreLightPass()
{
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// 设置目标缓存(光照缓存)
	pkRenderDevice->SetRenderTargetGroup(m_spLightTarget);

	// 清空缓存(颜色缓存)
	uint uiBlackColor = 0x00000000;
	pkRenderDevice->Clear(&uiBlackColor, NULL, NULL);

	// 绘制
	//
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::DrawShadingPasses()
{
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// 设置目标缓存(几何缓存)
	pkRenderDevice->SetRenderTargetGroup(m_spCurFinalRenderTarget);

	// 清空缓存(颜色缓存)
	NiColor& kFogColor = m_kEnvironmentParams.fogColor;
	uint uiFogR = (uint)(kFogColor.r * 255);
	uint uiFogG = (uint)(kFogColor.g * 255);
	uint uiFogB = (uint)(kFogColor.b * 255);
	uint uiD3DFogColor = (uiFogR << 16) | (uiFogG << 8) | uiFogB;
	pkRenderDevice->Clear(&uiD3DFogColor, NULL, NULL);

	// 着色地形
	m_pkTerrainAtlasShadingPass->Draw();

	// 着色道路

	// 着色StaticMesh
	m_pkMRTShadingPass->Draw();

	// 着色SkinnedMesh

	// 着色Decal和DecalShadow

	// 着色地表Doodads

	// 着色天空盒

}
//-------------------------------------------------------------------------------------------------
}