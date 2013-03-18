//*************************************************************************************************
// 内容: 重载的IDirect3DStateBlock9对象
//---------------------------------------------------------
// 作者:		
// 创建:		2013-03-04
// 最后修改:	2013-03-15
//*************************************************************************************************
#ifndef MTSTATEBLOCK9_HEAD_FILE
#define	MTSTATEBLOCK9_HEAD_FILE

#include "MTHeader.h"
//-------------------------------------------------------------------------------------------------
// MT_IDirect3DStateBlock9
//-------------------------------------------------------------------------------------------------
struct MT_IDirect3DStateBlock9 : IDirect3DStateBlock9
{ 
	IDirect3DDevice9*		m_pIDirect3DDevice9;
	IDirect3DStateBlock9*	m_pIDirect3DStateBlock9;
	LONG					m_uiRef;
	D3DSTATEBLOCKTYPE		m_Type;
}; 
//-------------------------------------------------------------------------------------------------
MT_IDirect3DStateBlock9* NewMT_IDirect3DStateBlock9(
	IDirect3DDevice9* pIDirect3DDevice9,			
	D3DSTATEBLOCKTYPE eType,
	IDirect3DStateBlock9* pIDirect3DStateBlock9);
//-------------------------------------------------------------------------------------------------
#endif