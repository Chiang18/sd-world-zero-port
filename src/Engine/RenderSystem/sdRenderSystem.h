//*************************************************************************************************
// 内容:	渲染系统基础接口
//---------------------------------------------------------
// 作者:		
// 创建:		2012-06-29
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _RENDER_SYSTEM_H__
#define _RENDER_SYSTEM_H__
#include <sdSingleton.h>
#include <sdVector2.h>
#include <sdVector3.h>
#include <sdVector4.h>
#include <NiMemObject.h>
#include "sdTypes.h"

// 前置声明
class sdMap;

namespace RenderSystem
{
	// 渲染参数,控制渲染策略与渲染质量
	class sdRenderParams : public NiMemObject
	{
	public:
		// 渲染通道,
		enum eShadingChannel
		{
			E_DIFFUSEMAP			= 0,
			E_NORMALMAP				= 1,
			E_GLOSSMAP				= 2,
			E_LIGHTMAP				= 3,
			E_AOMAP					= 4,
			E_FILTERMAP				= E_AOMAP,	///< for light and projector use
			E_GLOWMAP				= 5,
			E_DETAILNORMALMAP		= 6,
			E_ENVIRONMENTMAP		= 7,
			NUM_MAP_CHANNELS		= E_ENVIRONMENTMAP - E_DIFFUSEMAP + 1,

			E_DIFFUSELIGHTING		= 8,
			E_SPECULARLIGHTING		= 9,
			NUM_LIGHTING_CHINNELS	= E_SPECULARLIGHTING - E_DIFFUSELIGHTING + 1,

			E_AMBIENTLIGHT			= 10,
			E_MAINLIGHT				= 11,
			E_ASSISTLIGHT			= 12,
			E_LOCALLIGHT			= 13,
			NUM_LIGHT_CHINNELS		= E_LOCALLIGHT - E_AMBIENTLIGHT +1,

			NUM_SHADING_CHANNELS	= NUM_MAP_CHANNELS + NUM_LIGHTING_CHINNELS + NUM_LIGHT_CHINNELS,
		};

		// 物件类型
		enum eGameObjectType
		{
			E_BUILDING				= 0,	// 普通静态物件,例如建筑
			E_ACTOR					= 1,	// 动态物件,带骨骼动画
			E_TREELEAF				= 2,	// 地表灌木
			E_EFFECT				= 3,
			E_TERRAIN				= 4,	// 地形
			E_WEAPON				= 5,
			E_WATER					= 6,	// 水
			E_GODRAY				= 7,
			E_FOG					= 8,
			NUM_GAMEOBJECT_TYPE		= 9
		};

		sdRenderParams() { Reset();}
		void Reset();

		// 设置与访问渲染通道掩码
		void EnableChannel(eGameObjectType eObjectType, eShadingChannel eChannel, bool bEnable);
		bool IsEnableChannel(eGameObjectType eObjectType, eShadingChannel eChannel);

	public:
		// 整体各向异性过滤
		int				diffuseAnisotropicOffset;
		int				normalAnisotropicOffset;

		// 整体LOD偏移
		float			diffuseLodBiasOffset;
		float			normalLodBiasOffset;

		// 整体NormalMap强度缩放
		float			normalScaleOffset;

		// 渲染通道掩码
		unsigned int	buildingChannelMask;
		unsigned int	actorChannelMask;
		unsigned int	terrainChannelMask;

		//
		bool			densityMode;
		float			densityMinThreshold;
		float			densityMaxThreshold;
	};

	// 环境参数
	class sdEnvironmentParams : public NiMemObject
	{
	public:
		sdEnvironmentParams() { Reset();}
		void Reset();

	public:
		NiColor			fogColor;
		float			fogStart;
		float			fogEnd;
		float			fogHeightStart;
		float			fogHeightEnd;
		float			fogColorDensity;
		float			fogSkyDensity;
		float			fogDensity;

		NiColor			skyFogColor;
		float			skyFogTop;
		float			skyFogBottom;
		float			skyFogDensity;
	};

	// 后期特效参数
	class sdPostProcessParams : public NiMemObject
	{
	public:
		sdPostProcessParams() { Reset();}
		void Reset();

	public:
		// AA参数
		bool	enableMLAA;

		// ColorCorrection参数
		bool	enableColorCorrection;

		// Dof参数
		bool	enableDof;
		float	dofStart;
		float	dofEnd;
		float	dofSkyDepth;
		float	dofBlurWidth;

		// GodRay参数
		bool	enableGodRay;

		// HDR参数
		bool	enableHDR;

		// SSAO参数
		bool	enableSSAO;
	};
	NiSmartPointer(sdPostProcessParams);

	// 地形参数
	class sdTerrainParams : public NiRefObject
	{
	public:
		sdTerrainParams() { Reset();}
		void Reset();

	public:
		// 尺寸
		uint terrainSize;	// HeightMap Size,NormalMap Size
		uint blendMapSize;	// BlendMap Size
		uint tileMapSize;	// TileMap Size

		// 地形材质信息,Material info
		// @{
		Base::Math::sdVector3	ambientMaterial;
		Base::Math::sdVector3	diffuseMaterial;
		Base::Math::sdVector3	specularMaterial;
		float					shiness;
		// @}

		// 地形纹理信息,Texture info
		// @{
		NiTexture*	baseNormalMap;			// 整个地表的法线贴图
		NiTexture*	blendMap;				// 整个地表的纹理混合贴图
		NiTexture*	tileMap;				// 整个地表的纹理混合贴图查找表

