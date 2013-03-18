#include "MTStateBlock9.h"
#include "MTDevice9.h"
#include "MTRenderThread.h"

//-------------------------------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE MT_IDirect3DStateBlock9_QueryInterface(IDirect3DStateBlock9* pThis, REFIID riid, void** ppvObj)
{
	HRESULT	hr = E_NOINTERFACE;	///< 注意这里不是E_FAIL,参看DXSDK文档IUnKnown::QueryInterface

	MT_IDirect3DStateBlock9* p	= static_cast<MT_IDirect3DStateBlock9*>(pThis);
	if (p->m_pIDirect3DStateBlock9 != NULL)
	{
		hr = p->m_pIDirect3DStateBlock9->lpVtbl->QueryInterface(p->m_pIDirect3DStateBlock9, riid, ppvObj);
		if (SUCCEEDED(hr))
		{
			*ppvObj	= p;
		}
	}
	return hr;
};
//-------------------------------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE	MT_IDirect3DStateBlock9_AddRef(IDirect3DStateBlock9* pThis)
{
	MT_IDirect3DStateBlock9* p = static_cast<MT_IDirect3DStateBlock9*>(pThis);
	return ::InterlockedIncrement(&p->m_uiRef);
};
//-------------------------------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE	MT_IDirect3DStateBlock9_Release(IDirect3DStateBlock9* pThis)
{
	MT_IDirect3DStateBlock9* p = static_cast<MT_IDirect3DStateBlock9*>(pThis);
	LONG ret = ::InterlockedDecrement(&p->m_uiRef);
	if (ret <=0 )
	{
		//T_Release(p->m_pIDirect3DStateBlock9);
		//T_Release(p->m_pIDirect3DDevice9);
		//delete p;

		ReleaseObj	obj;
		obj.func	= enCF_StateBlock_Release;
		obj.pObj	= p;
		obj.pFunc	= NULL;

		MT_RenderThread::GetSingleton().GetDBuffer().PushReleaseObj(obj);
	}

	return	ret;
};
//-------------------------------------------------------------------------------------------------
HRESULT	STDMETHODCALLTYPE MT_IDirect3DStateBlock9_GetDevice(IDirect3DStateBlock9* pThis, IDirect3DDevice9** ppDevice)
{
	MT_IDirect3DStateBlock9* p = static_cast<MT_IDirect3DStateBlock9*>(pThis);
	*ppDevice =	p->m_pIDirect3DDevice9;
 	T_AddRef(p->m_pIDirect3DDevice9);
	return S_OK;
};
//-------------------------------------------------------------------------------------------------
HRESULT	STDMETHODCALLTYPE MT_IDirect3DStateBlock9_Capture(IDirect3DStateBlock9* pThis)
{
	CmdStateBlock_Capture* param = MT_RenderThread::GetSingleton().GetDBuffer().Request<CmdStateBlock_Capture>(enCF_StateBlock_Capture);
	*param = pThis;
	//T_AddRef(pThis);
	return S_OK;
};
//-------------------------------------------------------------------------------------------------
HRESULT	STDMETHODCALLTYPE MT_IDirect3DStateBlock9_Apply(IDirect3DStateBlock9* pThis){
	CmdStateBlock_Apply* param = MT_RenderThread::GetSingleton().GetDBuffer().Request<CmdStateBlock_Apply>(enCF_StateBlock_Apply);
	*param = pThis;
	//T_AddRef(pThis);
	return S_OK;
};
//-------------------------------------------------------------------------------------------------
// IDirect3DStateBlock9函数表,以C函数指针表的形式模拟C++类与类继承
//-------------------------------------------------------------------------------------------------
static	IDirect3DStateBlock9Vtbl MT_lpVtbl	=	
{
	MT_IDirect3DStateBlock9_QueryInterface,
	MT_IDirect3DStateBlock9_AddRef,
	MT_IDirect3DStateBlock9_Release,
	MT_IDirect3DStateBlock9_GetDevice,
	MT_IDirect3DStateBlock9_Capture,
	MT_IDirect3DStateBlock9_Apply,
};
//-------------------------------------------------------------------------------------------------
//IDirect3DStateBlock9Vtbl
//MT_lpVtbl.QueryInterface	= MT_IDirect3DStateBlock9_QueryInterface;
//MT_lpVtbl.AddRef			= MT_IDirect3DStateBlock9_AddRef;
//MT_lpVtbl.Release			= MT_IDirect3DStateBlock9_Release;
//MT_lpVtbl.GetDevice		= MT_IDirect3DStateBlock9_GetDevice;
//MT_lpVtbl.Capture			= MT_IDirect3DStateBlock9_Capture;
//MT_lpVtbl.Apply			= MT_IDirect3DStateBlock9_Apply;
//-------------------------------------------------------------------------------------------------
MT_IDirect3DStateBlock9* NewMT_IDirect3DStateBlock9(IDirect3DDevice9* pIDirect3DDevice9, D3DSTATEBLOCKTYPE eType, IDirect3DStateBlock9* pIDirect3DStateBlock9)
{
	MT_IDirect3DStateBlock9* p = new MT_IDirect3DStateBlock9;
	p->m_uiRef	= 1;
	p->m_Type	= eType;

	p->m_pIDirect3DStateBlock9 = pIDirect3DStateBlock9;
	//T_AddRef(p->m_pIDirect3DStateBlock9);

	p->m_pIDirect3DDevice9 = pIDirect3DDevice9;
	//T_AddRef(p->m_pIDirect3DDevice9);

	//IDirect3DStateBlock9* pState = p;
	p->lpVtbl = &MT_lpVtbl;	///< 替换本类的函数指针表

	return p;
};
//-------------------------------------------------------------------------------------------------