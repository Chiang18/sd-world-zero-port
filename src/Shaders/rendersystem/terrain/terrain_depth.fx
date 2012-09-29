//*************************************************************************************************
// 内容:	地形Mesh的深度预绘制
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-14
// 最后修改:
//*************************************************************************************************
#include "terrain_common.h"

//---------------------------------------------------------------------------------------
// 顶点着色器输出数据流
//---------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4	vProjPos		: POSITION;
	float	fDepth			: TEXCOORD0;
};

//---------------------------------------------------------------------------------------
// 顶点着色器
//---------------------------------------------------------------------------------------
VS_OUTPUT VS_Main(float2 vPos : POSITION0)
{
	VS_OUTPUT kOutput;

	// 提取压缩的XY
	//	ix * 129 + iy
	float2 iXY = floor(float2(vPos.x / 129.0, fmod(vPos.x, 129.0) + 0.01));
	
	// 变换到世界坐标
	// 	floor(fH * 20.0f + 0.5f)
	float4 vWorldPosition = float4(iXY * a_fQuadScale + a_vQuadOrigin, vPos.y * 0.05, 1.0);
	
	// 变换到观察坐标
	kOutput.vProjPos = mul(vWorldPosition, g_mViewProj);
	
	// 计算深度
	kOutput.fDepth = dot(vWorldPosition, g_mView._m02_m12_m22_m32);
	
	return kOutput;
}

//---------------------------------------------------------------------------------------
// 像素着色器
//---------------------------------------------------------------------------------------
float4	PS_Main(VS_OUTPUT kInput) : COLOR0
{
	// 归一化到[0,1]
	return float4(PackDepth(kInput.fDepth / g_fCameraFarClip), 0.0f, 0.0f);
	
	//*************************
	// 测试线性深度
	//float fDepth = kInput.fDepth / g_fCameraFarClip;
	//return float4(fDepth, fDepth, fDepth, 0);
	//*************************	
}

//---------------------------------------------------------------------------------------
// 着色技术
//---------------------------------------------------------------------------------------
technique Terrain_Depth
<
	string Description = "Terrain_Depth";
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