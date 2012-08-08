//*************************************************************************************************
// ����:	��Ⱦ��������ӿ�
//---------------------------------------------------------
// ����:		
// ����:		2012-07-04
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _RENDER_PASS_H__
#define _RENDER_PASS_H__
#include <NiRefObject.h>
#include "sdTypes.h"
#include "sdRenderObject.h"

namespace RenderSystem
{
	// ǰ������
	class sdRenderParams;
	class sdRenderObject;
	class sdRenderObjectAlloc;

	// ��Ⱦ����
	// RenderPass�����Լ���RenderObject����ͨ��RenderDevice��Ⱦ���ض�RenderTarget
	class sdRenderPass : public NiRefObject
	{
	public:
		sdRenderPass(sdRenderObjectAlloc* pkAlloc);
		virtual ~sdRenderPass();

		inline bool IsInitialized(){ return m_bInitialized;};

		// �Ƿ񼤻�
		inline bool GetActive() { return m_bActived;};
		inline bool SetActive(bool bActive){ m_bActived = bActive;};

		// ������Ⱦ����
		virtual void SetRenderParams(const sdRenderParams* pkRenderParams);

		//@{
		// ���ƽӿں���
		virtual void Begin();
		virtual void Draw();
		virtual void End();
		//@}

	protected:
		//@{
		// �����Ӷ�����Ⱦ����
		sdRenderObject::ShaderParamItem* CloneShaderParams(sdRenderObject::ShaderParamItem* pkParams);
		void InsertRenderObject(sdRenderObject* pkObject);
		void InsertStaticObject(NiMesh* spMesh, NiMaterial* spMaterial,
			NiPropertyList* spRenderProps, sdRenderObject::ShaderParamItem* pkShaderParams);
		void ClearObjects();
		//@}

		//@{
		// Ĭ�ϵ�������
		typedef bool (*RenderObjectComparator)(sdRenderObject* lhs, sdRenderObject* rhs);
		static bool	Comparator(sdRenderObject* lhs, sdRenderObject* rhs);

		// ��ȡ��ǰ������
		virtual RenderObjectComparator GetComparator();

		// �����������
		virtual void Sort();
		//@}

	protected:
		bool	m_bInitialized;
		bool	m_bActived;

		typedef std::vector<sdRenderObject*>	RenderObjectVec;
		typedef std::vector<sdRenderObject*>::iterator	RenderObjectVecItr;
		RenderObjectVec			m_kRenderObjectVec;
		sdRenderObjectAlloc*	m_pkAlloc;
		sdRenderParams*			m_pkRenderParams;
	};
	NiSmartPointer(sdRenderPass);
}
#endif