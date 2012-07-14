//*************************************************************************************************
// ����:	��Ⱦ·���ӿ�
//---------------------------------------------------------
// ����:		��ǿ
// ����:		2012-07-04
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _RENDER_PATH_H__
#define _RENDER_PATH_H__
#include <NiRefObject.h>
#include "sdTypes.h"
#include "sdRenderPass.h"

// ǰ������
class sdMap;

namespace RenderSystem
{
	// ǰ������
	class sdRenderParams;
	class sdEnvironmentParams;
	class sdPostProcessParams;

	// ��Ⱦ·���ӿ�
	// ��Ⱦ·������Ⱦ���Ե�һ�ַ�װ,�������������Ⱦһ��ָ����Map��ָ����RenderTarget
	//
	// RenderPath����������Ⱦ,
	//	1.����RenderPass���к������֯�͵���,�Լ�����һ�����޵���Դ������
	//	2.�ü�Map�õ�Mesh����,
	//	3.���Mesh��RenderObject
	//	4.���RenderObject����Ӧ��RenderPass
	//
	// RenderPass����һ���ض����м䲽��,����RenderObject��Ⱦ��RenderTarget
	class sdRenderPath : public NiRefObject
	{
	public:
		sdRenderPath();
		virtual ~sdRenderPath();

		//*************************************************
		// ��ʼ��������
		virtual bool Initialize() = 0;
		virtual void Destroy() = 0;

		inline bool IsInitialized(){ return m_bInitialized;};

		// �豸�ؽ��ص�
		//virtual bool Recreate() = 0;

		// ��Ⱦ�ܽӿ�,����ü�(Entity������Mesh����)����Ⱦ
		virtual void RenderScene(sdMap* pkMap, NiCamera* spCamera, NiRenderTargetGroup* spRenderTarget) = 0;

		//@{
		// ������Ⱦ����
		virtual void UpdateRenderParams(const sdRenderParams& kRenderParams) = 0;
		virtual void UpdateEnvironmentParams(const sdEnvironmentParams& kEnvParams) = 0;
		virtual void UpdatePostProcessParams(const sdPostProcessParams& kPostProcessParams) = 0;

		//@{
		// �ü�����Ⱦ�������ӿ�
		// (�������ҪĿ���ǽ�RenderPass�Ĺ��ܽӿڷ�װ֮��¶����)
		//virtual void RenderTerrainMesh(NiMesh* spMesh) = 0;
		//virtual void RenderTerrainHole(NiMesh* spMesh) = 0;
		virtual void RenderStaticMesh(NiMesh* spMesh) = 0;
		//virtual void RenderSkinnedMesh(NiMesh* spMesh) = 0;
		//virtual void RenderLight(NiLight* spLight) = 0;
		//virtual void RenderDecal()
		//virtual void RenderRoad()
		//virtual void RenderProjector()
		//@}

	protected:
		bool	m_bInitialized;
	};
	NiSmartPointer(sdRenderPath);
}
#endif