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
	//NiStream kStream;
	//kStream.Load("E:\\project_game\\dependence\\engine\\TheClockAndTheOcularis.nif");
	//NiAVObjectPtr pAVObject = (NiAVObject*)kStream.GetObjectAt(0);
	////pAVObject->SetScale(0.001f);
	//pAVObject->Update(0.0f);
	//pAVObject->UpdateProperties();
	//pAVObject->UpdateEffects();

	//m_pkRoot = pAVObject;

	//
	m_kTerrain.CreateScene(512,1024);

	sdLayerMap* pkLayerMap = NiNew sdLayerMap("Default", 
		"E:\\project_game\\edata\\world0\\texture\\terrain\\ice_broken.dds",
		"E:\\project_game\\edata\\world0\\texture\\terrain\\ice_broken_ddn.dds");
	m_kTerrain.AppendLayer(pkLayerMap);

	pkLayerMap = NiNew sdLayerMap("Default1", 
		"E:\\project_game\\edata\\world0\\texture\\terrain\\road_soil_small_pepples.dds",
		"E:\\project_game\\edata\\world0\\texture\\terrain\\road_soil_small_pepples_ddn.dds");
	m_kTerrain.AppendLayer(pkLayerMap);

	pkLayerMap = NiNew sdLayerMap("Default2", 
		"E:\\project_game\\edata\\world0\\texture\\terrain\\plains_ferns_green.dds",
		"E:\\project_game\\edata\\world0\\texture\\terrain\\plains_ferns_green_ddn.dds");
	m_kTerrain.AppendLayer(pkLayerMap);

	pkLayerMap = NiNew sdLayerMap("Default3", 
		"E:\\project_game\\edata\\world0\\texture\\terrain\\plains_grass_green_and_dry.dds",
		"E:\\project_game\\edata\\world0\\texture\\terrain\\plains_grass_green_and_dry_ddn.dds");
	m_kTerrain.AppendLayer(pkLayerMap);

	pkLayerMap = NiNew sdLayerMap("Default4", 
		"E:\\project_game\\edata\\world0\\texture\\terrain\\cliff_is2.dds",
		"E:\\project_game\\edata\\world0\\texture\\terrain\\cliff_is2_ddn.dds");
	m_kTerrain.AppendLayer(pkLayerMap);

	pkLayerMap = NiNew sdLayerMap("Default5", 
		"E:\\project_game\\edata\\world0\\texture\\terrain\\cliff_more_pieces.dds",
		"E:\\project_game\\edata\\world0\\texture\\terrain\\cliff_more_pieces_ddn.dds");
	m_kTerrain.AppendLayer(pkLayerMap);
	
	m_kTerrain.UpdateBlendMap(0, 0, 10000);
	//*************************************************

	// 更新一次节点
	m_spDebugNode->Update(0.0f);



	// 
	m_kRenderParams.Reset();
	m_kRenderParams.EnableChannel(sdRenderParams::E_TERRAIN, sdRenderParams::E_NORMALMAP, false);


	return true;
}
//-------------------------------------------------------------------------------------------------