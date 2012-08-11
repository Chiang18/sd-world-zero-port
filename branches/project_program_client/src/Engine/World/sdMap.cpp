#include "stdafx.h"
#include "sdMap.h"

using namespace RenderSystem;
//-------------------------------------------------------------------------------------------------
bool sdMap::CreateScene()
{

	//*************************************************
	// ÁÙÊ±´úÂë
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



	// 
	m_kRenderParams.Reset();
	m_kRenderParams.EnableChannel(sdRenderParams::E_TERRAIN, sdRenderParams::E_NORMALMAP, false);


	return true;
}
//-------------------------------------------------------------------------------------------------