#include "Stdafx.h"
#include "sdTerrain.h"


//-------------------------------------------------------------------------------------------------
sdTerrain::sdTerrain()
: m_bInitialized(false)
, m_bIsVisible(true)
, m_bEnableLOD(true)
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
bool sdTerrain::CreateScene(uint uiSize)
{
	using namespace Base::Math;

	if (m_bInitialized)
	{
		DestroyScene();
		m_bInitialized = false;
	}

	// ���߶�ͼ�ߴ�
	// (62,128,256,512,1024,2048)

	// ��ʼ����̬��Ա
	sdQuadMesh::StaticInitialize();

	// �����߶�ͼ
	m_pkHeightMap = NiNew sdHeightMap(uiSize);
	NIASSERT(m_pkHeightMap);

	// ��������ͼ
	m_pkNormalMap = NiNew sdNormalMap(uiSize, m_pkHeightMap);
	NIASSERT(m_pkNormalMap);
	m_pkNormalMap->Update();

	// �����Ĳ���(���ݸ߶�ͼ)
	m_pkQuadRoot = NiNew sdQuadNode;
	m_pkQuadRoot->Build(sdVector4ui(0, 0, uiSize, uiSize), NULL);

	// ��ʼ��������Ⱦ��Ϣ
	m_kRenderParams.Reset();
	m_kRenderParams.baseNormalMap = m_pkNormalMap->GetGBTexture();

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