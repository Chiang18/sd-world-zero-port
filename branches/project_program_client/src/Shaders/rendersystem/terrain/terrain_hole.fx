//*************************************************************************************************
// 内容:	地形Voxel(未完成)
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-24
// 最后修改:
//*************************************************************************************************
#include "terrain_common.h"

//---------------------------------------------------------------------------------------
// 全局变量
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// 输入纹理采样器
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// 顶点着色器
//---------------------------------------------------------------------------------------
float4 VS_Main(float3 vPos : POSITION0) : POSITION
{
	return mul(float4(vPos, 1.f), g_mWorldViewProj);
}

//---------------------------------------------------------------------------------------
// 像素着色器
//---------------------------------------------------------------------------------------
float4 PS_Main() : COLOR0
{
	return float4(1.f, 0.f, 0.f, 0.f);
}

//---------------------------------------------------------------------------------------
// 着色技术
//---------------------------------------------------------------------------------------
technique Terrain_Hole
<
	string Description = "Terrain_Hole";
	bool UsesNiRenderState = false;
	bool UsesNiLightState = false;
>
{
	pass P0
	{
		VertexShader 	= compile vs_2_0 VS_Main ();
		PixelShader 	= compile ps_2_0 PS_Main ();
	}
}
//---------------------------------------------------------------------------------------