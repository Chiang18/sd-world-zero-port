//*************************************************************************************************
// ����:	StaticMesh��EarlyZPass��ɫ��
//---------------------------------------------------------
// ����:		
// ����:		2012-07-10
// ����޸�:
//*************************************************************************************************
#include "static_mesh_common.h"

//@{
// StaticMesh��SolidMesh�Ĵ���
//-----------------------------------------------------------------------------
float4	VS_StaticMesh_Solid(float3 vPos : POSITION)	: POSITION
{
	return mul(float4(vPos, 1.0f), g_mWorldViewProj);
}
//-----------------------------------------------------------------------------
float4 	PS_StaticMesh_Solid() : COLOR0
{
	return 0;
}
//-----------------------------------------------------------------------------
technique StaticMesh_Solid_EarlyZ
<
	string	Description 		= "StaticMesh_Solid_EarlyZ";
	bool	UsesNiRenderState 	= false;
	bool	UsesNiLightState	= false;
>
{
	pass P0
	{
		VertexShader	= compile 	vs_2_a 	VS_StaticMesh_Solid();
		PixelShader		= compile	ps_2_a 	PS_StaticMesh_Solid();
	}
}
//-----------------------------------------------------------------------------
//@}