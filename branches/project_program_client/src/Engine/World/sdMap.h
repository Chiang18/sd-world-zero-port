//*************************************************************************************************
// ����:	������ͼ
//---------------------------------------------------------
// ����:		
// ����:		2012-06-29
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _MAP_H__
#define _MAP_H__
#include "sdGameObject.h"
#include "sdTerrain.h"

// ��Ⱦϵͳ
#include "sdRenderSystem.h"


// ������ͼ
class sdMap: public sdGameObject
{

public:

	// 
	bool	CreateScene();

	//*********************
	NiAVObjectPtr	m_pkRoot;
	//**********************

	//
	sdTerrain*	GetTerrain() { return &m_kTerrain;};

	const RenderSystem::sdRenderParams& GetRenderParams() const { return m_kRenderParams;};
	const RenderSystem::sdEnvironmentParams& GetEnvironmentParams() const { return m_kEnvironmentParams;};
	const RenderSystem::sdPostProcessParams& GetPostProcessParams() const { return m_kPostProcessParams;};

protected:

protected:
	

	// ����ϵͳ
	sdTerrain	m_kTerrain;

	// �������Ʋ���(�����ṩ����Ⱦϵͳʹ��,�ڲ���Ӧ�ñ������ڴ�ͱ�����Դ)
	RenderSystem::sdRenderParams		m_kRenderParams;
	RenderSystem::sdEnvironmentParams	m_kEnvironmentParams;
	RenderSystem::sdPostProcessParams	m_kPostProcessParams;
};
NiSmartPointer(sdMap);
#endif