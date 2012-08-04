//*************************************************************************************************
// ����:	��Ⱦ�豸��GB-D3D9ʵ��
//---------------------------------------------------------
// ����:		��ǿ
// ����:		2012-06-30
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _RENDER_DEVICE_DX9_H__
#define _RENDER_DEVICE_DX9_H__
#include "sdRenderDevice.h"

namespace RenderSystem
{

	class sdRenderDevice_DX9: public sdRenderDevice
	{
	public:
		sdRenderDevice_DX9();
		virtual ~sdRenderDevice_DX9();

		//*************************************************
		// �麯���̳�
		virtual bool Initialize();
		virtual void Destroy();

		virtual void SetRenderState(uint uiState, uint uiValue);
		virtual uint GetRenderState(uint uiState);

		virtual void SaveAllRenderStates();
		virtual void RestoreAllRenderStates();

		virtual void SetSampleState(uint uiSlot, uint uiState, uint uiValue);
		virtual uint GetSampleState(uint uiSlot, uint uiState);

		virtual void SaveAllSampleStates();
		virtual void RestoreAllSampleStates();

		virtual	void Clear(uint* puiColor, float* pfDepth, uint* puiStencil);
		virtual void ClearVertexBinding();

		virtual bool SetupShaderSystem(const char* apcProgramDirectoies[], uint uiNumProgramDirectoies,	const char* apcShaderDirectoies[], uint uiNumShaderDirectoies);
		virtual void SetGlobalShaderConstant(const sdFixedString& szName, uint uiSize, const void* pData);
		virtual void GetGlobalShaderConstant(const sdFixedString& szName, uint& uiSize, const void*& pData);

		virtual void SetCameraData(NiCamera* spCamera);

		virtual void SetRenderTargetGroup(NiRenderTargetGroup* spRenderTarget);
		virtual NiRenderTargetGroup* GetRenderTargetGroup();
		virtual NiRenderTargetGroup* GetDefaultRenderTargetGroup();

		virtual NiMaterial* CreateMaterial(const sdFixedString& szName);

		virtual NiRenderedTexturePtr	CreateRenderTexture(TextureSpec& kSpec);
		virtual	NiRenderTargetGroupPtr	CreateRenderTarget(uint uiNum);
		//virtual NiDX9Direct3DTexturePtr	CreateRawTexture();

		//virtual NiSourceTexturePtr		LoadTexture(const char* szTexName);
		//virtual	bool					SaveTexture(const char* szFileName, NiTexture* pTexture);

		virtual uint AddLostEventDelegate(LostEventDelegate pfFunc, void* pData);
		virtual uint AddResetEventDelegate(ResetEventDelegate pfFunc, void* pData);
		virtual bool RemoveLostEventDelegate(uint uiIndex);
		virtual bool RemoveResetEventDelegate(uint uiIndex);
		virtual void RemoveAllEventDelegate();

	protected:
		// fx��ɫ����صĻص�����
		// @{
		static bool FXLibraryClassCreate(const char* pcLibFile, NiRenderer* spRender,
			int iDirectoryCount, const char* apcDirecties[],
			bool bRecurseSubFolders, NiShaderLibrary** sppLibrary);
		//static bool FXLiteLibraryClassCreate(const char* pcLibFile, NiRenderer* spRender,
		//	int iDirectoryCount const char* apcDirecties[],
		//	bool bRecurseSubFolders, NiShaderLibrary** sppLibrary);
		// }@

	protected:
		NiDX9RendererPtr		m_spDX9Render;
		NiDX9RenderStatePtr		m_spDX9RenderState;
		NiRenderTargetGroupPtr	m_spRenderTarget;
	};

}
#endif