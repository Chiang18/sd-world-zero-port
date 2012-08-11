#include "Stdafx.h"
#include "sdTerrain.h"

using namespace RenderSystem;
//-------------------------------------------------------------------------------------------------
sdTerrain::sdTerrain()
: m_bInitialized(false)
, m_bIsVisible(true)
, m_bEnableLOD(true)
, m_uiMeterPerUnit(1)
, m_uiTexTileSize(4)
, m_uiTileSize(64)
, m_uiMeshLevel(2)
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
	m_uiBlendTexSize	= uiBlendTexSize * 2;

	// 初始化静态成员
	sdQuadMesh::StaticInitialize();

	// 创建高度图
	m_pkHeightMap = NiNew sdHeightMap(uiTerrainSize);
	NIASSERT(m_pkHeightMap);

	// 创建法线贴图
	// @{
	m_pkNormalMap = NiNew sdNormalMap(uiTerrainSize, m_pkHeightMap);
	NIASSERT(m_pkNormalMap);
	m_pkNormalMap->Update();
	// @}

	// 创建混合贴图
	// @{
	m_spDynamicBlendMap = NiNew sdDynamicTexture(uiBlendTexSize, uiBlendTexSize, sdDynamicTexture::E_FORMAT_A4R4G4B4);
	NIASSERT(m_spDynamicBlendMap);

	sdDynamicTexture::sdLockedRect kLockedRect = m_spDynamicBlendMap->LockRegion(0, 0, uiBlendTexSize, uiBlendTexSize);
	NIASSERT(kLockedRect.pBits);
	memset(kLockedRect.pBits, 0, kLockedRect.uiPitch * uiBlendTexSize);
	m_spDynamicBlendMap->UnlockRegion();

	m_spBlendMap = m_spDynamicBlendMap->GetGBTexture();
	NIASSERT(m_spBlendMap);
	// @}

	// 创建混合贴图瓦片贴图(用于保存每个瓦片内的四层混合贴图信息)
	// @{
	uint	uiTileMapSize = m_uiBlendTexSize / m_uiTexTileSize;
	m_spDynamicTileMap = NiNew sdDynamicTexture(uiTileMapSize, uiTileMapSize, sdDynamicTexture::E_FORMAT_A4R4G4B4);
	NIASSERT(m_spDynamicTileMap);

	kLockedRect = m_spDynamicTileMap->LockRegion(0, 0, uiTileMapSize, uiTileMapSize);
	NIASSERT(kLockedRect.pBits);
	memset(kLockedRect.pBits, 0, kLockedRect.uiPitch * uiTileMapSize);
	m_spDynamicTileMap->UnlockRegion();

	m_spTileMap = m_spDynamicTileMap->GetGBTexture();
	NIASSERT(m_spTileMap);
	// @}

	// 构建四叉树(依据高度图)
	m_pkQuadRoot = NiNew sdQuadNode;
	m_pkQuadRoot->Build(sdVector4ui(0, 0, uiTerrainSize, uiTerrainSize), NULL);

	// 初始化地形渲染信息
	m_kRenderParams.Reset();
	m_kRenderParams.baseNormalMap = m_pkNormalMap->GetGBTexture();
	m_kRenderParams.spBlendMap	= m_spBlendMap;	
	m_kRenderParams.spTileMap	= m_spTileMap;

	return (m_bInitialized = true);
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
void sdTerrain::Cull(const NiCamera& kCamera, std::vector<NiMesh*>& kMeshVec)
{
	if (HasScene())
	{
		NiFrustumPlanes kFrustumPlanes;
		kFrustumPlanes.Set(kCamera.GetViewFrustum(), kCamera.GetWorldTransform());
		kFrustumPlanes.EnableAllPlanes();
		
		m_pkQuadRoot->Cull(kCamera, kFrustumPlanes, kMeshVec);
	}
}
//-------------------------------------------------------------------------------------------------