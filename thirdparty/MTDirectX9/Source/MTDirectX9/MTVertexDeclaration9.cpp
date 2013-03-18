#include "MTVertexDeclaration9.h"
#include "MTDevice9.h"
//-------------------------------------------------------------------------------------------------
extern T_CreateVertexDeclaration ST_CreateVertexDeclaration;
//-------------------------------------------------------------------------------------------------
HRESULT	STDMETHODCALLTYPE MT_IDirect3DVertexDeclaration9_QueryInterface(IDirect3DVertexDeclaration9* pThis, REFIID riid, void** ppvObj)
{
	HRESULT	hr = E_NOINTERFACE;	///< ע�����ﲻ��E_FAIL,�ο�DXSDK�ĵ�IUnKnown::QueryInterface

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
	return 1;	///< ����û��ά�����ü���, ��Ϊģ����MTRenderThread�л�ά��һ��
				///< IDirect3DVertexDeclaration9ʵ�����ڱ����ظ�����������
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
// IDirect3DVertexDeclaration9������,��C����ָ������ʽģ��C++������̳�,�������Ա˳��μ�d3d9.h
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

	p->m_pIDirect3DVertexDeclaration9 = NULL;		///< ����û�����̴���,�����ӳٵ�GetVD()����
	//T_AddRef(p->m_pIDirect3DVertexDeclaration9);

	p->m_pIDirect3DDevice9 = pIDirect3DDevice9;		///< û��������ü���,�˳�ʱҲ�����ټ���
	//T_AddRef(p->m_pIDirect3DDevice9);

	p->m_kElementVec.resize(uiNumElement);
	memcpy(&p->m_kElementVec[0], pVertexElements, sizeof(D3DVERTEXELEMENT9) * uiNumElement);

	p->lpVtbl = &MT_lpVtbl;	///< �滻����ĺ���ָ���

	return p;
}
//-------------------------------------------------------------------------------------------------