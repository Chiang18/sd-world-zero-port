#include "Stdafx.h"
#include "sdDynamicTexture.h"
#include <NiDX9Direct3DTextureData.h>

namespace RenderSystem
{
//-------------------------------------------------------------------------------------------------
D3DFORMAT sdDynamicTexture::ms_kD3DFormatTable[NUM_FORMATS] = 
{
	D3DFMT_A8R8G8B8,
	D3DFMT_A8,
	D3DFMT_A8R8G8B8,
	D3DFMT_A4R4G4B4,
	D3DFMT_R8G8B8,
};

sdDynamicTexture* sdDynamicTexture::ms_pkHead = NULL;
sdDynamicTexture* sdDynamicTexture::ms_pkTail = NULL;
uint sdDynamicTexture::ms_uiLostNotifyIndex = 0;
uint sdDynamicTexture::ms_uiResetNotifyIndex = 0;
//-------------------------------------------------------------------------------------------------
void sdDynamicTexture::StaticInitialize()
{
	NiDX9Renderer* spRenderer = NiDX9Renderer::GetRenderer();
	NIASSERT(spRenderer);
	ms_uiLostNotifyIndex = spRenderer->AddLostDeviceNotificationFunc(sdDynamicTexture::DeviceLostCallBack, NULL);
	ms_uiResetNotifyIndex = spRenderer->AddResetNotificationFunc(sdDynamicTexture::DeviceResetCallBack, NULL);

	ms_pkHead = NULL;
	ms_pkTail = NULL;
}
//-------------------------------------------------------------------------------------------------
void sdDynamicTexture::StaticDestroy()
{
	NiDX9Renderer* spRenderer = NiDX9Renderer::GetRenderer();
	NIASSERT(spRenderer);
	spRenderer->RemoveLostDeviceNotificationFunc(ms_uiLostNotifyIndex);
	spRenderer->RemoveResetNotificationFunc(ms_uiResetNotifyIndex);

	ms_pkHead = NULL;
	ms_pkTail = NULL;
}
//-------------------------------------------------------------------------------------------------
bool sdDynamicTexture::DeviceLostCallBack(void*)
{
	// 为啥?
	::Sleep(50);	

	// 遍历列表,回调
	if (ms_pkHead)
	{
		sdDynamicTexture* pkTexture = ms_pkHead;
		do
		{
			pkTexture->OnDeviceLost();
		
			pkTexture = pkTexture->m_pkNext;
			if (pkTexture == ms_pkHead)
				break;
		}while(1);
	};
	
	return  true;
}
//-------------------------------------------------------------------------------------------------
bool sdDynamicTexture::DeviceResetCallBack(bool bBefore, void* pvData)
{
	// 遍历列表,回调
	if (ms_pkHead)
	{
		sdDynamicTexture* pkTexture = ms_pkHead;
		do
		{
			pkTexture->OnDeviceReset(bBefore);

			pkTexture = pkTexture->m_pkNext;
			if (pkTexture == ms_pkHead)
				break;
		}while(1);
	};

	return  true;
}
//-------------------------------------------------------------------------------------------------
sdDynamicTexture::sdDynamicTexture(uint uiWidth, uint uiHeight, eDynamicFormat eFormat, bool bMipmap)
: m_pkPrev(NULL)
, m_pkNext(NULL)
, m_spRenderer(NULL)
, m_spD3DDevice(NULL)
, m_spD3DSysTexture(NULL)
, m_spD3DVedTexture(NULL)
{
	// 添加列表
	AddTextureToList();

	// 获取渲染设备
	m_spRenderer = NiDX9Renderer::GetRenderer();
	NIASSERT(m_spRenderer);
	m_spD3DDevice = m_spRenderer->GetD3DDevice();

	//
	m_uiWidth = uiWidth;
	m_uiHeight = uiHeight;
	m_eFormat = eFormat;
	m_bMipmap = bMipmap;

	// 创建D3D纹理
	uint uiLevel = (bMipmap ? 1: 0);

	HRESULT hr = D3DXCreateTexture(m_spD3DDevice, 
		m_uiWidth, 
		m_uiHeight, 
		uiLevel, 
		0,								// 内存纹理居然是0...
		ms_kD3DFormatTable[m_eFormat],
		D3DPOOL_SYSTEMMEM,
		&m_spD3DSysTexture);
	if (FAILED(hr))	
	{
		NIASSERT(0);
		return;
	}

	hr = D3DXCreateTexture(m_spD3DDevice, 
		m_uiWidth, 
		m_uiHeight, 
		uiLevel, 
		D3DUSAGE_DYNAMIC,
		ms_kD3DFormatTable[m_eFormat],
		D3DPOOL_DEFAULT,
		&m_spD3DVedTexture);
	if (FAILED(hr))	
	{
		NIASSERT(0);
		return;
	}

	// 创建GB纹理对象
	m_spTexture = m_spRenderer->CreateNiTextureFromD3DTexture(m_spD3DVedTexture);
	NIASSERT(m_spTexture);
}
//-------------------------------------------------------------------------------------------------
sdDynamicTexture::~sdDynamicTexture()
{
	// 从列表移除
	RemoveTextureFromList();

	//
	if (m_spD3DVedTexture)
	{
		m_spD3DVedTexture->Release();
		m_spD3DVedTexture = NULL;
	}

	if (m_spD3DSysTexture)
	{
		m_spD3DSysTexture->Release();
		m_spD3DSysTexture = NULL;
	}

	if (m_spTexture)
	{
		m_spRenderer->PurgeTexture(m_spTexture);
		m_spTexture = 0;
	}

	m_pkPrev = NULL;
	m_pkNext = NULL;
}
//-------------------------------------------------------------------------------------------------
sdDynamicTexture::sdLockedRect sdDynamicTexture::LockRegion(uint uiX, uint uiY, uint uiW, uint uiH)
{
	sdLockedRect kLockedData;

	// 锁定内存纹理
	D3DLOCKED_RECT kD3DLockRect;
	RECT kRect[] = {uiX, uiY, uiX + uiW, uiY + uiH};
	if (FAILED(m_spD3DSysTexture->LockRect(0, &kD3DLockRect, kRect, D3DLOCK_NO_DIRTY_UPDATE)))
	{
		NIASSERT(0);

		kLockedData.pBits = 0;
		kLockedData.uiPitch = 0;
		return kLockedData;
	}

	m_spD3DSysTexture->AddDirtyRect(kRect);
	
	kLockedData.pBits = kD3DLockRect.pBits;
	kLockedData.uiPitch = kD3DLockRect.Pitch;

	return kLockedData;
}
//-------------------------------------------------------------------------------------------------
void sdDynamicTexture::UnlockRegion()
{
	if (FAILED(m_spD3DSysTexture->UnlockRect(0)))
	{
		NIASSERT(0);
		return;
	}

	// 设备丢失
	if (m_spRenderer->IsDeviceLost())
		return;

	// 更新到最终纹理
	if (FAILED(m_spD3DDevice->UpdateTexture(m_spD3DSysTexture, m_spD3DVedTexture)))
	{
		NIASSERT(0);
		return;
	}
}
//-------------------------------------------------------------------------------------------------
void sdDynamicTexture::AddTextureToList()
{
	if (!ms_pkHead)
		ms_pkHead = this;

	if (ms_pkTail)
	{
		// 添加到链表结尾
		ms_pkTail->m_pkNext = this;
		m_pkPrev = ms_pkTail;
	}
	else
	{
		m_pkPrev = NULL;
	}

	ms_pkTail = this;
	m_pkNext = NULL;
}
//-------------------------------------------------------------------------------------------------
void sdDynamicTexture::RemoveTextureFromList()
{
	if (ms_pkHead == this)	ms_pkHead = m_pkNext;
	if (ms_pkTail == this)	ms_pkTail = m_pkPrev;
	if (m_pkPrev)	m_pkPrev->m_pkNext = m_pkNext;
	if (m_pkNext)	m_pkNext->m_pkPrev = m_pkPrev;
}
//-------------------------------------------------------------------------------------------------
bool sdDynamicTexture::OnDeviceLost()
{
	// 尝试删除显存纹理
	if (m_spD3DVedTexture)
	{
		m_spD3DVedTexture->Release();
		m_spD3DVedTexture = 0;
	}

	return true;
}
//-------------------------------------------------------------------------------------------------
bool sdDynamicTexture::OnDeviceReset(bool bBefore)
{
	// 尝试删除D3D显存纹理
	if (m_spD3DVedTexture)
	{
		m_spD3DVedTexture->Release();
		m_spD3DVedTexture = 0;
	}

	//
	if (bBefore)
	{
		// 设备重设前
		//
		// 清理GB纹理的显存相关数据
		m_spRenderer->PurgeTexture(m_spTexture);
		return true;
	}
	else
	{
		// 设备已经重设
		//
		// 创建D3D显存纹理
		uint uiLevel = (m_bMipmap ? 1: 0);

		HRESULT hr = D3DXCreateTexture(m_spD3DDevice, 
			m_uiWidth, 
			m_uiHeight, 
			uiLevel, 
			D3DUSAGE_DYNAMIC,
			ms_kD3DFormatTable[m_eFormat],
			D3DPOOL_DEFAULT,
			&m_spD3DVedTexture);
		if (SUCCEEDED(hr))	
		{
			// 初始化GB纹理显存相关数据
			if (!NiDX9Direct3DTextureData::Create((NiDX9Direct3DTexture*)(NiTexture*)m_spTexture, m_spRenderer, m_spD3DVedTexture))
			{
				// 重新更新内存纹理数据到显存纹理
				if (SUCCEEDED(m_spD3DSysTexture->AddDirtyRect(NULL)))
				{
					if (SUCCEEDED(m_spD3DDevice->UpdateTexture(m_spD3DSysTexture, m_spD3DVedTexture)))
					{
						return true;
					}
				}
			}
		}

		// 出错
		NIASSERT(0);
		return false;
	}
}
//-------------------------------------------------------------------------------------------------
}