//*************************************************************************************************
// 内容:	默认材质(平面着色)
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-10
// 最后修改:
//*************************************************************************************************
#include "common.h"

//-----------------------------------------------------------------------------
struct VS_SKINNED_INPUT
{
	float3 	vPos			: POSITION;
	float3 	vBlendWeights 	: BLENDWEIGHT0;
	int4	iBlendIndices	: BLENDINDICES0;
}
//-----------------------------------------------------------------------------
float4	VS_StaticMesh_TransformPosition(float3 vPos : POSITION)	: POSITION
{
	return mul(float4(vPos, 1.0f), g_mWorldViewProj);
}
//-----------------------------------------------------------------------------
float4	VS_SkinnedMesh_TransformPosition(VS_SKINNED_INPUT In) : POSITION
{
	float4x4 mSkinnedBoneTransform
	float4 vWorldPos;
	TransformSkinnedPosition(In.vPos.xyz, In.iBlendIndices, In.vBlendWeights, g_mSkinnedBoneMatrix3, 
		vWorldPos, mSkinnedBoneTransform);
	return vWorldPos;
}
//-----------------------------------------------------------------------------
float4 	PS_FlatShading() : COLOR0
{
	return g_vFloatShadingColor;
}
//-----------------------------------------------------------------------------
// StaticMesh的FlatShading的处理
technique StaticMesh_FlatShading
<
	string	Description 		= "StaticMesh_FlatShading";
	bool	UsesNiRenderState 	= false;
	bool	UsesNiLightState	= false;
>
{
	pass P0
	{
		VertexShader	= compile 	vs_2_0	VS_StaticMesh_TransformPosition();
		PixelShader		= compile	ps_2_0 	PS_FlatShading();
	}
}
//-----------------------------------------------------------------------------
// SkinnedMesh的FlatShading的处理
technique SkinnedMesh_FlatShading
<
	string	Description 		= "SkinnedMesh_FlatShading";
	bool	UsesNiRenderState 	= false;
	bool	UsesNiLightState	= false;
	//bool 	BlendIndicesAsD3DColor = true;
>
{
	pass P0
	{
		VertexShader	= compile 	vs_2_a	VS_SkinnedMesh_TransformPosition();
		PixelShader		= compile	ps_2_a 	PS_FlatShading();
	}
}
//-----------------------------------------------------------------------------


