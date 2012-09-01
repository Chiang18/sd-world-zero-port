#include "stdafx.h"
#include "sdMap.h"

using namespace RenderSystem;
//-------------------------------------------------------------------------------------------------
sdMap::sdMap()
{
	// 
	// @{
	m_spDebugNode = NiNew NiNode;
	NIASSERT(m_spDebugNode);
	m_spDebugNode->SetName("DebugNode");
	// @}
}
//-------------------------------------------------------------------------------------------------
sdMap::~sdMap()
{
	m_spDebugNode = 0;
}
//-------------------------------------------------------------------------------------------------
bool sdMap::CreateScene()
{

	//*************************************************
	// 临时代码
	NiStream kStream;
	kStream.Load("E:\\project_game\\dependence\\engine\\TheClockAndTheOcularis.nif");
	NiAVObjectPtr pAVObject = (NiAVObject*)kStream.GetObjectAt(0);
	//pAVObject->SetScale(0.001f);
	pAVObject->Update(0.0f);
	pAVObject->UpdateProperties();
	pAVObject->UpdateEffects();

	m_pkRoot = pAVObject;

	//
	m_kTerrain.CreateScene(512,1024);
	//*************************************************

	// 更新一次节点
	m_spDebugNode->Update(0.0f);



	// 
	m_kRenderParams.Reset();
	m_kRenderParams.EnableChannel(sdRenderParams::E_TERRAIN, sdRenderParams::E_NORMALMAP, false);


	return true;
}
//-------------------------------------------------------------------------------------------------