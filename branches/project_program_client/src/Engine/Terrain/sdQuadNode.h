//*************************************************************************************************
// 内容:	地形Mesh绘制对象
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-01
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _TERRAIN_QUAD_NODE_H__
#define _TERRAIN_QUAD_NODE_H__
#include <sdVector2.h>
#include <sdVector4.h>
#include <sdAxisAlignedBox.h>
#include "sdTypes.h"
#include "sdQuadMesh.h"

class sdQuadNode;
NiSmartPointer(sdQuadNode);

// 地形QuadTree节点,每个节点只有一个Mesh,暂时木有支持节点内LOD
// (考虑预分配节点以优化内存)
// (考虑到以后实施无限地图需要多棵四叉树,所以最好不要有树相关的类静态成员变量)
// (考虑以后实现每个节点内多级LOD)
class sdQuadNode : public NiRefObject
{
public:
	// 四叉树节点枚举
	enum eQuadNodeIndex
	{
		E_LT_CHILD = 0,
		E_RT_CHILD,
		E_LB_CHILD,
		E_RB_CHILD,
		NUM_CHILD_TYPE,
	};

	sdQuadNode();
	~sdQuadNode();

	// 从高度图构建四叉树,从指定范围向下细分,直到尺寸等于QuadSize
	// (为了提高效率,这里没有检查输入参数)
	bool	Build(Base::Math::sdVector4ui& kRect, sdQuadNode* pkParent);

	// 
	void	Destroy();

	// 用指定相机裁剪四叉树
	void	Cull(const NiCamera& kCamera, NiFrustumPlanes& kFrustumPlanes,std::vector<NiMesh*>& kMeshVec);

	//
	bool	IsLeaf() const { return (m_uiLevel == 0);};
	uint	GetLevel() const { return m_uiLevel;};
	uint	GetSpacing() const { return m_uiSpacing;};
	float	GetAccumError() const { return m_fAccumError;};
	
	const Base::Math::sdAxisAlignedBox& GetBound() const { return m_kAABB;};

protected:

	// 计算当前节点与下一节点采样高度图之间的最大误差绝对值
	float	CalcError();

	// 计算当前节点高度图的包围盒
	void	CalcBound(Base::Math::sdAxisAlignedBox& kAABB);

protected:
	// 父节点与孩子节点(注意避免互相引用)
	sdQuadNodePtr	m_pkQuadNodeChild[NUM_CHILD_TYPE];
	sdQuadNode*		m_pkQuadNodePar;
	
	// 当前节点的Mesh
	sdQuadMeshPtr	m_pkQuadMesh;

	uint			m_uiLevel;		// 当前节点深度(叶节点为0,向上递增,根节点最大)
	uint			m_uiSpacing;	// 当前节点栅格尺寸(叶节点为1,向上几何级数增加)
	float			m_fAccumError;	// 当前节点累积最大误差(叶节点为0,向上累积)
	float			m_fError;		// 当前节点与下一节点最大误差,作为地形Tile裙子高度

	Base::Math::sdVector2ui			m_kOrigin;	// 当前节点起始点位置
	Base::Math::sdAxisAlignedBox	m_kAABB;	// 当前节点包围盒(该节点往下整个四叉树对应高度图的包围盒)
};
#endif