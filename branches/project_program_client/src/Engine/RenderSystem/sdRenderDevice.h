//*************************************************************************************************
// 内容:	渲染设备接口
//---------------------------------------------------------
// 作者:		
// 创建:		2012-06-29
// 最后修改:
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

	// 渲染设备接口,封装所有对渲染设备的操作
	// (封装了渲染状态管理部分,即NiDX9RenderState)
	// (封装了部分资源管理部分,即NiDX9Renderer)
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
		// 初始化与销毁
		virtual bool Initialize() = 0;
		virtual void Destroy() = 0;

		inline bool IsInitialized(){ return m_bIsInitialized;};

		//*************************************************
		// 渲染器状态设置
		//@{
		// 渲染状态
		virtual void SetRenderState(uint uiState, uint uiValue) = 0;
		virtual uint GetRenderState(uint uiState) = 0;

		virtual void SaveAllRenderStates() = 0;
		virtual void RestoreAllRenderStates() = 0;

		// 纹理采样器状态
		virtual void SetSampleState(uint uiSlot, uint uiState, uint uiValue) = 0;
		virtual uint GetSampleState(uint uiSlot, uint uiState) = 0;

		virtual void SaveAllSampleStates() = 0;
		virtual void RestoreAllSampleStates() = 0;

		// 清除D3D设备状态
		virtual	void Clear(uint* puiColor, float* pfDepth, uint* puiStencil) = 0;
		virtual void ClearVertexBinding() = 0;
		//virtual void ClearTextureBinding() = 0;

		// 相机
		virtual void SetCameraData(NiCamera* spCamera) = 0;

		// 渲染目标
		virtual void SetRenderTargetGroup(NiRenderTargetGroup* spRenderTarget) = 0;
		virtual NiRenderTargetGroup* GetRenderTargetGroup() = 0;
		virtual NiRenderTargetGroup* GetDefaultRenderTargetGroup() = 0;
		// @}

		//*************************************************
		// 着色器
		// @{
		// 初始化着色器系统(主要是设置着色器目录)
		virtual bool SetupShaderSystem(
			const char* apcProgramDirectoies[], uint uiNumProgramDirectoies,
			const char* apcShaderDirectoies[], uint uiNumShaderDirectoies) = 0;

		virtual void SetGlobalShaderConstant(const sdFixedString& szName, uint uiSize, const void* pData) = 0;
		virtual void GetGlobalShaderConstant(const sdFixedString& szName, uint& uiSize, const void*& pData) = 0;
		// @}

		//*************************************************
		// 材质
		// @{
		// 材质创建
		virtual NiMaterial* CreateMaterial(const sdFixedString& szName) = 0;
		// @}

		//*************************************************
		// 渲染器纹理资源创建
		//@{
		virtual NiRenderedTexturePtr	CreateRenderTexture(TextureSpec& kSpec) = 0;
		virtual	NiRenderTargetGroupPtr	CreateRenderTarget(uint uiNum) = 0;
		//virtual NiDX9Direct3DTexturePtr	CreateRawTexture() = 0;

		//virtual NiSourceTexturePtr		LoadTexture(const char* szTexName) = 0;
		//virtual	bool					SaveTexture(const char* szFileName, NiTexture* pTexture) = 0;
		//@}

		//*************************************************
		// 渲染设备丢失与重设事件
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