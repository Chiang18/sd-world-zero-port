#include "Stdafx.h"
#include "sdRenderPath_DX9.h"

// World����
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

	// �ü�Map�õ�����ȾEntity����

	// �ü�Entity����õ�����ȾMesh,����RenderObject����,��䵽RenderPath


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

	// �ü�����
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
					// wzԭ���õ�IsMesh,�����޸���GBԴ��,����IsNode
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

	// ȷ��Camera/Map/Terrain��Ч��
	if (!m_spCurCam || !m_pkCurMap || !m_pkCurMap->GetTerrain())
		return;

	sdTerrain* pkTerrain = m_pkCurMap->GetTerrain();
	NIASSERT(pkTerrain);

	// �ü�
	vector<NiMesh*> kMeshVec;
	pkTerrain->Cull(*m_spCurCam, kMeshVec);

	// ��ӵ���Ⱦ·��
	vector<NiMesh*>::iterator itr = kMeshVec.begin();
	vector<NiMesh*>::iterator itr_end = kMeshVec.end();
	for (; itr != itr_end; ++itr)
	{
		RenderTerrainMesh(*itr);
	}
}
//-------------------------------------------------------------------------------------------------
}
