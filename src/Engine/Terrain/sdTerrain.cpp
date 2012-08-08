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

	// 检查高度图尺寸
	// (62,128,256,512,1024,2048)

	// 初始化静态成员
	sdQuadMesh::StaticInitialize();

	// 创建高度图
	m_pkHeightMap = NiNew sdHeightMap(uiSize);
	NIASSERT(m_pkHeightMap);

	// 创建法线图
	m_pkNormalMap = NiNew sdNormalMap(uiSize, m_pkHeightMap);
	NIASSERT(m_pkNormalMap);
	m_pkNormalMap->Update();

	// 构建四叉树(依据高度图)
	m_pkQuadRoot = NiNew sdQuadNode;
	m_pkQuadRoot->Build(sdVector4ui(0, 0, uiSize, uiSize), NULL);

	// 初始化地形渲染信息
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