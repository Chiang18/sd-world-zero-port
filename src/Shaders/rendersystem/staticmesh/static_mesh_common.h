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
float4	a_vLightMapChannel			: ATTRIBUTE	= float4(1.f, 0.f, 0.f,	0.f);
float4 	a_vLightMapTransform		: ATTRIBUTE	= float4(0.f, 0.f, 1.f, 1.f);

// DetailNormalMap的缩放与采样
float3	a_vDetailNormalScale		: ATTRIBUTE	= float3(1.f, 1.f, 1.f);
float2	a_vDetailNormalUVTiling		: ATTRIBUTE = float2(5.f, 5.f);
float	a_fDetailNormalFadeDistance	: ATTRIBUTE = 30.f;

//
float4x4	a_mWorldProjectionMat	: ATTRIBUTE;

// 延迟着色的纹理采样器
SD_POINT_CLAMP_SAMPLE(0,	sdLightBufferSampler, 		sdTexLightBuffer, 	false);	
SD_POINT_CLAMP_SAMPLE(1, 	sdGeomBufferSampler, 		sdTexGeomBuffer, 	false);
SD_POINT_CLAMP_SAMPLE(2, 	sdMatBufferSampler, 		sdTexMatBuffer, 	false);	// Old MRT
SD_LINEAR_CLAMP_SAMPLE(3, 	sdCurveFogBufferSampler, 	sdTexFogCurveMap, 	false);	

SD_POINT_CLAMP_SAMPLE(2, 	sdMatBuffer0Sampler, 		sdTexMatBuffer0, 	false);	// MRT3
SD_POINT_CLAMP_SAMPLE(3, 	sdMatBuffer1Sampler, 		sdTexMatBuffer1, 	false);	// MRT3
SD_POINT_CLAMP_SAMPLE(4, 	sdMatBuffer2Sampler, 		sdTexMatBuffer2, 	false);	// MRT3

#endif