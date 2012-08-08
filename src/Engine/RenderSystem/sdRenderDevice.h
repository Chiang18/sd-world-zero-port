//*************************************************************************************************
// ����:	��Ⱦ�豸�ӿ�
//---------------------------------------------------------
// ����:		
// ����:		2012-06-29
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _RENDER_DEVICE_H__
#define _RENDER_DEVICE_H__
#include <sdSingleton.h>
#include "sdFixedString.h"
#include "sdTypes.h"
#include "sdEngine.h"
#include <NiDX9Direct3DTexture.h>

namespace RenderSystem
{

	typedef NiDX9Renderer::LOSTDEVICENOTIFYFUNC LostEventDelegate;
	typedef NiDX9Renderer::RESETNOTIFYFUNC		ResetEventDelegate;

	// ��Ⱦ�豸�ӿ�,��װ���ж���Ⱦ�豸�Ĳ���
	// (��װ����Ⱦ״̬������,��NiDX9RenderState)
	// (��װ�˲�����Դ������,��NiDX9Renderer)
	class sdRenderDevice : public NiMemObject, public Base::sdTSingleton<sdRenderDevice>
	{
	public:
		struct TextureSpec
		{
			uint	uiWidth;
			uint	uiHeight;
			NiTexture::FormatPrefs kFormat;
		};

		sdRenderDevice();
		virtual ~sdRenderDevice() = 0;

		//*************************************************
		// ��ʼ��������
		virtual bool Initialize() = 0;
		virtual void Destroy() = 0;

		inline bool IsInitialized(){ return m_bIsInitialized;};

		//*************************************************
		// ��Ⱦ��״̬����
		//@{
		// ��Ⱦ״̬
		virtual void SetRenderState(uint uiState, uint uiValue) = 0;
		virtual uint GetRenderState(uint uiState) = 0;

		virtual void SaveAllRenderStates() = 0;
		virtual void RestoreAllRenderStates() = 0;

		// ���������״̬
		virtual void SetSampleState(uint uiSlot, uint uiState, uint uiValue) = 0;
		virtual uint GetSampleState(uint uiSlot, uint uiState) = 0;

		virtual void SaveAllSampleStates() = 0;
		virtual void RestoreAllSampleStates() = 0;

		// ���D3D�豸״̬
		virtual	void Clear(uint* puiColor, float* pfDepth, uint* puiStencil) = 0;
		virtual void ClearVertexBinding() = 0;
		//virtual void ClearTextureBinding() = 0;

		// ���
		virtual void SetCameraData(NiCamera* spCamera) = 0;

		// ��ȾĿ��
		virtual void SetRenderTargetGroup(NiRenderTargetGroup* spRenderTarget) = 0;
		virtual NiRenderTargetGroup* GetRenderTargetGroup() = 0;
		virtual NiRenderTargetGroup* GetDefaultRenderTargetGroup() = 0;
		// @}

		//*************************************************
		// ��ɫ��
		// @{
		// ��ʼ����ɫ��ϵͳ(��Ҫ��������ɫ��Ŀ¼)
		virtual bool SetupShaderSystem(
			const char* apcProgramDirectoies[], uint uiNumProgramDirectoies,
			const char* apcShaderDirectoies[], uint uiNumShaderDirectoies) = 0;

		virtual void SetGlobalShaderConstant(const sdFixedString& szName, uint uiSize, const void* pData) = 0;
		virtual void GetGlobalShaderConstant(const sdFixedString& szName, uint& uiSize, const void*& pData) = 0;
		// @}

		//*************************************************
		// ����
		// @{
		// ���ʴ���
		virtual NiMaterial* CreateMaterial(const sdFixedString& szName) = 0;
		// @}

		//*************************************************
		// ��Ⱦ��������Դ����
		//@{
		virtual NiRenderedTexturePtr	CreateRenderTexture(TextureSpec& kSpec) = 0;
		virtual	NiRenderTargetGroupPtr	CreateRenderTarget(uint uiNum) = 0;
		//virtual NiDX9Direct3DTexturePtr	CreateRawTexture() = 0;

		//virtual NiSourceTexturePtr		LoadTexture(const char* szTexName) = 0;
		//virtual	bool					SaveTexture(const char* szFileName, NiTexture* pTexture) = 0;
		//@}

		//*************************************************
		// ��Ⱦ�豸��ʧ�������¼�
		//@{
		virtual uint AddLostEventDelegate(LostEventDelegate pfFunc, void* pData) = 0;
		virtual uint AddResetEventDelegate(ResetEventDelegate pfFunc, void* pData) = 0;

		virtual bool RemoveLostEventDelegate(uint uiIndex) = 0;
		virtual bool RemoveResetEventDelegate(uint uiIndex) = 0;

		virtual void RemoveAllEventDelegate() = 0;
		//@}

	protected:
		bool	m_bIsInitialized;
	};

}
#endif