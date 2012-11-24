//*************************************************************************************************
// 内容:	平面着色
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-10
// 最后修改:
//*************************************************************************************************
#include "../common.h"

//---------------------------------------------------------------------------------------
// 输入变量
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// 输入纹理采样器
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// 顶点着色器输入数据流
//---------------------------------------------------------------------------------------
struct VS_SKINNED_INPUT
{
	float3 	vPos			: POSITION;
	float3 	vBlendWeights 	: BLENDWEIGHT0;
	int4	iBlendIndices	: BLENDINDICES0;
};

//---------------------------------------------------------------------------------------
// 顶点着色器输出数据流
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// 顶点着色器
//---------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------
float4	VS_StaticMesh_TransformPosition(float3 vPos : POSITION)	: POSITION
{
	return mul(float4(vPos, 1.0f), g_mWorldViewProj);
}
//---------------------------------------------------------------------------------------
float4	VS_SkinnedMesh_TransformPosition(VS_SKINNED_INPUT In) : POSITION
{
	float4x4 mSkinnedBoneTransform;
	float4 vWorldPos;
	TransformSkinnedPosition(In.vPos.xyz, In.iBlendIndices, In.vBlendWeights, g_mSkinnedBoneMatrix3, 
		vWorldPos, mSkinnedBoneTransform);
	return vWorldPos;
}

//---------------------------------------------------------------------------------------
// 像素着色器
//---------------------------------------------------------------------------------------
float4 	PS_Main() : COLOR0
{
	return g_vFlatShadingColor;
}

//---------------------------------------------------------------------------------------
// 着色技术
//---------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------
// StaticMesh的FlatShading的处理
technique Misc_StaticMesh_FlatShading
<
	string	Description 		= "Misc_StaticMesh_FlatShading";
	bool	UsesNiRenderState 	= false;
	bool	UsesNiLightState	= false;
>
{
	pass P0
	{
		VertexShader	= compile 	vs_2_0	VS_StaticMesh_TransformPosition();
		PixelShader		= compile	ps_2_0 	PS_Main();
	}
}
//---------------------------------------------------------------------------------------
// SkinnedMesh的FlatShading的处理
technique Misc_SkinnedMesh_FlatShading
<
	string	Description 		= "Misc_SkinnedMesh_FlatShading";
	bool	UsesNiRenderState 	= false;
	bool	UsesNiLightState	= false;
	//bool 	BlendIndicesAsD3DColor = true;
>
{
	pass P0
	{
		VertexShader	= compile 	vs_2_a	VS_SkinnedMesh_TransformPosition();
		PixelShader		= compile	ps_2_a 	PS_Main();
	}
}
//---------------------------------------------------------------------------------------


