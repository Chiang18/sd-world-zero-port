#include "Stdafx.h"
#include "sdTerrain.h"

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
	m_uiBlendTexSize	= uiBlendTexSize;
	m_uiTileMapSize		= uiBlendTexSize / m_uiTexTileSize;
	m_fMeterPerPixel	= uiTerrainSize / (float)uiBlendTexSize;
	

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

	if (m_bEnableEditMaterial)
	{
		// ���������ͼ
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

		// ���������ͼ��Ƭ��ͼ(���ڱ���ÿ����Ƭ�ڵ��Ĳ�����ͼ��Ϣ)
		// @{
		m_pkDynamicTileMap = NiNew sdDynamicTexture(m_uiTileMapSize, m_uiTileMapSize, sdDynamicTexture::E_FORMAT_A4R4G4B4);
		NIASSERT(m_pkDynamicTileMap);

		kLockedRect = m_pkDynamicTileMap->LockRegion(0, 0, m_uiTileMapSize, m_uiTileMapSize);
		NIASSERT(kLockedRect.pBits);
		ZeroMemory(kLockedRect.pBits, kLockedRect.uiPitch * m_uiTileMapSize);
		m_pkDynamicTileMap->UnlockRegion();

		m_spTileMap = m_pkDynamicTileMap->GetGBTexture();
		NIASSERT(m_spTileMap);
		// @}

		// ����ƴ����
		// @{
		m_pkDiffuseAtlas = NiNew sdTextureAtlas(D3DFMT_DXT5);
		NIASSERT(m_pkDiffuseAtlas);

		m_pkNormalAtlas = NiNew sdTextureAtlas(D3DFMT_DXT5);
		NIASSERT(m_pkNormalAtlas);
		// @}

		// ��ʼ��Tile��Ϲ�����
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

	// �����Ĳ���(���ݸ߶�ͼ)
	m_pkQuadRoot = NiNew sdQuadNode;
	m_pkQuadRoot->Build(sdVector4ui(0, 0, uiTerrainSize, uiTerrainSize), NULL);

	// ��ʼ��������Ⱦ��Ϣ
	m_kRenderParams.Reset();
	m_kRenderParams.baseNormalMap = m_pkNormalMap->GetGBTexture();
	m_kRenderParams.spTileMap	= m_spTileMap;
	m_kRenderParams.spBlendMap	= m_spBlendMap;	

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