#ifndef MTVERTEXBUFFER9_HEAD_FILE
#define	MTVERTEXBUFFER9_HEAD_FILE

#include "MTResource9.h"


class MT_IDirect3DVertexBuffer9	:	public	MT_IDirect3DResource9
{
public:
    /*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj){
		return	m_pIDirect3DVertexBuffer9->QueryInterface(riid,ppvObj);
	};
    //STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    //STDMETHOD_(ULONG,Release)(THIS) PURE;

    /*** IDirect3DResource9 methods ***/
    //STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice) PURE;
    //STDMETHOD(SetPrivateData)(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags) PURE;
    //STDMETHOD(GetPrivateData)(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData) PURE;
    //STDMETHOD(FreePrivateData)(THIS_ REFGUID refguid) PURE;
    //STDMETHOD_(DWORD, SetPriority)(THIS_ DWORD PriorityNew) PURE;
    //STDMETHOD_(DWORD, GetPriority)(THIS) PURE;
    //STDMETHOD_(void, PreLoad)(THIS) PURE;
    //STDMETHOD_(D3DRESOURCETYPE, GetType)(THIS) PURE;
    STDMETHOD(Lock)(THIS_ UINT OffsetToLock,UINT SizeToLock,void** ppbData,DWORD Flags);
    STDMETHOD(Unlock)(THIS);
	STDMETHOD(GetDesc)(THIS_ D3DVERTEXBUFFER_DESC *pDesc){
		return	m_pIDirect3DVertexBuffer9->GetDesc(pDesc);
	};
public:
	MT_IDirect3DVertexBuffer9(MT_IDirect3DDevice9* pMT_IDirect3DDevice9,IDirect3DVertexBuffer9* pIDirect3DVertexBuffer9);
public:
	IDirect3DVertexBuffer9* m_pIDirect3DVertexBuffer9;
};

#endif