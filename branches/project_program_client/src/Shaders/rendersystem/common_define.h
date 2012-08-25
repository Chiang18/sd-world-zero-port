//*************************************************************************************************
// ����:	������ɫ���Ĺ�������/�궨��/����
//			������Ԥ�����������
//---------------------------------------------------------
// ����:		
// ����:		2012-07-10
// ����޸�:
//*************************************************************************************************
#ifndef _COMMON_DEFINE_H__
#define _COMMON_DEFINE_H__

//*****************************************************************************
// ���Բ���
// float4 	g_DebugParam			: GLOBAL;

//-----------------------------------------------------------------------------
// �任����(GB�ڽ�)
float4x4	g_mWorld				: WORLD;
float4x4	g_mInvWorld				: INVWORLD;

float4x4	g_mView					: VIEW;
float4x4	g_mInvView				: INVVIEW;
float4x4	g_mInvViewT				: VIEWIT;

float4x4	g_mProj					: PROJ;
float4x4	g_mInvProj				: INVPROJ;

float4x4	g_mViewProj				: VIEWPROJECTION;
float4x4	g_mInvViewProj			: INVVIEWPROJ;

float4x4	g_mWorldView			: WORLDVIEW;
float4x4	g_mInvWorldView			: INVWORLDVIEW;

float4x4	g_mWorldViewProj		: WORLDVIEWPROJ;
float4x4	g_mInvWorldViewProj		: INVWORLDVIEWPROJ;

//-----------------------------------------------------------------------------
// Ӳ������
#define BONEMATRIX_TYPE	float4x3
static const int MAX_BONES = 32;

BONEMATRIX_TYPE	g_mSkinnedBoneMatrix3[MAX_BONES] : SKINBONEMATRIX3;


//-----------------------------------------------------------------------------
// ���
float4 		g_vCameraPostion		: EYEPOS;	// �����������	
float4		g_vCameraUp				: GLOBAL;	// ���������̬
float4		g_vCameraRight			: GLOBAL;

float4		g_vCameraFrustum		: GLOBAL;	// �����׶��(���,���,��,Զ)
float		g_fCameraFarClip		: GLOBAL;	// ���Զ�ü���

float4x4	g_mDepthToWorld			: GLOBAL;
#define		g_vViewPos				g_mDepthToWorld._m30_m31_m32

float4x4	g_mDepthToView			: GLOBAL;

//-----------------------------------------------------------------------------
// ����
float2		g_vHalfPixelOffset		: GLOBAL;	// ������ƫ��
float2		g_vPixToTexOffset		: GLOBAL;	// ͶӰ���굽��Ļ���������ƫ��(ò�Ʋ��Ǻܱ�Ҫ)
float2		g_vWindowSize			: GLOBAL;	// ��Ļ�ߴ�

//-----------------------------------------------------------------------------
// ʱ��
float		g_fTime					: GLOBAL;

//-----------------------------------------------------------------------------
// 
float4		g_vWireframeColor		: GLOBAL = float4(1.0f, 1.0f, 1.0f, 1.0f);
float4		g_vSolidColor			: GLOBAL = float4(0.75f, 0.75f, 0.75f, 1.0f);
float4 		g_vFloatShadingColor 	: GLOBAL = float4(1.0f, 0, 0, 1.0f);

//*****************************************************************************
// ��������(����/ȫ����)
//*****************************************************************************
// ������
float3		g_vTerraimAmbientColor		: GLOBAL;
float3		g_vSkyAmbientColor			: GLOBAL;

// �������
float3		g_vMainLightDir				: GLOBAL;
float3		g_vMainLightColor			: GLOBAL;
float3 		g_vMainLightAmbientColor	: GLOBAL;

// ���������
float3		g_vAssistLightDir			: GLOBAL;
float3		g_vAssistLightColor			: GLOBAL;
float3		g_vAssistLightAmbientColor	: GLOBAL;

