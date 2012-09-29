#include "Stdafx.h"
#include "sdTerrain.h"
#include "sdTerrainStream.h"

using namespace Base;
using namespace Base::Math;
using namespace RenderSystem;
//-------------------------------------------------------------------------------------------------
sdTerrain::sdTerrain()
: m_bInitialized(false)
, m_bIsVisible(true)
, m_bEnableLOD(true)
, m_fMeterPerGrid(1.0f)
, m_uiTileSize(64)
, m_uiTexTileSize(4)
, m_fMeterPerPixel(0.5f)
, m_uiMeshLevel(2)
, m_fScale(1.0f)
, m_kOrigin(NiPoint3::ZERO)
{

}
//-------------------------------------------------------------------------------------------------
sdTerrain::~sdTerrain()
{
	DestroyScene();
}
//-------------------------------------------------------------------------------------------------
bool sdTerrain::CreateScene(uint uiTerrainSize, uint uiBlendTexSize)
{
	using namespace Base::Math;

	if (m_bInitialized)
	{
		DestroyScene();
		m_bInitialized = false;
	}

	// 检查高度图尺寸,混合贴图尺寸
	//
	//
	m_uiTerrainSize		= uiTerrainSize;
	m_uiBlendTexSize	= uiBlendTexSize;
	m_uiTileMapSize		= uiBlendTexSize / m_uiTexTileSize;
	m_fMeterPerPixel	= uiTerrainSize / (float)uiBlendTexSize;
	
	// 初始化静态成员
	sdQuadMesh::StaticInitialize();

	// 创建高度图
	m_pkHeightMap = NiNew sdHeightMap(uiTerrainSize);
	NIASSERT(m_pkHeightMap);

	// 地形拾取
	m_pkPick = NiNew sdTerrainPick(m_pkHeightMap);
	NIASSERT(m_pkPick);

	// 创建法线贴图
	m_pkNormalMap = NiNew sdNormalMap(uiTerrainSize, m_pkHeightMap);
	NIASSERT(m_pkNormalMap);
	m_pkNormalMap->Update();

	if (m_bEnableEditMaterial)
	{
		// 创建混合贴图
		// @{
		m_pkDynamicBlendMap = NiNew sdDynamicTexture(uiBlendTexSize, uiBlendTexSize, sdDynamicTexture::E_FORMAT_A4R4G4B4);
		NIASSERT(m_pkDynamicBlendMap);

		sdDynamicTexture::sdLockedRect kLockedRect = m_pkDynamicBlendMap->LockRegion(0, 0, uiBlendTexSize, uiBlendTexSize);
		NIASSERT(kLockedRect.pBits);
		ZeroMemory(kLockedRect.pBits, kLockedRect.uiPitch * uiBlendTexSize);
		m_pkDynamicBlendMap->UnlockRegion();

		m_spBlendMap = m_pkDynamicBlendMap->GetGBTexture();
		NIASSERT(m_spBlendMap);
		// @}

		// 创建混合贴图瓦片贴图(用于保存每个瓦片内的四层混合贴图信息)
		// @{
		m_pkDynamicTileMap = NiNew sdDynamicTexture(m_uiTileMapSize, m_uiTileMapSize, sdDynamicTexture::E_FORMAT_A8R8G8B8);
		NIASSERT(m_pkDynamicTileMap);

		kLockedRect = m_pkDynamicTileMap->LockRegion(0, 0, m_uiTileMapSize, m_uiTileMapSize);
		NIASSERT(kLockedRect.pBits);
		ZeroMemory(kLockedRect.pBits, kLockedRect.uiPitch * m_uiTileMapSize);
		m_pkDynamicTileMap->UnlockRegion();

		m_spTileMap = m_pkDynamicTileMap->GetGBTexture();
		NIASSERT(m_spTileMap);
		// @}

		// 创建拼接器
		// @{
		m_pkDiffuseAtlas = NiNew sdTextureAtlas(D3DFMT_DXT5);
		NIASSERT(m_pkDiffuseAtlas);

		m_pkNormalAtlas = NiNew sdTextureAtlas(D3DFMT_DXT5);
		NIASSERT(m_pkNormalAtlas);
		// @}

		// 初始化Tile混合过滤器
		m_kTileFilterMapVec.resize(m_uiTileSize * m_uiTileSize);
		float fOffset = 0.5f;
		float fScale = m_uiTileSize / (m_uiTileSize - 1.0f);
		for (uint uiY = 0; uiY < m_uiTileSize; ++uiY)
		{
			float fPosY = uiY * fScale + fOffset;
			uint uiTop = (uint)floor(fPosY);
			uint uiBottom = uiTop + 1;
			float fFracY = fPosY - uiTop;

			for (uint uiX = 0; uiX < m_uiTileSize; ++uiX)
			{
				float fPosX = uiX * fScale + fOffset;
				uint uiLeft = (uint)floor(fPosX);
				uint uiRight = uiLeft + 1;
				float fFracX = fPosX - uiTop;

				stFilterMap& kMap = m_kTileFilterMapVec[uiY * m_uiTileSize + uiX];
				kMap.uiIdx[0] = uiTop * (m_uiTileSize + 2) + uiLeft;	
				kMap.uiIdx[1] = uiTop * (m_uiTileSize + 2) + uiLeft + 1;	
				kMap.uiIdx[2] = (uiTop + 1) * (m_uiTileSize + 2) + uiLeft;
				kMap.uiIdx[3] = (uiTop + 1) * (m_uiTileSize + 2) + uiLeft + 1;

				kMap.fWeight[0] = (1.0f - fFracX) * (1.0f - fFracY);
				kMap.fWeight[1] = fFracX * (1.0f - fFracY);
				kMap.fWeight[2] = (1.0f - fFracX) * fFracY;
				kMap.fWeight[3] = fFracX * fFracY;
			}
		}
	}

	// 构建四叉树(依据高度图)
	m_pkQuadRoot = NiNew sdQuadNode;
	m_pkQuadRoot->Build(sdVector4ui(0, 0, uiTerrainSize, uiTerrainSize), NULL);

	// 初始化地形渲染信息
	// @{
	m_kRenderParams.Reset();

	m_kRenderParams.terrainSize = sdVector2ui(m_uiTerrainSize, m_uiTerrainSize);

	m_kRenderParams.ambientMaterial	 = sdVector3(0, 0, 0);
	m_kRenderParams.diffuseMaterial	 = sdVector3(1.0f, 1.0f, 1.0f);
	m_kRenderParams.specularMaterial = sdVector3(0, 0, 0);
	m_kRenderParams.shiness			 = 0.0f;

	m_kRenderParams.baseNormalMap = m_pkNormalMap->GetGBTexture();
	m_kRenderParams.tileMap		= m_spTileMap;
	m_kRenderParams.blendMap	= m_spBlendMap;	
	// @}

	return (m_bInitialized = true);
}
//-------------------------------------------------------------------------------------------------
bool sdTerrain::SaveScene(const std::string& szSceneFullPath)
{
	if (!m_bInitialized)
		return false;

	// 保存
	sdTerrainStream::SaveHeightMap(m_pkHeightMap, szSceneFullPath.c_str());
	sdTerrainStream::SaveBlendMap(m_spBlendMap, szSceneFullPath.c_str());
	sdTerrainStream::SaveDiffuseAtlasMap(m_spDiffuseAtlasMap, szSceneFullPath.c_str());
	sdTerrainStream::SaveDiffuseAtlasTableMap(m_spDiffuseAtlasTable, szSceneFullPath.c_str());
	sdTerrainStream::SaveNormalAtlasMap(m_spNormalAtlasMap, szSceneFullPath.c_str());
	sdTerrainStream::SaveNormalAtlasTableMap(m_spNormalAtlasTable, szSceneFullPath.c_str());

	return true;
}
//-------------------------------------------------------------------------------------------------
void sdTerrain::DestroyScene()
{
	if (m_bInitialized)
	{
		m_pkHeightMap = 0;

		m_pkQuadRoot->Destroy();
		m_pkQuadRoot = 0;

		sdQuadMesh::StaticDestroy();

		m_bInitialized = false;
	}
}
//-------------------------------------------------------------------------------------------------
void sdTerrain::UpdateGeometry(float fCenterX, float fCenterY, float fRadius)
{
	// 更行GeometryMesh
	m_pkQuadRoot->UpdateGeometry(fCenterX, fCenterY, fRadius);

	// 更新NormalMap
	// @{
	// 计算范围(世界坐标)
	float fStartX	= fCenterX - fRadius;
	float fStartY	= fCenterY - fRadius;
	float fEndX	= fCenterX + fRadius;
	float fEndY = fCenterY + fRadius;

	// 计算范围(高度图/法线图坐标)
	float fTotalScale = m_fScale * m_fMeterPerGrid;
	float fNStartX = (fStartX - m_kOrigin.x) / fTotalScale;
	float fNStartY = (fStartY - m_kOrigin.y) / fTotalScale;
	float fNEndX = (fEndX - m_kOrigin.x) / fTotalScale;
	float fNEndY = (fEndY - m_kOrigin.y) / fTotalScale;

	// 钳位到范围内整数
	if (fNStartX < 0)	fNStartX = 0.0f;
	if (fNStartY < 0)	fNStartY = 0.0f;
	if (fNEndX > (float)m_uiTerrainSize)	fNEndX = (float)m_uiTerrainSize;
	if (fNEndY > (float)m_uiTerrainSize)	fNEndY = (float)m_uiTerrainSize;

	uint uiNStartX = (uint)fNStartX;
	uint uiNStartY = (uint)fNStartY;
	uint uiNEndX = (uint)(fNEndX);
	uint uiNEndY = (uint)(fNEndY);

	// 更新法线图
	m_pkNormalMap->Update(uiNStartX, uiNStartY, uiNEndX - uiNStartX, uiNEndY - uiNStartY);
	// @}
}
//-------------------------------------------------------------------------------------------------
void sdTerrain::Cull(const NiCamera& kCamera, std::vector<NiMesh*>& kMeshVec)
{
	NiFrustumPlanes kFrustumPlanes;
	kFrustumPlanes.Set(kCamera.GetViewFrustum(), kCamera.GetWorldTransform());
	kFrustumPlanes.EnableAllPlanes();
	
	m_pkQuadRoot->Cull(kCamera, kFrustumPlanes, kMeshVec);
}
//-------------------------------------------------------------------------------------------------
float sdTerrain::GetRawHeight(uint uiX, uint uiY)
{
	return m_pkHeightMap->GetRawHeight(uiX, uiY);
}
//-------------------------------------------------------------------------------------------------
void sdTerrain::SetRawHeight(uint uiX, uint uiY, float fHeight)
{
	m_pkHeightMap->SetRawHeight(uiX, uiY, fHeight);
}
//-------------------------------------------------------------------------------------------------
float sdTerrain::GetHeight(float fX, float fY)
{
	// 转换到HeightMap坐标系(不进行参数检查)
	float fTotalScale = m_fScale * m_fMeterPerGrid;
	float fHX = (fX - m_kOrigin.x) / fTotalScale;
	float fHY = (fY - m_kOrigin.y) / fTotalScale;

	return m_pkHeightMap->GetHeight(fHX, fHY) * fTotalScale + m_kOrigin.z;
}
//-------------------------------------------------------------------------------------------------
bool sdTerrain::Pick(const sdRay& kRay, sdVector3& kIntersect, float fLimit)
{
	return m_pkPick->Pick(kRay, kIntersect);
}
//-------------------------------------------------------------------------------------------------