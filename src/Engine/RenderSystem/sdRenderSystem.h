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
#include <NiMemObject.h>
#include "sdTypes.h"

// ǰ������
class sdMap;

namespace RenderSystem
{
	// ��Ⱦ����
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

		// �������,������Ⱦϵͳʶ���Ҳ�ȡ��ͬ����Ⱦ����
		enum eGameObjectType
		{
			E_BUILDING				= 0,	// ��ͨ��̬���,���罨��
			E_ACTOR					= 1,	// ��̬���,����������
			E_TREELEAF				= 2,	// �ر��ľ
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

		// �����������Ⱦͨ������
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
		float dofStart;
		float dofEnd;
		float dofSkyDepth;
		float dofBlurWidth;
	};
	NiSmartPointer(sdPostProcessParams);

	// ���λ��Ʋ���
	class sdTerrainParams : public NiRefObject
	{
	public:
		sdTerrainParams() { Reset();}
		void Reset();

	public:
		// ��������ߴ�
		Base::Math::sdVector2ui terrainSize;

		// ���β�����Ϣ
		Base::Math::sdVector3	ambientMaterial;
		Base::Math::sdVector3	diffuseMaterial;
		Base::Math::sdVector3	specularMaterial;
		float					shiness;

		// ����������Ϣ
		// @{
		NiTexture*	baseLightMap;			// �����ر�Ĺ�����ͼ
		NiTexture*	baseNormalMap;			// �����ر�ķ�����ͼ

		NiTexture*	spBlendMap;				// �����ر����������ͼ
		NiTexture*	spTileMap;				// �����ر����������ͼ���ұ�

		NiTexture*	diffuseAtlasMap;		// ����������������ͼ��
		NiTexture*	diffuseAtlasTableMap;	// ����������������ͼ���ұ�
		
		NiTexture*	normalAtlasMap;			// ����������������ͼ��
		NiTexture*	normalAtlasTableMap;	// ����������������ͼ���ұ�
		// @}

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
			E_SCREEN_QUAD,
			E_UNIT_CUBE,
			E_UNIT_SPHERE,
			E_UNIT_CONE,
			E_UNIT_PYRAMID,
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