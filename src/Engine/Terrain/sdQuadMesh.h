//*************************************************************************************************
// ����:	����Mesh���ƶ���
//---------------------------------------------------------
// ����:		
// ����:		2012-08-01
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _TERRAIN_QUAD_MESH_H__
#define _TERRAIN_QUAD_MESH_H__
#include <sdVector3.h>
#include "sdTypes.h"

// ����QuadTree�ڵ��Mesh����
//	1.ʹ�õ���Skirt,����IndexBuffer
//	2.����GPU��VertexBuffer,�Ż����ж���
//	  ��XBOX��,����24��vertex���ݵ�Post Vertex Cache.Cache���㷨��FIFO��(PC�ϵ�Ҳ��һ����)
//	3.ʹ��TriangleStrip������
//	4.ʹ��IndexStream(����,GB?)
// (����Ԥ����MeshCache���Ż��ڴ�)
class sdQuadMesh : public NiMesh
{
public:
	sdQuadMesh(uint uiX, uint uiY, uint uiSpacing, uint uiSize, float fSkirtDepth);
	~sdQuadMesh();

	// ��̬��ʼ��
	// (������Ҫ��Ϊ�˳�ʼ�������㻺��)
	static void	StaticInitialize();
	static void StaticDestroy();

protected:
	// ��������������
	void CreateVertexStream(uint uiX, uint uiY, uint uiSpacing, uint uiSize, float fSkirtDepth, bool bAllowEdit = false);

	//
	static uint	CalcIndexNum(uint uiSize);
	static uint	CalcVertexNum(uint uiSize);

	// ��������������
	static void CreateIndexStream(uint uiSize);
	static void CreateIndexMap(uint uiSize);

protected:
	// ��ǰTileMesh�������HeightMap����ʼλ��(��ͬTile��λ�ò�ͬ:0/64/128)
	uint	m_uiX;
	uint	m_uiY;

	// ��ǰTileMesh�����ű���(��ͬLOD���ܶȲ�ͬ��:1/2/4/8/16)
	uint	m_uiSpacing;

	// ����������
	NiDataStreamPtr			m_spVertexStream;

	// ��������
	NiFloatsExtraDataPtr	m_spExtraData;

protected:
	// GPU���������ߴ�(Post T&L Cache, ���洦����Ķ���)
	static	uint ms_uiGPUPostVertexCacheSize;

	// ����Ķ�������
	static	NiDataStreamPtr	ms_spIndexStream;

	// ����
	static	std::vector<Base::Math::sdVector3us>	ms_kIndexMap;
	static	std::vector<short>						ms_kVertexBuf;
};
NiSmartPointer(sdQuadMesh);
#endif