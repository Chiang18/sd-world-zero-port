//*************************************************************************************************
// 内容:	地形Mesh绘制对象
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-01
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _TERRAIN_QUAD_MESH_H__
#define _TERRAIN_QUAD_MESH_H__
#include <sdVector3.h>
#include "sdTypes.h"

// 地形QuadTree节点的Mesh对象
//	1.使用地形Skirt,共享IndexBuffer
//	2.根据GPU的VertexBuffer,优化排列顶点
//	  在XBOX中,保存24个vertex数据的Post Vertex Cache.Cache的算法是FIFO的(PC上的也是一样的)
//	3.使用TriangleStrip来绘制
//	4.使用IndexStream(不解,GB?)
// (考虑预分配MeshCache以优化内存)
class sdQuadMesh : public NiMesh
{
public:
	sdQuadMesh(uint uiX, uint uiY, uint uiSpacing, uint uiSize, float fSkirtDepth);
	~sdQuadMesh();

	// 静态初始化
	// (这里主要是为了初始化共享顶点缓存)
	static void	StaticInitialize();
	static void StaticDestroy();

protected:
	// 创建顶点数据流
	void CreateVertexStream(uint uiX, uint uiY, uint uiSpacing, uint uiSize, float fSkirtDepth, bool bAllowEdit = false);

	//
	static uint	CalcIndexNum(uint uiSize);
	static uint	CalcVertexNum(uint uiSize);

	// 创建索引数据流
	static void CreateIndexStream(uint uiSize);
	static void CreateIndexMap(uint uiSize);

protected:
	// 当前TileMesh相对整个HeightMap的起始位置(不同Tile的位置不同:0/64/128)
	uint	m_uiX;
	uint	m_uiY;

	// 当前TileMesh的缩放比例(不同LOD的密度不同的:1/2/4/8/16)
	uint	m_uiSpacing;

	// 顶点数据流
	NiDataStreamPtr			m_spVertexStream;

	// 额外数据
	NiFloatsExtraDataPtr	m_spExtraData;

protected:
	// GPU顶点后处理缓存尺寸(Post T&L Cache, 保存处理过的顶点)
	static	uint ms_uiGPUPostVertexCacheSize;

	// 共享的顶点索引
	static	NiDataStreamPtr	ms_spIndexStream;

	// 缓存
	static	std::vector<Base::Math::sdVector3us>	ms_kIndexMap;
	static	std::vector<short>						ms_kVertexBuf;
};
NiSmartPointer(sdQuadMesh);
#endif