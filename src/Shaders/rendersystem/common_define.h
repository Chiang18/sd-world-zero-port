//*************************************************************************************************
// 内容:	所有着色器的公共变量/宏定义/函数
//			这里是预定义变量部分
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-10
// 最后修改:
//*************************************************************************************************
#ifndef _COMMON_DEFINE_H__
#define _COMMON_DEFINE_H__

//*****************************************************************************
// float4 	g_DebugParam			: GLOBAL;

// 变换
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

//
float4x4	g_mDepthToWorldPos		: GLOBAL;
#define		g_vViewPos				g_mDepthToWorldPos._m30_m32_m32

float4x4	g_mDepthToViewPos		: GLOBAL;

//-----------------------------------------------------------------------------
// 相机
float4 		g_vCameraPostion		: EYEPOS;
float4		g_vCameraFrustum		: GLOBAL;
float4		g_vCameraUp				: GLOBAL;
float4		g_vCameraRight			: GLOBAL;
float		g_fCameraFarClip		: GLOBAL;

//-----------------------------------------------------------------------------
// 窗口
float2		g_vHalfPixelOffset		: GLOBAL;
float2		g_vPixToTexOffset		: GLOBAL;
float2		g_vWindowSize			: GLOBAL;

//
float		g_fTime					: GLOBAL;

//-----------------------------------------------------------------------------
//
//#define		g_vLuminScale			float3(0.299, 0.587, 0.114)
//#define 		g_fLocalLightRange		4.0f;
//#define 		g_fLocalLightScale		(1.0f / g_fLocalLightRange)	

// 
float4		g_vWireframeColor		: GLOBAL = float4(1.0f, 1.0f, 1.0f, 1.0f);
float4		g_vSolidColor			: GLOBAL = float4(0.75f, 0.75f, 0.75f, 1.0f);

//*****************************************************************************
// 环境变量(光照/全局雾)
//*****************************************************************************
float4 g_vAmbientMaterial	: GLOBAL = float4(1.0f, 1.0f, 1.0f, 1.0f);

float 	g_fMainLightOcclusion	: GLOBAL = 1.0f;

//*****************************************************************************
// 材质
//*****************************************************************************
float4 	g_vDiffuseMaterial	: GLOBAL;
#define g_vDiffuseMaterialColor		g_vDiffuseMaterial.rgb
#define g_fLocalLightRadio			g_vDiffuseMaterial.a
#define g_fAlpha					g_vDiffuseMaterial.a

float4 	g_vSpeculatMaterial	: GLOBAL;
#define g_vSpeculatMaterialColor	g_vSpeculatMaterial.rgb
#define g_fShiness					g_vSpeculatMaterial.a

float4 	g_vEmissiveMaterial	: GLOBAL = float4(1.0f, 1.0f, 1.0f, 1.0f);

float 	g_fAlphaTestRef		: GLOBAL;
float 	g_fEdgeEnhancement	: GLOBAL;

//*****************************************************************************
// 纹理
//*****************************************************************************
float4	g_vNormalScale		: GLOBAL;
#define	g_fStaticMeshNormalScale	(g_vNormalScale.x)
#define g_fLeafcardNormalScale		(g_vNormalScale.y)
#define g_fTerrainNormalScale		(g_vNormalScale.z)
#define g_fRoadNormalScale			(g_vNormalScale.w)


//*****************************************************************************
// 渲染通道
//*****************************************************************************
#define DIFFUSEMAP_CHANNEL		0x01
#define NORMALMAP_CHANNEL		0x02
#define GLOSSMAP_CHANNEL		0x04
#define LIGHTMAP_CHANNEL		0x08
#define MONO_CHANNEL			0x08	// 单色通道
#define AOMAP_CHANNEL			0x10
#define FILTERMAP_CHANNEL		0x10
#define GLOWMAP_CHANNEL			0x20
#define ENVMAP_CHANNEL			0x80	// GB的BumpMap通道

// 用于MRT
#define DETAIL_NORMAL_CHANNEL	0x10	
#define ALPHA_TEST_FLAG			0x20
#define VERTEX_COLOR_CHANNEL	0x40

// 用于根据flag动态生成变量名函数名之类的
#define HEADER_KEY(header, flag)	header##flag

// 用于生成字符串
#define MAKE_STRING(a)	#a
 
// 用于判定flag的指定位是否启用
#define TEST_CHANNEL(flag, channel)	(((flag / channel) / 2) != ((flag / channel + 1) / 2))
//*****************************************************************************
#endif