// �ڵ�����
float4		g_vLightFactor				: GLOBAL;
#define		g_fAmbientLightAOFactor		g_vLightFactor.x
#define		g_fMainLightAOFactor		g_vLightFactor.y
#define		g_fAssistLightAOFactor		g_vLightFactor.z
#define		g_fMainLightLightMapFactor	g_vLightFactor.w

// ��֪����ʲô
//float 		a_fMainLightOcclusion		: ATTRIBUTE = 1.0f;

// rimlight���Ʋ���(��ɫ�����ƹ�?)
// [fresnel-eta][fresnel-power][brightness]
// [color.r][color.g][color.b]
// [dir.x][dir.y][dir.z]
float3x3	a_mRimLightParam			: ATTRIBUTE;
#define 	a_fRimLightEta				mRimLightParam[0][0]
#define 	a_fRimLightPower			mRimLightParam[0][1]	
#define 	a_fRimBrightness			mRimLightParam[0][2]
#define 	a_vRimLightColor			float3(mRimLightParam[1][0], mRimLightParam[1][1], mRimLightParam[1][2])	
#define 	a_vRimLightDirection		float3(mRimLightParam[2][0], mRimLightParam[2][1], mRimLightParam[2][2])

//
#define		g_vLuminScale				float3(0.299, 0.587, 0.114)
#define		g_fLocalLightRange			4.0f;							// ���صƹⷶΧ[0, 4.0f]
#define		g_fLocalLightScale			(1.0f / g_fLocalLightRange)	
#define		g_fFogCameraFar				2500.0f

//*****************************************************************************
// ������Ч
//*****************************************************************************
// Gamma
float		g_fGammaValue				: GLOBAL;
float		g_fFinalGammaValue			: GLOBAL;

// HDR
bool		g_bUseToneMapping			: GLOBAL;

// DoF
float2		g_vDoF_akSampleOffsets[9]	: GLOBAL;
float		g_vDoF_akSampleWeights[9]	: GLOBAL;
float3		g_vDoFStartEnd				: GLOBAL;
#define		g_vDoFStartZ				g_vDoFStartEnd.z

// ȫ����
float4		g_vFogColorDensity			: GLOBAL;
float4		g_vFogStartEnd				: GLOBAL;
float4		g_vFogDensityParams			: GLOBAL;
float		g_fFogColorFactor			: GLOBAL;
#define		g_vFogColor					g_vFogColorDensity.rgb
#define		g_fFogDensity				g_vFogColorDensity.a
#define		g_fFogDistanceScale			g_vFogColorDensity.a
//#define	g_fFogStartZ				g_vFogStartEnd.z		// ľ�б�ʹ��
//#define	g_fFogEndZ					g_vFogStartEnd.w		// ľ�б�ʹ��
#define		g_fFogStartDistance			g_vFogStartEnd.x		// ȫ�����ƽ��
#define		g_fFogEndDistance			g_vFogStartEnd.y		// ȫ����Զƽ��
#define		g_fFogHeightStartEnd		g_vFogStartEnd.zw		// ȫ����߶�����
#define		g_fFogExtinctionDistance	5000.0f
#define		g_fFogSkyDensity			g_vFogDensityParams.x

float4		g_vSkyFogColorDensity		: GLOBAL;
float2		g_vSkyFogTopBottom			: GLOBAL;
#define		g_vSkyFogColor				g_vFogColorDensity.rgb
#define		g_fSkyFogDensity			g_vFogColorDensity.a

//*****************************************************************************
// ����
//*****************************************************************************
float4 		g_vAmbientMaterial			: GLOBAL = float4(1.0f, 1.0f, 1.0f, 1.0f);

float4 		g_vDiffuseMaterial			: GLOBAL = float4(1.0f, 1.0f, 1.0f, 1.0f);
#define		g_vDiffuseMaterialColor		g_vDiffuseMaterial.rgb
#define		g_fLocalLightRadio			g_vDiffuseMaterial.a
#define		g_fAlpha					g_vDiffuseMaterial.a