		NiTexture*	diffuseAtlasMap;		// 整个地形漫反射贴图集
		NiTexture*	diffuseAtlasTableMap;	// 整个地形漫反射贴图查找表
		Base::Math::sdVector4 diffuseAtlasTableParam;
		
		NiTexture*	normalAtlasMap;			// 整个地形漫反射贴图集
		NiTexture*	normalAtlasTableMap;	// 整个地形漫反射贴图查找表
		Base::Math::sdVector4 normalAtlasTableParam;
		// @}

		// 
		float	terrainFarStart;		// 地形的"近处"与"远处"部分的分界距离
		float	normalScale;			// 地形NormalMap的法线缩放

		// 各向异性过滤,Anisotropic filter setting
		//	0:		means linear filter
		//	>=1:	means max anisotropic
		uint	diffuseAnisotropic;
		uint	normalAnisotropic;

		// LOD偏置, Mipmap lod bias setting
		// \remark Using it with anisotropic filter won't help visual quality 
		//		but hurt terrain rendering performance.)
		float	diffuseLodBias;
		float	normalLodBias;

		// 材质LOD信息,Material LOD info
		bool	useTriplanar;	// 是否使用立方体纹理
		bool	useSpecular;	// 是否使用高光

		// 调试信息,Debug params
		bool	showInvisiableLayers;
		bool	showTileGrid;
		bool	showWireframe;
	};

	// 渲染系统基础接口
	// 这里合并了原WZ的IRenderSystem/sdRenderMan/sdRenderSystem三个类
	//	1.一个Engine只可以有一个RenderSystem
	//  2.一个RenderSystem可以有多个RenderPath实例
	//	3.每个RenderPath管理多个RenderPass实例,掌握RenderTarget资源集,负责裁剪Map
	//	  每个RenderPass根据渲染RenderObject到RenderTarget
	//
	// RenderSystem相对独立,对于World部分,只知道sdMap,sdEntity
	class sdRenderSystem : public Base::sdTSingleton<sdRenderSystem>
	{
	public:
		// 缺省Mesh
		enum eDefaultMesh
		{
			E_SCREEN_QUAD,	///< LeftTop: coord(-1,1), uv(0,0); RightBottom:  coord(1,-1), uv(1,1)
			E_UNIT_CUBE,	///< Center: coord(0,0,0); Size Length 1.f
			E_UNIT_SPHERE,	///< Center: coord(0,0,0); Radius: 0.5f
			E_UNIT_CONE,	///< Center: top point(0,0,0), bottom center(1,0,0), bottom radius: 1.f
			E_UNIT_PYRAMID,	///< Center: top point(0,0,0), bottom center(1,0,0), bottom side length: 2.f
			NUM_DEFAULT_MESHES,
		};

		// 缺省材质
		enum eDefaultMaterial
		{
			E_STATIC_FLAT_SHADING,
			E_SKINNED_FLAT_SHAING,
			NUM_DEFAULT_MATERIALS,
		};

		// 缺省纹理
		enum eDefaultTexture
		{
			E_ERROE_TEXTURE,
			E_BLACK_TEXTURE,
			E_WHITE_TEXTURE,
			E_GRAY75_TEXTURE,
			E_CHESSBOARD_TEXTURE,
			E_UP_NORMALMAP,
			E_MIPMAP512_TEXTURE,
			NUM_DEFAULT_TEXTURES,
		};

		sdRenderSystem();
		virtual ~sdRenderSystem();

		// 初始化与销毁
		virtual bool Initialize() = 0;
		virtual void Destroy() = 0;

		inline bool IsInitialized(){ return m_bInitialized;};

		// 进行绘制的地图
		virtual void SetMap(sdMap* pkMap) = 0;
		virtual sdMap* GetMap() = 0;

		// 渲染相机
		virtual void SetCamera(NiCamera* spCamera) = 0;
		virtual NiCamera* GetCamera() = 0;

		// 渲染目标
		//virtual bool SetRenderTargetGroup(NiRenderTargetGroup* spRenderTargetGroup) = 0;

		//@{
		// 主渲染流程
		virtual void UpdateFrame(float fAccumTime, float fFrameTime) = 0;
		virtual void RenderFrame() = 0;
		virtual void DisplayFrame() = 0;

		// 单独的渲染UI,以确保登录等界面的流畅
		//virtual void RenderUIFrame() = 0;

		// 单独渲染其他物件,主要用于编辑器等
		//virtual void BeginDraw() = 0;
		//virtual void Draw(NiAVObject* spRoot) = 0;
		//virtual void DrawImmediate(NiAVObject* spRoot) = 0;
		//virtual void EndDraw() = 0;
		//@}

		//@{
		// 渲染属性设置,调整渲染效果
		//@}

		//@{
		// 缺省渲染资源
		virtual NiMesh*		GetDefaultMesh(eDefaultMesh eMesh) = 0;
		virtual NiMaterial*	GetDefaultMaterial(eDefaultMaterial eMaterial) = 0;
		//virtual NiTexture*	GetDefaultTexture(eDefaultTexture eTexture) = 0;
		//@}

		//@{
		// 渲染设备
		//typedef bool (*RESETNOTIFICATIONFUNC)(void* pData);
		//virtual uint AddResetNotificationFunc(RESETNOTIFICATIONFUNC pfnNotify, void* pData) = 0;
		//virtual bool RemoveResetNotification(RESETNOTIFICATIONFUNC pfnNotify) = 0;
		//virtual void CallResetNotificationFunc() = 0;
		//@}

	protected:
		bool	m_bInitialized;
	};
};
#endif