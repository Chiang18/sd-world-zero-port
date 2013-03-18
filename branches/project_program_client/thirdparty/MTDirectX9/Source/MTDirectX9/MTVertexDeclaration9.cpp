#include "MTVertexDeclaration9.h"
#include "MTDevice9.h"
//-------------------------------------------------------------------------------------------------
extern T_CreateVertexDeclaration ST_CreateVertexDeclaration;
//-------------------------------------------------------------------------------------------------
HRESULT	STDMETHODCALLTYPE MT_IDirect3DVertexDeclaration9_QueryInterface(IDirect3DVertexDeclaration9* pThis, REFIID riid, void** ppvObj)
{
	HRESULT	hr = E_NOINTERFACE;	///< 注意这里不是E_FAIL,参看DXSDK文档IUnKnown::QueryInterface

	MT_IDirect3DVertexDeclaration9* p = static_cast<MT_IDirect3DVertexDeclaration9*>(pThis);
	if (p->m_pIDirect3DVertexDeclaration9!=NULL)
	{
		hr = p->m_pIDirect3DVertexDeclaration9->lpVtbl->QueryInterface(p->m_pIDirect3DVertexDeclaration9, riid, ppvObj);
		if (SUCCEEDED(hr))
		{
			*ppvObj	= p;
		}
	}
	return hr;
}
//-------------------------------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE	MT_IDirect3DVertexDeclaration9_AddRef(IDirect3DVertexDeclaration9* pThis)
{
	return 2;	///< ::interlockedIncrement(&p->m_uiRef);
};
//-------------------------------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE	MT_IDirect3DVertexDeclaration9_Release(IDirect3DVertexDeclaration9* pThis)
{
	return 1;	///< 这里没有维护引用计数, 因为模块在MTRenderThread中会维护一组
				///< IDirect3DVertexDeclaration9实例用于避免重复创建和重用
};
//-------------------------------------------------------------------------------------------------
HRESULT	STDMETHODCALLTYPE MT_IDirect3DVertexDeclaration9_GetDevice(IDirect3DVertexDeclaration9* pThis, IDirect3DDevice9** ppDevice)
{
	MT_IDirect3DVertexDeclaration9* p =	static_cast<MT_IDirect3DVertexDeclaration9*>(pThis);
	*ppDevice = p->m_pIDirect3DDevice9;
 	T_AddRef(p->m_pIDirect3DDevice9);
	return S_OK;
};
//-------------------------------------------------------------------------------------------------
HRESULT	STDMETHODCALLTYPE MT_IDirect3DVertexDeclaration9_GetDeclaration(IDirect3DVertexDeclaration9* pThis, D3DVERTEXELEMENT9* pElement, UINT* pNumElements)
{
	MT_IDirect3DVertexDeclaration9* p = static_cast<MT_IDirect3DVertexDeclaration9*>(pThis);
	*pNumElements = p->m_kElementVec.size();
	if (pElement != NULL)
	{
		memcpy(pElement, &p->m_kElementVec[0], sizeof(D3DVERTEXELEMENT9) * (*pNumElements));
	}
	return S_OK;
};
//-------------------------------------------------------------------------------------------------
IDirect3DVertexDeclaration9* MT_IDirect3DVertexDeclaration9::GetVD()
{
	if (m_pIDirect3DVertexDeclaration9 == NULL)
	{
		HRESULT	hr = ST_CreateVertexDeclaration(m_pIDirect3DDevice9, &m_kElementVec[0], &m_pIDirect3DVertexDeclaration9);
	}
	return	m_pIDirect3DVertexDeclaration9;
}
//-------------------------------------------------------------------------------------------------
// IDirect3DVertexDeclaration9函数表,以C函数指针表的形式模拟C++类与类继承,函数表成员顺序参见d3d9.h
//-------------------------------------------------------------------------------------------------
IDirect3DVertexDeclaration9Vtbl	MT_lpVtbl =
{
	MT_IDirect3DVertexDeclaration9_QueryInterface,
	MT_IDirect3DVertexDeclaration9_AddRef,
	MT_IDirect3DVertexDeclaration9_Release,
	MT_IDirect3DVertexDeclaration9_GetDevice,
	MT_IDirect3DVertexDeclaration9_GetDeclaration
};
//-------------------------------------------------------------------------------------------------
MT_IDirect3DVertexDeclaration9*	NewMT_IDirect3DVertexDeclaration9(IDirect3DDevice9* pIDirect3DDevice9, CONST D3DVERTEXELEMENT9* pVertexElements, UINT uiNumElement)
{
	if (pVertexElements	== NULL)
		return	NULL;

	MT_IDirect3DVertexDeclaration9* p = new MT_IDirect3DVertexDeclaration9;

	p->m_pIDirect3DVertexDeclaration9 = NULL;		///< 这里没有立刻创建,而是延迟到GetVD()函数
	//T_AddRef(p->m_pIDirect3DVertexDeclaration9);

	p->m_pIDirect3DDevice9 = pIDirect3DDevice9;		///< 没有添加引用计数,退出时也不减少计数
	//T_AddRef(p->m_pIDirect3DDevice9);

	p->m_kElementVec.resize(uiNumElement);
	memcpy(&p->m_kElementVec[0], pVertexElements, sizeof(D3DVERTEXELEMENT9) * uiNumElement);

	p->lpVtbl = &MT_lpVtbl;	///< 替换本类的函数指针表

	return p;
}
//-------------------------------------------------------------------------------------------------