float4 		g_vSpecularMaterial			: GLOBAL = float4(1.0f, 1.0f, 1.0f, 1.0f);
#define		g_vSpecularMaterialColor	g_vSpecularMaterial.rgb
#define 	g_fShiness					g_vSpecularMaterial.a

float4 		g_vEmissiveMaterial			: GLOBAL = float4(0.0f, 0.0f, 0.0f, 0.0f);

float 		g_fAlphaTestRef				: GLOBAL;
float 		g_fEdgeEnhancement			: GLOBAL;


//*****************************************************************************
// �������
//*****************************************************************************
float4		g_vNormalScale				: GLOBAL;
#define		g_fStaticMeshNormalScale	(g_vNormalScale.x)
#define 	g_fLeafcardNormalScale		(g_vNormalScale.y)
#define 	g_fTerrainNormalScale		(g_vNormalScale.z)
#define 	g_fRoadNormalScale			(g_vNormalScale.w)

// 
float4		g_vDiffuseMapSize			: GLOBAL;
float3		g_vVertexColorMask			: GLOBAL;
float3		g_vDiffuseMapMask			: GLOBAL;
float		g_fLightMapMask				: GLOBAL;
float		g_fGlossMapMask				: GLOBAL;

// ����任
float4x4	g_mBaseMapTextureTransform		: TexTransformBase;
float4x4	g_mDarkMapTextureTransform		: TexTransformDark;
float4x4	g_mDetailMapTextureTransform	: TexTransformDetail;
float4x4	g_mGlowMapTextureTransform		: TexTransformGlow;

//*****************************************************************************
// ��Ⱦͨ��
//*****************************************************************************
// ���ڱ�ǵ�ǰ��ͼͨ���Ƿ�ʹ��(ʹShader�����ڶ��ֲ������)
#define DIFFUSEMAP_CHANNEL		0x01
#define NORMALMAP_CHANNEL		0x02
#define GLOSSMAP_CHANNEL		0x04
#define LIGHTMAP_CHANNEL		0x08
#define MONO_CHANNEL			0x08	// ��ɫͨ��
#define AOMAP_CHANNEL			0x10
#define FILTERMAP_CHANNEL		0x10
#define GLOWMAP_CHANNEL			0x20
#define ENVMAP_CHANNEL			0x80	// GB��BumpMapͨ��

// 
//#define	FINALPROCESS_DOF			0x01
//#define	FINALPROCESS_CURVED_DOF	0x02
//#define	FINALPROCESS_GODRAY		0x04
//#define	FINALPROCESS_COLOR_GRADING 0x08
//
//#define	COPY_RED_ALERT		0x01
//#define	COPY_DEAD_EFFECT		0x02

// ����MRT��Ⱦ
#define DETAIL_NORMAL_CHANNEL	0x10	
#define ALPHA_TEST_FLAG			0x20
#define VERTEX_COLOR_CHANNEL	0x40
#define RIMLIGHT_CHANNEL		0x08
#define SINGLEBONE_CHANNEL		0x10	// �򵥹����任

// ����Effect��Ⱦ
//#define	EFFECT_DARKMAP			0x01
//#define	EFFECT_DETAILMAP		0x02
//#define	EFFECT_BASEUVANI		0x04
//#define	EFFECT_ALPHAADD			0x08
//#define	EFFECT_HAEDWARESKIN		0x10
//#define	EFFECT_VERTEXCOLOR		0x20

// ���ڸ���flag��̬���ɱ�����������֮���
#define HEADER_KEY(header, flag)	header##flag

//
#define LINK_MACRO(a,b)	a##b

// ���������ַ���
#define MAKE_STRING(a)	#a
 
// �����ж�flag��ָ��λ�Ƿ�����
#define TEST_CHANNEL(flag, channel)	(((flag / channel) / 2) != ((flag / channel + 1) / 2))
//*****************************************************************************
#endif