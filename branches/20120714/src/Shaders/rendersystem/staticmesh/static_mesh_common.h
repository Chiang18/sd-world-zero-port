//*************************************************************************************************
// 内容:	StaticMesh的着色器的特有的公共变量与函数
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-10
// 最后修改:
//*************************************************************************************************
#ifndef _STATICMESH_COMMON_H__
#define _STATICMESH_COMMON_H__

//
#include "../common.h"

// LightMap
float4	vLightMapChannel	: ATTRIBUTE	= float4(1.0f, 	0,	0,	0);
float4 	vLightMapTransform	: ATTRIBUTE	= float4(0, 	0,	1, 	1);

// DetailNormalMap的缩放与采样
float3	vDetailNormalScale			: ATTRIBUTE	= float3(1.0f, 1.0f, 1.0f);
float2	vDetailNormalUVTiling		: ATTRIBUTE = float2(5.0f, 5.0f);
float	fDetailNormalFadeDistance	: ATTRIBUTE = 30.0f;



#endif