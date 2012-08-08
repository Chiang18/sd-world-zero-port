//*************************************************************************************************
// ����:	����Mesh���ƶ���
//---------------------------------------------------------
// ����:		
// ����:		2012-08-01
// ����޸�:
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

// ����QuadTree�ڵ�,ÿ���ڵ�ֻ��һ��Mesh,��ʱľ��֧�ֽڵ���LOD
// (����Ԥ����ڵ����Ż��ڴ�)
// (���ǵ��Ժ�ʵʩ���޵�ͼ��Ҫ����Ĳ���,������ò�Ҫ������ص��ྲ̬��Ա����)
// (�����Ժ�ʵ��ÿ���ڵ��ڶ༶LOD)
class sdQuadNode : public NiRefObject
{
public:
	// �Ĳ����ڵ�ö��
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

	// �Ӹ߶�ͼ�����Ĳ���,��ָ����Χ����ϸ��,ֱ���ߴ����QuadSize
	// (Ϊ�����Ч��,����û�м���������)
	bool	Build(Base::Math::sdVector4ui& kRect, sdQuadNode* pkParent);

	// 
	void	Destroy();

	// ��ָ������ü��Ĳ���
	void	Cull(const NiCamera& kCamera, NiFrustumPlanes& kFrustumPlanes,std::vector<NiMesh*>& kMeshVec);

	//
	bool	IsLeaf() const { return (m_uiLevel == 0);};
	uint	GetLevel() const { return m_uiLevel;};
	uint	GetSpacing() const { return m_uiSpacing;};
	float	GetAccumError() const { return m_fAccumError;};
	
	const Base::Math::sdAxisAlignedBox& GetBound() const { return m_kAABB;};

protected:

	// ���㵱ǰ�ڵ�����һ�ڵ�����߶�ͼ֮������������ֵ
	float	CalcError();

	// ���㵱ǰ�ڵ�߶�ͼ�İ�Χ��
	void	CalcBound(Base::Math::sdAxisAlignedBox& kAABB);

protected:
	// ���ڵ��뺢�ӽڵ�(ע����⻥������)
	sdQuadNodePtr	m_pkQuadNodeChild[NUM_CHILD_TYPE];
	sdQuadNode*		m_pkQuadNodePar;
	
	// ��ǰ�ڵ��Mesh
	sdQuadMeshPtr	m_pkQuadMesh;

	uint			m_uiLevel;		// ��ǰ�ڵ����(Ҷ�ڵ�Ϊ0,���ϵ���,���ڵ����)
	uint			m_uiSpacing;	// ��ǰ�ڵ�դ��ߴ�(Ҷ�ڵ�Ϊ1,���ϼ��μ�������)
	float			m_fAccumError;	// ��ǰ�ڵ��ۻ�������(Ҷ�ڵ�Ϊ0,�����ۻ�)
	float			m_fError;		// ��ǰ�ڵ�����һ�ڵ�������,��Ϊ����Tileȹ�Ӹ߶�

	Base::Math::sdVector2ui			m_kOrigin;	// ��ǰ�ڵ���ʼ��λ��
	Base::Math::sdAxisAlignedBox	m_kAABB;	// ��ǰ�ڵ��Χ��(�ýڵ����������Ĳ�����Ӧ�߶�ͼ�İ�Χ��)
};
#endif