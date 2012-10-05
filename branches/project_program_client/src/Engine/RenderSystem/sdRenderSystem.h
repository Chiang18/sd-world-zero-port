//*************************************************************************************************
// ����:	��Ⱦϵͳ�����ӿ�
//---------------------------------------------------------
// ����:		
// ����:		2012-06-29
// ����޸�:
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

// ǰ������
class sdMap;

namespace RenderSystem
{
	// ��Ⱦ����,������Ⱦ��������Ⱦ����
	class sdRenderParams : public NiMemObject
	{
	public:
		// ��Ⱦͨ��,
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

		// �������
		enum eGameObjectType
		{
			E_BUILDING				= 0,	// ��ͨ��̬���,���罨��
			E_ACTOR					= 1,	// ��̬���,����������
			E_TREELEAF				= 2,	// �ر��ľ
			E_EFFECT				= 3,
			E_TERRAIN				= 4,	// ����
			E_WEAPON				= 5,
			E_WATER					= 6,	// ˮ
			E_GODRAY				= 7,
			E_FOG					= 8,
			NUM_GAMEOBJECT_TYPE		= 9
		};

		sdRenderParams() { Reset();}
		void Reset();

		// �����������Ⱦͨ������
		void EnableChannel(eGameObjectType eObjectType, eShadingChannel eChannel, bool bEnable);
		bool IsEnableChannel(eGameObjectType eObjectType, eShadingChannel eChannel);

	public:
		// ����������Թ���
		int				diffuseAnisotropicOffset;
		int				normalAnisotropicOffset;

		// ����LODƫ��
		float			diffuseLodBiasOffset;
		float			normalLodBiasOffset;

		// ����NormalMapǿ������
		float			normalScaleOffset;

		// ��Ⱦͨ������
		unsigned int	buildingChannelMask;
		unsigned int	actorChannelMask;
		unsigned int	terrainChannelMask;

		//
		bool			densityMode;
		float			densityMinThreshold;
		float			densityMaxThreshold;
	};

	// ��������
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

	// ������Ч����
	class sdPostProcessParams : public NiMemObject
	{
	public:
		sdPostProcessParams() { Reset();}
		void Reset();

	public:
		// AA����
		bool	enableMLAA;

		// ColorCorrection����
		bool	enableColorCorrection;

		// Dof����
		bool	enableDof;
		float	dofStart;
		float	dofEnd;
		float	dofSkyDepth;
		float	dofBlurWidth;

		// GodRay����
		bool	enableGodRay;

		// HDR����
		bool	enableHDR;

		// SSAO����
		bool	enableSSAO;
	};
	NiSmartPointer(sdPostProcessParams);

	// ���β���
	class sdTerrainParams : public NiRefObject
	{
	public:
		sdTerrainParams() { Reset();}
		void Reset();

	public:
		// �ߴ�
		uint terrainSize;	// HeightMap Size,NormalMap Size
		uint blendMapSize;	// BlendMap Size
		uint tileMapSize;	// TileMap Size

		// ���β�����Ϣ,Material info
		// @{
		Base::Math::sdVector3	ambientMaterial;
		Base::Math::sdVector3	diffuseMaterial;
		Base::Math::sdVector3	specularMaterial;
		float					shiness;
		// @}

		// ����������Ϣ,Texture info
		// @{
		NiTexture*	baseNormalMap;			// �����ر�ķ�����ͼ
		NiTexture*	blendMap;				// �����ر����������ͼ
		NiTexture*	tileMap;				// �����ر����������ͼ���ұ�

		NiTexture*	diffuseAtlasMap;		// ����������������ͼ��
		NiTexture*	diffuseAtlasTableMap;	// ����������������ͼ���ұ�
		Base::Math::sdVector4 diffuseAtlasTableParam;
		
		NiTexture*	normalAtlasMap;			// ����������������ͼ��
		NiTexture*	normalAtlasTableMap;	// ����������������ͼ���ұ�
		Base::Math::sdVector4 normalAtlasTableParam;
		// @}

		// 
		float	terrainFarStart;		// ���ε�"����"��"Զ��"���ֵķֽ����
		float	normalScale;			// ����NormalMap�ķ�������

