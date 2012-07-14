#include "stdafx.h"
#include "sdRenderObject.h"
#include "sdRenderDevice_DX9.h"

namespace RenderSystem
{

//*************************************************************************************************
sdRenderObject::sdRenderObject(eRenderObjectType eType, NiMesh* spMesh, NiMaterial* spMaterial, 
		NiPropertyList* spRenderProps, ShaderParamItem* pkShaderParams)
: m_eType(eType)
, m_spMesh(spMesh)
, m_spMaterial(spMaterial)
, m_spProperties(spRenderProps)
, m_pkShaderParams(pkShaderParams)
, m_spOldMaterial(NULL)
, m_spOldTexturingProp(NULL)
{
	// 生成SortKey
	//
	// Material 与 VertexBuffer
	NIASSERT(spMesh);
	IDirect3DVertexBuffer9*	spVertexBuffer = NULL;
	NiDataStreamRef* spVertexStreamRef = spMesh->FindStreamRef(NiCommonSemantics::POSITION());
	if (spVertexStreamRef)
	{
		NiDataStream* spStream = spVertexStreamRef->GetDataStream();
		NiDX9DataStream* spDX9Stream = NiVerifyStaticCast(NiDX9DataStream, spStream);
		spVertexBuffer = spDX9Stream->GetVertexBuffer();
	}
	m_kRenderKey.key0 = (((uint)m_spMaterial & 0xffff) << 16) | ((uint)spVertexBuffer & 0xffff);

	// BaseTexture 与 NormalTexture 与 DrawMode
	NIASSERT(spMesh->GetPropertyState());
	NiPropertyState* spPropertyState = spMesh->GetPropertyState();
	NiTexturingProperty* spTexturingProp = spPropertyState->GetTexturing();
	if (spTexturingProp)
	{
		NiTexture* spBaseTexture = spTexturingProp->GetBaseTexture();
		NiTexture* spNormalTexture = spTexturingProp->GetNormalMap() ?
			spTexturingProp->GetNormalMap()->GetTexture() : NULL;

		m_kRenderKey.key0 = (((uint)spBaseTexture & 0xffff) << 16) | ((uint)spNormalTexture & 0xffff);
	}

	NiStencilProperty* spStencilProp = spPropertyState->GetStencil();
	if (spStencilProp)
	{
		m_kRenderKey.key1 = (uint)spStencilProp->GetDrawMode() + 1;
	}

}
//-------------------------------------------------------------------------------------------------
void sdRenderObject::ApplyRenderSetting()
{
	ApplyRenderProps();
	ApplyMaterial();
	SetShaderParams();
	SetRenderStates();
}
//-------------------------------------------------------------------------------------------------
void sdRenderObject::RestoreRenderSetting()
{
	RestoreMaterial();
	RestoreRenderProps();
}
//-------------------------------------------------------------------------------------------------
void sdRenderObject::ApplyRenderProps()
{
	if (m_spProperties)
	{
		ApplyTexturingProp();
	}
}
//-------------------------------------------------------------------------------------------------
void sdRenderObject::RestoreRenderProps()
{
	if (m_spProperties)
	{
		RestoreTexturingProp();
	}
}
//-------------------------------------------------------------------------------------------------
void sdRenderObject::ApplyTexturingProp()
{
	NiPropertyState* spPropertyState = m_spMesh->GetPropertyState();
	NiTexturingProperty* spMeshTexturingProp = spPropertyState->GetTexturing();
	if (!spMeshTexturingProp)	return;

	NiProperty* spProp = FindProperty(m_spProperties, NiProperty::TEXTURING);
	NiTexturingProperty* spTexturingProp = (NiTexturingProperty*)spProp;
	if (!spTexturingProp)	return;

	// 拷贝数组里面凉
	const NiTexturingProperty::NiMapArray& kMeshMaps = spMeshTexturingProp->GetMaps();
	const NiTexturingProperty::NiMapArray& kMaps = spTexturingProp->GetMaps();

	//************

}
//-------------------------------------------------------------------------------------------------
void sdRenderObject::RestoreTexturingProp()
{
	
}
//-------------------------------------------------------------------------------------------------
void sdRenderObject::ApplyMaterial()
{
	if (m_spMaterial)
	{
		m_spOldMaterial = (NiMaterial*)m_spMesh->GetActiveMaterial();
		m_spMesh->ApplyAndSetActiveMaterial(m_spMaterial);
	}
}
//-------------------------------------------------------------------------------------------------
void sdRenderObject::RestoreMaterial()
{
	if (m_spMaterial)
	{
		m_spMesh->ApplyAndSetActiveMaterial(m_spOldMaterial);
		m_spOldMaterial = NULL;
	}
}
//-------------------------------------------------------------------------------------------------
void sdRenderObject::SetShaderParams()
{
	if (m_pkShaderParams)
	{
		sdRenderDevice* pkRenderDevice = (sdRenderDevice*)sdRenderDevice_DX9::InstancePtr();
		for (uint i = 0; NULL != m_pkShaderParams[i].szName; ++i)
		{
			pkRenderDevice->SetGlobalShaderConstant(
				*m_pkShaderParams[i].szName,
				m_pkShaderParams[i].uiByteSize,
				m_pkShaderParams[i].pData);
		}
	}
}
//-------------------------------------------------------------------------------------------------
void sdRenderObject::SetRenderStates()
{
	if (m_spMaterial)
	{
		sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
		NiPropertyState* spPropertyState = m_spMesh->GetPropertyState();
		NiStencilProperty*	spStencilProp = spPropertyState->GetStencil();
		if (spStencilProp)
		{
			// 此处参考GB的NiStencilProperty
			static D3DCULL d3dCullTable[] = 
			{
				D3DCULL_CW,		// 对应GB的DRAW_CCW_OR_BOTH
				D3DCULL_CW,		// 对应GB的DRAW_CCW
				D3DCULL_CCW,	// 对应GB的DRAW_CW
				D3DCULL_NONE,	// 对应GB的DRAW_BOTH
			};
			NiStencilProperty::DrawMode eDrawMode = spStencilProp->GetDrawMode();
			pkRenderDevice->SetRenderState(D3DRS_CULLMODE, d3dCullTable[(uint)eDrawMode]);
		}
		else
		{
			pkRenderDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		}
	}
}
//-------------------------------------------------------------------------------------------------
NiProperty* sdRenderObject::FindProperty(NiPropertyList* spPropList, int iPropType)
{
	NIASSERT(spPropList);

	if (spPropList->IsEmpty())	return NULL;

	// 查找
	NiTListIterator itr =spPropList->GetHeadPos();
	while (itr)
	{
		NiProperty* pProp = spPropList->GetNext(itr);
		if (iPropType == pProp->Type())
			return pProp;
	}

	return NULL;
}
//*************************************************************************************************
StaticMeshRenderObject::StaticMeshRenderObject(NiMesh* spMesh, NiMaterial* spMaterial, 
	NiPropertyList* spRenderProps, ShaderParamItem* pkShaderParams)
: sdRenderObject(E_STATIC_MESH, spMesh, spMaterial, spRenderProps, pkShaderParams)
{

}
//-------------------------------------------------------------------------------------------------
void StaticMeshRenderObject::Draw()
{
	ApplyRenderSetting();

	//
	NiRenderer* spRenderer = NiRenderer::GetRenderer();
	m_spMesh->RenderImmediate(spRenderer);

	RestoreRenderSetting();
}
//*************************************************************************************************
}