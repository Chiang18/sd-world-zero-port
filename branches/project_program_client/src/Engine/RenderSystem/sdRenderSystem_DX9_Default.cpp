#include "Stdafx.h"
#include "sdRenderSystem_DX9.h"

// GB
#include <NiMeshScreenElements.h>

namespace RenderSystem
{
//-------------------------------------------------------------------------------------------------
NiMesh* sdRenderSystem_DX9::GetDefaultMesh(eDefaultMesh eMesh)
{
	if (eMesh < 0 || eMesh >= NUM_DEFAULT_MESHES)
	{
		NIASSERT(0);
		return NULL;
	}

	switch(eMesh)
	{
	case sdRenderSystem::E_SCREEN_QUAD:
		return m_spScreenQuadMesh;

	case sdRenderSystem::E_UNIT_CUBE:
		return m_spUnitCubeMesh;

	case sdRenderSystem::E_UNIT_CONE:
		return m_spUnitConeMesh;

	case sdRenderSystem::E_UNIT_PYRAMID:
		return m_spUnitPyramidMesh;

	case sdRenderSystem::E_UNIT_SPHERE:
		return m_spUnitSphereMesh;	
	}

	return m_spErrorMesh;
}
//-------------------------------------------------------------------------------------------------
NiMaterial*	sdRenderSystem_DX9::GetDefaultMaterial(eDefaultMaterial eMaterial)
{
	if (eMaterial < 0 || eMaterial >= NUM_DEFAULT_MATERIALS)
	{
		NIASSERT(0);
		return NULL;
	}

	return m_spDefaultMaterials[eMaterial];
}
//-------------------------------------------------------------------------------------------------
NiMesh*	sdRenderSystem_DX9::CreateScreenQuadMesh()
{
	NiMeshScreenElements* spScreenMesh = NiNew NiMeshScreenElements(false, false, 1);
	NIASSERT(spScreenMesh);

	spScreenMesh->Insert(4);
	spScreenMesh->SetRectangle(0, -1.0f, 1.0f, 2.0f, -2.0f);
	spScreenMesh->SetTextures(0, 0, 0.0f, 0.0f, 1.0f, 1.0f);
	spScreenMesh->Update(0.0f);
	spScreenMesh->UpdateEffects();
	spScreenMesh->UpdateProperties();

	return spScreenMesh;
}
//-------------------------------------------------------------------------------------------------
}