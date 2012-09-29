//*************************************************************************************************
// ����:	������ɫ���Ĺ�������/�궨��/����
//			�����Ǻ�������
//---------------------------------------------------------
// ����:		
// ����:		2012-07-10
// ����޸�:
//*************************************************************************************************
#ifndef _COMMON_FUNCTION_H__
#define _COMMON_FUNCTION_H__

//*****************************************************************************
// ���ѹ��
//*****************************************************************************
float2 PackDepth(float depth)
{
	float depth_int, depth_frac;
	depth_frac = modf(sqrt(depth) * 256.0f, depth_int);
	return float2(depth_int / 255.0f, depth_frac);
}
//-----------------------------------------------------------------------------
float UnpackDepth(float2 enc)
{
	float depth = dot(enc.xy, float2(255.0f, 1.0f)) * (1.0f / 256.0f);
	depth = depth * depth;
	return depth;
}

//*****************************************************************************
// ����ѹ��
// �ο�
//*****************************************************************************
//-----------------------------------------------------------------------------
// Spheremap Transform, WorldZero����
//-----------------------------------------------------------------------------
float2 PackNormal(float3 normal)
{
	float f = sqrt(dot(float2(-normal.z, 1.0f), float2(8.0f, 8.0f)));
	return normal.xy / f + 0.5f;
}
//-----------------------------------------------------------------------------
float3 UnpackNormal(float2 enc)
{
	float2	fenc 	= enc * 4.0f - 2.0f;
	float	f		= dot(fenc, fenc);
	float 	g 		= sqrt(1.0f - f / 4.0f);
	float3	normal	= float3(fenc * g, f * 0.5f - 1.0f);
	return normal;
}
//-----------------------------------------------------------------------------
// Spheremap Transform, Crytek����
//-----------------------------------------------------------------------------
float2 PackNormal_Crytek(float3 normal)
{
   return normalize(normal.xy) * sqrt(normal.z * 0.5 + 0.5);
}
//-----------------------------------------------------------------------------
float3 UnpackNormal_Crytek(float2 enc)
{
   float3 normal;
   normal.z = dot(enc, enc) * 2 - 1;
   normal.xy = normalize(enc) * sqrt(1 - normal.z * normal.z);
   return normal;
}

//*****************************************************************************
// ��ɫ�ռ�
//*****************************************************************************
float4 ToLinear(float4 color)
{
	return float4(pow(color.rgb, g_fGammaValue), color.a);
}

float3 ToLinear(float3 color)
{
	return float3(pow(color.rgb, g_fGammaValue));
}

float4 ToGamma(float4 color)
{
	return float4(pow(color.rgb, 1.0f / g_fFinalGammaValue), color.a);
}

//*****************************************************************************
// �����任
//*****************************************************************************
void TransformSkinnedPosition(float3 vPosition, 
							int4 iBlendIndices, 
							float3 vBlendWeights, 
							BONEMATRIX_TYPE mBones[MAX_BONES],
							out float4 vWorldPos,
							out float4x4 mSkinnedBoneTransform)
{
	// ����任����
	float fWeight3 = 1.0f - vBlendWeights[0] - vBlendWeights[1] - vBlendWeights[2];
	BONEMATRIX_TYPE mShortSkinnedBoneTransform;
	mShortSkinnedBoneTransform 	= mBones[iBlendIndices[0] * vBlendWeights[0]];
	mShortSkinnedBoneTransform += mBones[iBlendIndices[1] * vBlendWeights[1]];
	mShortSkinnedBoneTransform += mBones[iBlendIndices[2] * vBlendWeights[2]];
	mShortSkinnedBoneTransform += mBones[iBlendIndices[3] * fWeight3];
	mSkinnedBoneTransform = float4x4(
		mShortSkinnedBoneTransform[0], 0.0f,
		mShortSkinnedBoneTransform[1], 0.0f,
		mShortSkinnedBoneTransform[2], 0.0f,
		mShortSkinnedBoneTransform[3], 1.0f);
		
	// �任
	vWorldPos.xyz = mul(float4(vPosition, 1.0f), mShortSkinnedBoneTransform);
	vWorldPos.w = 1.0f;
}


//*****************************************************************************
// ���ռ���
//*****************************************************************************
void AccumLighting(float3 V, float3 N, float fShiness, float fLightMap, float4 vLocalLight, out float3 vDiffuseLight, out float3 vSpecularLight)
{
	// 
	fLightMap = 1;//saturate(1.0f + g_fMainLightLightMapFactor * (fLightMap - 1.0f));
	
	// ���㲢�ϳ�ȫ���������(��ֵļ����㷨)
	// @{
	float3 L = -g_vMainLightDir;
	float3 R = reflect(g_vMainLightDir, N);
	
	float NL = saturate(dot(N, L));
	float VR = saturate(dot(V, R));
	
	vDiffuseLight 	= lerp(g_vMainLightAmbientColor, g_vMainLightColor, NL * fLightMap);
	vSpecularLight	= vDiffuseLight * pow(VR, fShiness);
	// @}
	
	// ���㲢�ϳ�ȫ�ָ��������(��ֵļ����㷨)
	// @{
	L = -g_vAssistLightDir;
	R = reflect(g_vAssistLightDir, N);
	
	NL = saturate(dot(N, L));
	VR = saturate(dot(V, R));
	
	vDiffuseLight 	+= lerp(g_vAssistLightAmbientColor, g_vAssistLightColor, NL);
	vSpecularLight	+= vDiffuseLight * pow(VR, fShiness);
	// @}

	
	// �ϳɾֲ�����(��ֵĺϳ��㷨)
	// @{
	vLocalLight *= g_fLocalLightRange;
	
	vDiffuseLight	+= vLocalLight.rgb;
	vSpecularLight	+= vLocalLight.rgb / (dot(vLocalLight.rgb, g_vLuminScale) + 0.001f) * vLocalLight.a;
	// @}
	
	
	// �ϳ�ȫ�ֻ�������(�ѷ���ת��������ռ�,Ȼ���ֵ��������ջ�����)
	vDiffuseLight	+= lerp(g_vTerraimAmbientColor, g_vSkyAmbientColor, saturate(dot(N, g_mInvViewT._m20_m21_m22)));
}
//*****************************************************************************
#endif