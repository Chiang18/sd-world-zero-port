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
		"E:\\project_game\\edata\\world0\\texture\\terrain\\ice_broken_ddn.dds", 10.0);
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
	m_kTerrain.RepackMaps();
	//*************************************************

	// 灯光
	// @{
	m_pkAmbientLight = NiNew sdLight(sdLight::E_LT_AMBIENT);
	NIASSERT(m_pkAmbientLight);
	m_pkAmbientLight->SetAmbientColor(NiColor::BLACK);
	m_pkAmbientLight->SetDiffuseColor(NiColor::BLACK);

	m_pkMainLight = NiNew sdLight(sdLight::E_LT_DIR);
	NIASSERT(m_pkMainLight);

	NiMatrix3 kRotation;
	kRotation.FromEulerAnglesXYZ(0, -NI_PI/4.0f, -NI_PI/4.0f);
	m_pkMainLight->SetRotate(kRotation);
	m_pkMainLight->SetAmbientColor(NiColor::BLACK);
	m_pkMainLight->SetDiffuseColor(NiColor::WHITE);
	m_pkMainLight->SetSpecularColor(NiColor::BLACK);
	m_pkMainLight->SetDimmer(1.0f);
	m_pkMainLight->Update(0.0f);

	m_pkAssistantLight = NiNew sdLight(sdLight::E_LT_DIR);
	NIASSERT(m_pkAssistantLight);
	m_pkAssistantLight->SetAmbientColor(NiColor::BLACK);
	m_pkAssistantLight->SetDiffuseColor(NiColor::BLACK);
	m_pkAssistantLight->SetSpecularColor(NiColor::BLACK);
	m_pkAssistantLight->SetDimmer(1.0f);
	m_pkAssistantLight->Update(0.0f);
	// @}

	// 更新Debug节点
	m_spDebugNode->Update(0.0f);



	// 
	m_kRenderParams.Reset();
	m_kRenderParams.EnableChannel(sdRenderParams::E_TERRAIN, sdRenderParams::E_NORMALMAP, false);


	return true;
}
//-------------------------------------------------------------------------------------------------
bool sdMap::AddEntity(sdEntity* pkEntity)
{
	if (NULL == pkEntity)
		return false;

	return true;
}
//-------------------------------------------------------------------------------------------------
bool sdMap::RemoveEntity(sdEntity* pkEntity)
{


	return true;
}
//-------------------------------------------------------------------------------------------------