		// �������Թ���,Anisotropic filter setting
		//	0:		means linear filter
		//	>=1:	means max anisotropic
		uint	diffuseAnisotropic;
		uint	normalAnisotropic;

		// LODƫ��, Mipmap lod bias setting
		// \remark Using it with anisotropic filter won't help visual quality 
		//		but hurt terrain rendering performance.)
		float	diffuseLodBias;
		float	normalLodBias;

		// ����LOD��Ϣ,Material LOD info
		bool	useTriplanar;	// �Ƿ�ʹ������������
		bool	useSpecular;	// �Ƿ�ʹ�ø߹�

		// ������Ϣ,Debug params
		bool	showInvisiableLayers;
		bool	showTileGrid;
		bool	showWireframe;
	};

	// ��Ⱦϵͳ�����ӿ�
	// ����ϲ���ԭWZ��IRenderSystem/sdRenderMan/sdRenderSystem������
	//	1.һ��Engineֻ������һ��RenderSystem
	//  2.һ��RenderSystem�����ж��RenderPathʵ��
	//	3.ÿ��RenderPath������RenderPassʵ��,����RenderTarget��Դ��,����ü�Map
	//	  ÿ��RenderPass������ȾRenderObject��RenderTarget
	//
	// RenderSystem��Զ���,����World����,ֻ֪��sdMap,sdEntity
	class sdRenderSystem : public Base::sdTSingleton<sdRenderSystem>
	{
	public:
		// ȱʡMesh
		enum eDefaultMesh
		{
			E_SCREEN_QUAD,	///< LeftTop: coord(-1,1), uv(0,0); RightBottom:  coord(1,-1), uv(1,1)
			E_UNIT_CUBE,	///< Center: coord(0,0,0); Size Length 1.f
			E_UNIT_SPHERE,	///< Center: coord(0,0,0); Radius: 0.5f
			E_UNIT_CONE,	///< Center: top point(0,0,0), bottom center(1,0,0), bottom radius: 1.f
			E_UNIT_PYRAMID,	///< Center: top point(0,0,0), bottom center(1,0,0), bottom side length: 2.f
			NUM_DEFAULT_MESHES,
		};

		// ȱʡ����
		enum eDefaultMaterial
		{
			E_STATIC_FLAT_SHADING,
			E_SKINNED_FLAT_SHAING,
			NUM_DEFAULT_MATERIALS,
		};

		// ȱʡ����
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

		// ��ʼ��������
		virtual bool Initialize() = 0;
		virtual void Destroy() = 0;

		inline bool IsInitialized(){ return m_bInitialized;};

		// ���л��Ƶĵ�ͼ
		virtual void SetMap(sdMap* pkMap) = 0;
		virtual sdMap* GetMap() = 0;

		// ��Ⱦ���
		virtual void SetCamera(NiCamera* spCamera) = 0;
		virtual NiCamera* GetCamera() = 0;

		// ��ȾĿ��
		//virtual bool SetRenderTargetGroup(NiRenderTargetGroup* spRenderTargetGroup) = 0;

		//@{
		// ����Ⱦ����
		virtual void UpdateFrame(float fAccumTime, float fFrameTime) = 0;
		virtual void RenderFrame() = 0;
		virtual void DisplayFrame() = 0;

		// ��������ȾUI,��ȷ����¼�Ƚ��������
		//virtual void RenderUIFrame() = 0;

		// ������Ⱦ�������,��Ҫ���ڱ༭����
		//virtual void BeginDraw() = 0;
		//virtual void Draw(NiAVObject* spRoot) = 0;
		//virtual void DrawImmediate(NiAVObject* spRoot) = 0;
		//virtual void EndDraw() = 0;
		//@}

		//@{
		// ��Ⱦ��������,������ȾЧ��
		//@}

		//@{
		// ȱʡ��Ⱦ��Դ
		virtual NiMesh*		GetDefaultMesh(eDefaultMesh eMesh) = 0;
		virtual NiMaterial*	GetDefaultMaterial(eDefaultMaterial eMaterial) = 0;
		//virtual NiTexture*	GetDefaultTexture(eDefaultTexture eTexture) = 0;
		//@}

		//@{
		// ��Ⱦ�豸
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