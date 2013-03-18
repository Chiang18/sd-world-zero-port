//*************************************************************************************************
// 内容: 重载的IDirect3DVertexDeclaration9对象
//---------------------------------------------------------
// 作者:		
// 创建:		2013-03-04
// 最后修改:	2013-03-15
//*************************************************************************************************
#ifndef	MTVERTEXDECLARATION9_HEAD_FILE
#define MTVERTEXDECLARATION9_HEAD_FILE

#include "MTHeader.h"
//-------------------------------------------------------------------------------------------------
// MT_IDirect3DVertexDeclaration9
//-------------------------------------------------------------------------------------------------
struct MT_IDirect3DVertexDeclaration9 : IDirect3DVertexDeclaration9
{
	IDirect3DVertexDeclaration9*		GetVD();

	IDirect3DDevice9*					m_pIDirect3DDevice9;
	IDirect3DVertexDeclaration9*		m_pIDirect3DVertexDeclaration9;
	std::vector<D3DVERTEXELEMENT9>		m_kElementVec;
};
//-------------------------------------------------------------------------------------------------
MT_IDirect3DVertexDeclaration9*	NewMT_IDirect3DVertexDeclaration9(
	IDirect3DDevice9* pIDirect3DDevice9,
	CONST D3DVERTEXELEMENT9* pVertexElements,
	UINT uiNumElement);
//-------------------------------------------------------------------------------------------------
#endif