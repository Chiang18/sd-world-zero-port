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

	// ���߶�ͼ�ߴ�,�����ͼ�ߴ�
	//
	//
	m_uiTerrainSize		= uiTerrainSize;
	m_uiBlendTexSize	= uiBlendTexSize * 2;

	// ��ʼ����̬��Ա
	sdQuadMesh::StaticInitialize();

	// �����߶�ͼ
	m_pkHeightMap = NiNew sdHeightMap(uiTerrainSize);
	NIASSERT(m_pkHeightMap);

	// ����������ͼ
	// @{
	m_pkNormalMap = NiNew sdNormalMap(uiTerrainSize, m_pkHeightMap);
	NIASSERT(m_pkNormalMap);
	m_pkNormalMap->Update();
	// @}

	// ���������ͼ
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

	// ���������ͼ��Ƭ��ͼ(���ڱ���ÿ����Ƭ�ڵ��Ĳ�����ͼ��Ϣ)
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

	// �����Ĳ���(���ݸ߶�ͼ)
	m_pkQuadRoot = NiNew sdQuadNode;
	m_pkQuadRoot->Build(sdVector4ui(0, 0, uiTerrainSize, uiTerrainSize), NULL);

	// ��ʼ��������Ⱦ��Ϣ
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