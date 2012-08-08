#include "Stdafx.h"
#include "sdQuadNode.h"
#include "sdTerrain.h"

//-------------------------------------------------------------------------------------------------
sdQuadNode::sdQuadNode()
: m_pkQuadNodePar(NULL)
, m_uiLevel(0)
, m_uiSpacing(1)
, m_fError(0.0f)
, m_fAccumError(0.0f)
{

}
//-------------------------------------------------------------------------------------------------
sdQuadNode::~sdQuadNode()
{
	
}
//-------------------------------------------------------------------------------------------------
bool sdQuadNode::Build(Base::Math::sdVector4ui& kRect, sdQuadNode* pkParent)
{
	using namespace Base::Math;

	//
	sdTerrain* pkTerrain = sdTerrain::InstancePtr();
	NIASSERT(pkTerrain);

	// ���ڵ�
	m_pkQuadNodePar = pkParent;

	// ��ʼ��
	m_kOrigin.m_kX	= kRect.m_kX;
	m_kOrigin.m_kY	= kRect.m_kY;

	// �ݹ鴦���ӽڵ�(ע�����ȵݹ�,�ٴ���ڵ�)
	// @{
	uint uiTileSize = pkTerrain->GetTileSize();
	if (kRect.m_kZ - kRect.m_kX != uiTileSize)
	{
		// ��ǰ�ڵ�Ϊ�м�ڵ�
		//
		// �ݹ��������
		sdVector4ui kChildRect;
		sdVector2ui kCenter((kRect.m_kX + kRect.m_kZ) / 2, (kRect.m_kY + kRect.m_kW) / 2);

		kChildRect.m_kX = kRect.m_kX;
		kChildRect.m_kY = kCenter.m_kY;
		kChildRect.m_kZ = kCenter.m_kX;
		kChildRect.m_kW = kRect.m_kW;
		m_pkQuadNodeChild[E_LT_CHILD] = NiNew sdQuadNode;
		m_pkQuadNodeChild[E_LT_CHILD]->Build(kChildRect, this);

		kChildRect.m_kX = kCenter.m_kX;
		kChildRect.m_kY = kCenter.m_kY;
		kChildRect.m_kZ = kRect.m_kZ;
		kChildRect.m_kW = kRect.m_kW;
		m_pkQuadNodeChild[E_RT_CHILD] = NiNew sdQuadNode;
		m_pkQuadNodeChild[E_RT_CHILD]->Build(kChildRect, this);

		kChildRect.m_kX = kRect.m_kX;
		kChildRect.m_kY = kRect.m_kY;
		kChildRect.m_kZ = kCenter.m_kX;
		kChildRect.m_kW = kCenter.m_kY;
		m_pkQuadNodeChild[E_LB_CHILD] = NiNew sdQuadNode;
		m_pkQuadNodeChild[E_LB_CHILD]->Build(kChildRect, this);

		kChildRect.m_kX = kCenter.m_kX;
		kChildRect.m_kY = kRect.m_kY;
		kChildRect.m_kZ = kRect.m_kZ;
		kChildRect.m_kW = kCenter.m_kY;
		m_pkQuadNodeChild[E_RB_CHILD] = NiNew sdQuadNode;
		m_pkQuadNodeChild[E_RB_CHILD]->Build(kChildRect, this);

		// �ڵ�㼶(�������ϵ���+1)
		m_uiLevel = m_pkQuadNodeChild[E_LT_CHILD]->GetLevel() + 1;

		// �ڵ�������(�������ϵ���*2)
		m_uiSpacing = m_pkQuadNodeChild[E_LT_CHILD]->GetSpacing() * 2;

		// �ڵ�������ۻ����(���������ۻ�,�ӽڵ������ۻ����+���ڵ�����ӽڵ����)
		m_fError = CalcError();

		m_fAccumError = 0.0f;
		m_fAccumError = max(m_pkQuadNodeChild[E_LT_CHILD]->GetAccumError(), m_fAccumError);
		m_fAccumError = max(m_pkQuadNodeChild[E_RT_CHILD]->GetAccumError(), m_fAccumError);
		m_fAccumError = max(m_pkQuadNodeChild[E_LB_CHILD]->GetAccumError(), m_fAccumError);
		m_fAccumError = max(m_pkQuadNodeChild[E_RB_CHILD]->GetAccumError(), m_fAccumError);
		m_fAccumError += m_fError;


		// �ڵ��Χ��(�������Ϻϲ�)
		CalcBound(m_kAABB);
		m_kAABB.Merge(m_pkQuadNodeChild[E_LT_CHILD]->GetBound());
		m_kAABB.Merge(m_pkQuadNodeChild[E_RT_CHILD]->GetBound());
		m_kAABB.Merge(m_pkQuadNodeChild[E_LB_CHILD]->GetBound());
		m_kAABB.Merge(m_pkQuadNodeChild[E_RB_CHILD]->GetBound());
	}
	else
	{
		// ��ǰ�ڵ�ΪҶ�ӽڵ�
		//
		// �ڵ�㼶
		m_uiLevel = 0;	

		// �ڵ�������
		m_uiSpacing = 1;

		// �ڵ����
		m_fError = 0.0f;
		m_fAccumError = 0.0f;

		// ����ڵ��Χ��
		CalcBound(m_kAABB);
	}
	// @}

	// 
	uint uiMeshLevel = pkTerrain->GetMeshLevel();
	if (m_uiLevel < uiMeshLevel)
	{
		// ������ǰNode��Mesh
		m_pkQuadMesh = NiNew sdQuadMesh(m_kOrigin.m_kX, m_kOrigin.m_kY, m_uiSpacing, uiTileSize, m_fError);
		NIASSERT(m_pkQuadMesh);
	}

	return true;
}
//-------------------------------------------------------------------------------------------------
void sdQuadNode::Destroy()
{

}
//-------------------------------------------------------------------------------------------------
void sdQuadNode::Cull(const NiCamera& kCamera, NiFrustumPlanes& kFrustumPlanes, std::vector<NiMesh*>& kMeshVec)
{
	using namespace Base::Math;

	// �Ե�ǰ�ڵ���вü�,���
	//	1.�Ƿ�ɼ�
	//	2.�Ƿ�������±���

	// 
	sdTerrain* pkTerrain = sdTerrain::InstancePtr();
	NIASSERT(pkTerrain);

	// ��Χ��
	sdVector3 kCenter, kHalfSize;
	m_kAABB.GetCenter(kCenter);
	m_kAABB.GetSize(kHalfSize);

	NiPoint3 kTCenter(kCenter.m_fX, kCenter.m_fY, kCenter.m_fZ);
	NiPoint3 kTHalfSize(kHalfSize.m_fX, kHalfSize.m_fY, kHalfSize.m_fZ);

	// ����ü�(�ο���Ogre)
	//
	// �������������������вü�,ֻ�о����ڸ�����ı�����п��ܿɼ�
	// (������һЩ�����ܵ�AABB��Frustum���㷨)
	// @{
	bool bVisible = false;
	//////////uint uiPlaneCount = 0;
	//////////for (; uiPlaneCount < NiFrustumPlanes::MAX_PLANES; uiPlaneCount++)
	//////////{
	//////////	if (kFrustumPlanes.IsPlaneActive(uiPlaneCount))
	//////////	{
	//////////		const NiPlane& kPlane = kFrustumPlanes.GetPlane(uiPlaneCount);
	//////////		
	//////////		// Calculate the distance between box centre and the plane
	//////////		float fDistance = kPlane.Distance(kTCenter);

	//////////		// Calculate the maximise allows absolute distance for
	//////////		// the distance between box centre and plane
	//////////		const NiPoint3& kNormal = kPlane.GetNormal();
	//////////		float fMaxAbsDistance = fabsf(kNormal.x * kHalfSize.m_fX) + 
	//////////			fabsf(kNormal.y * kHalfSize.m_fY) +	fabsf(kNormal.z * kHalfSize.m_fZ);

	//////////		// ֻҪ��Χ��λ����һƽ�渺��,ֱ���޳�
	//////////		if (fDistance < -fMaxAbsDistance)
	//////////		{
	//////////			// NiPlane::NEGATIVE_SIDE;
	//////////			break;
	//////////		}
	//////////		//else if (fDistance > fMaxAbsDistance)
	//////////		//{
	//////////		//	// NiPlane::POSITIVE_SIDE
	//////////		//}
	//////////		//else
	//////////		//{
	//////////		//	//NiPlane::NO_SIDE;
	//////////		//}
	//////////	}
	//////////}

	//////////if (uiPlaneCount == NiFrustumPlanes::MAX_PLANES)
		bVisible = true;
	// @}


	// �����Ƿ���Ҫ��һ�����±���
	// @{
	bool bShouldSubdivide = false;
	if (bVisible)
	{
		if (!pkTerrain->GetEnableLOD() || m_uiLevel < pkTerrain->GetMeshLevel())
		{
			// 1.���ν���LOD
			// 2.��ǰ�㼶û������Mesh
			bShouldSubdivide = true;
		}
		else
		{
			// 1.����ڵ��Χ�е��ӵ�ľ���
			// 2.���ݽڵ��ۻ�����������۲����
			// 3.�Ƚ���������ȷ���Ƿ�ϸ��
			//********************
			bShouldSubdivide = true;
			//********************
		}
	}
	// @}


	// ����
	// @{
	if (bVisible)
	{
		if (bShouldSubdivide)
		{
			// ���±���
			if (IsLeaf())
			{
				// Ҷ�ӽڵ�ֱ����ȡMesh
				if (m_pkQuadMesh)
					kMeshVec.push_back(m_pkQuadMesh);
			}
			else
			{
				// �м�ڵ�,���մӽ���Զ�Ĵ������δ����ӽڵ�
				//
				// �����ӽڵ����ĵ��۲�����
				float vfDistance[NUM_CHILD_TYPE] =  { 0 };

				sdVector3 kCamPosition(kCamera.GetWorldTranslate().x, kCamera.GetWorldTranslate().y, kCamera.GetWorldTranslate().z);
				for (int i = 0; i < NUM_CHILD_TYPE; ++i)
				{
					vfDistance[i] = (m_pkQuadNodeChild[i]->GetBound().GetCenter() - kCamPosition).SquaredLength();
				}

				// ÿ��ȡ��������������Сֵ��Node����, �����������Ϊ�����
				for (int i = 0; i < NUM_CHILD_TYPE; ++i)
				{
					float fMinDistance = vfDistance[0];
					int iMinChild = 0;

					for (int j = 1; j < NUM_CHILD_TYPE; ++j)
					{
						if (vfDistance[j] < fMinDistance)
						{
							fMinDistance = vfDistance[j];
							iMinChild = j;
						}
					}
					vfDistance[iMinChild] = sdMath::POS_INFINITY;

					m_pkQuadNodeChild[iMinChild]->Cull(kCamera, kFrustumPlanes, kMeshVec);
				}
				
			}
		}
		else
		{
			// ֹͣ������ȡMesh
			if (m_pkQuadMesh)
				kMeshVec.push_back(m_pkQuadMesh);
		}
	}
	// @}
}
//-------------------------------------------------------------------------------------------------
float sdQuadNode::CalcError()
{
	sdTerrain* pkTerrain = sdTerrain::InstancePtr();
	NIASSERT(pkTerrain);

	sdHeightMap* pkHeightMap = pkTerrain->GetHeightMap();
	NIASSERT(pkHeightMap);

	// Ҷ�ӽڵ������
	if (m_uiLevel == 0)
		return 0.0f;

	float fError = 0.0f;
	uint uiSize = pkTerrain->GetTileSize();
	uint uiStartX = m_kOrigin.m_kX;
	uint uiStartY = m_kOrigin.m_kY;
	uint uiSpacing = m_uiSpacing;
	for (uint uiY = 0; uiY <= uiSize; ++uiY)
	{
		for (uint uiX = 0; uiX <= uiSize; ++uiX)
		{
			float fCalcError = pkHeightMap->GetMorphDelta(uiStartX, uiStartY, uiSpacing);
			fError = max(fError, fabsf(fCalcError));

			uiStartX += uiSpacing;
		}
	
		uiStartY += uiSpacing;
	}

	return fError;
}
//-------------------------------------------------------------------------------------------------
void sdQuadNode::CalcBound(Base::Math::sdAxisAlignedBox& kAABB)
{
	sdTerrain* pkTerrain = sdTerrain::InstancePtr();
	NIASSERT(pkTerrain);

	sdHeightMap* pkHeightMap = pkTerrain->GetHeightMap();
	NIASSERT(pkHeightMap);


	uint uiSize = pkTerrain->GetTileSize();
	uint uiStartX = m_kOrigin.m_kX;
	uint uiStartY = m_kOrigin.m_kY;
	uint uiSpacing = m_uiSpacing;

	//
	float fHeight = pkHeightMap->GetRawHeight(uiStartX, uiStartY);
	float fMinHeight = fHeight;
	float fMaxHeight = fHeight;

	for (uint uiY = 0; uiY <= uiSize; ++uiY)
	{
		for (uint uiX = 0; uiX <= uiSize; ++uiX)
		{
			fHeight = pkHeightMap->GetRawHeight(uiStartX, uiStartY);
			if (fHeight < fMinHeight)	
				fMinHeight = fHeight;
			else if (fHeight > fMaxHeight)	
				fMaxHeight = fHeight;
				

			uiStartX += uiSpacing;
		}

		uiStartY += uiSpacing;
	}

	kAABB.SetMinimumX((float)m_kOrigin.m_kX);
	kAABB.SetMinimumY((float)m_kOrigin.m_kY);
	kAABB.SetMinimumZ(fMinHeight);
	kAABB.SetMaximumX((float)(m_kOrigin.m_kX + uiSize * uiSpacing));
	kAABB.SetMaximumY((float)(m_kOrigin.m_kY + uiSize * uiSpacing));
	kAABB.SetMaximumZ(fMaxHeight);
}
//-------------------------------------------------------------------------------------------------