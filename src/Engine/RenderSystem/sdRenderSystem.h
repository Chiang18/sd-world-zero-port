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
#include <NiMemObject.h>
#include "sdTypes.h"

// 前置声明
class sdMap;

namespace RenderSystem
{
	// 渲染参数
	class sdRenderParams : public NiMemObject
	{
	public:
		enum eShadingChannel
		{
			E_DIFFUSEMAP			= 0,
			E_NORMALMAP				= 1,
			E_GLOSSMAP				= 2,
			E_LIGHTMAP				= 3,
			E_AOMAP					= 4,
			E_FILTERMAP				= E_AOMAP,
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

		// 物件类型,用于渲染系统识别，且采取不同的渲染策略
		enum eGameObjectType
		{
			E_BUILDING				= 0,	// 普通静态物件,例如建筑
			E_ACTOR					= 1,	// 动态物件,带骨骼动画
			E_TREELEAF				= 2,	// 地表灌木
			E_EFFECT				= 3,
			E_TERRAIN				= 4,
			E_WEAPON				= 5,
			E_WATER					= 6,
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
		int				diffuseAnisotropicOffset;
		int				normalAnisotropicOffset;
		float			diffuseLodBiasOffset;
		float			normalLodBiasOffset;
		float			normalScaleOffset;

		unsigned int	buildingChannelMask;
		unsigned int	actorChannelMask;
		unsigned int	terrainChannelMask;

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
		float dofStart;
		float dofEnd;
		float dofSkyDepth;
		float dofBlurWidth;
	};
	NiSmartPointer(sdPostProcessParams);

	// 地形绘制参数
	class sdTerrainParams : public NiRefObject
	{
	public:
		sdTerrainParams() { Reset();}
		void Reset();

	public:
		// 地形总体尺寸
		Base::Math::sdVector2ui terrainSize;

		// 地形材质信息
		Base::Math::sdVector3	ambientMaterial;
		Base::Math::sdVector3	diffuseMaterial;
		Base::Math::sdVector3	specularMaterial;
		float					shiness;

		// 地形纹理信息
		// @{
		NiTexture*	baseLightMap;			// 整个地表的光照贴图
		NiTexture*	baseNormalMap;			// 整个地表的法线贴图

		NiTexture*	spBlendMap;				// 整个地表的纹理混合贴图
		NiTexture*	spTileMap;				// 整个地表的纹理混合贴图查找表

		NiTexture*	diffuseAtlasMap;		// 整个地形漫反射贴图集
		NiTexture*	diffuseAtlasTableMap;	// 整个地形漫反射贴图查找表
		
		NiTexture*	normalAtlasMap;			// 整个地形漫反射贴图集
		NiTexture*	normalAtlasTableMap;	// 整个地形漫反射贴图查找表
		// @}

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
			E_SCREEN_QUAD,
			E_UNIT_CUBE,
			E_UNIT_SPHERE,
			E_UNIT_CONE,
			E_UNIT_PYRAMID,
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