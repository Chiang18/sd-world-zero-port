#include "Stdafx.h"
#include "sdRenderPath_DX9.h"

// World部分
#include "sdMap.h"

namespace RenderSystem
{
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::Render()
{
	// RenderShadow();
	// RenderReflection();
	RenderMainView();
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::RenderMainView()
{
	//sdPF_Func_Auto;

	// 裁剪Map得到待渲染Entity数组

	// 裁剪Entity数组得到待渲染Mesh,构建RenderObject队列,填充到RenderPath


	//********************************************************
	using namespace std;
	vector<NiMesh*> kMeshVec;
	CollectMeshes((NiNode*)(NiAVObject*)m_pkCurMap->m_pkRoot, kMeshVec);
	
	vector<NiMesh*>::iterator itr = kMeshVec.begin();
	vector<NiMesh*>::iterator itr_end = kMeshVec.end();
	for (; itr!=itr_end; ++itr)
	{
		RenderStaticMesh(*itr);
	}
	//********************************************************

	// 裁剪地形
	RenderTerrain();
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::OctreeCull()
{

}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::CollectMeshes(NiNode* spNode, std::vector<NiMesh*>& kMeshVec)
{
	NIASSERT(spNode);

	if (spNode->GetAppCulled())
		return;

	using namespace std;
	stack<NiNode*> kNodeStack;
	kNodeStack.push(spNode);
	while(!kNodeStack.empty())
	{
		NiNode* spCurNode = kNodeStack.top();
		kNodeStack.pop();

		int iCount = spCurNode->GetChildCount();
		for (int i = 0; i < iCount; ++i)
		{
			NiAVObject* spAVObject = spCurNode->GetAt(i);
			if (spAVObject && !spAVObject->GetAppCulled())
			{
				if (spAVObject->IsNode())
				{
					kNodeStack.push((NiNode*)spAVObject);
				}
				else if (NiIsKindOf(NiMesh, spAVObject))
				{
					// wz原文用的IsMesh,他们修改了GB源码,类似IsNode
					kMeshVec.push_back((NiMesh*)spAVObject);
				}
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::RenderTerrain()
{
	using namespace std;

	// 确信Camera/Map/Terrain有效性
	if (!m_spCurCam || !m_pkCurMap || !m_pkCurMap->GetTerrain())
		return;

	sdTerrain* pkTerrain = m_pkCurMap->GetTerrain();
	NIASSERT(pkTerrain);

	// 裁剪
	vector<NiMesh*> kMeshVec;
	pkTerrain->Cull(*m_spCurCam, kMeshVec);

	// 添加到渲染路径
	vector<NiMesh*>::iterator itr = kMeshVec.begin();
	vector<NiMesh*>::iterator itr_end = kMeshVec.end();
	for (; itr != itr_end; ++itr)
	{
		RenderTerrainMesh(*itr);
	}
}
//-------------------------------------------------------------------------------------------------
}
