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

	// 父节点
	m_pkQuadNodePar = pkParent;

	// 起始点
	m_kOrigin.m_kX	= kRect.m_kX;
	m_kOrigin.m_kY	= kRect.m_kY;

	// 递归处理子节点(注意是先递归,再处理节点)
	// @{
	uint uiTileSize = pkTerrain->GetTileSize();
	if (kRect.m_kZ - kRect.m_kX != uiTileSize)
	{
		// 当前节点为中间节点
		//
		// 递归调用子树
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

		// 节点层级(从下往上递增+1)
		m_uiLevel = m_pkQuadNodeChild[E_LT_CHILD]->GetLevel() + 1;

		// 节点采样间距(从下往上递增*2)
		m_uiSpacing = m_pkQuadNodeChild[E_LT_CHILD]->GetSpacing() * 2;

		// 节点误差与累积误差(从下往上累积,子节点的最大累积误差+本节点相对子节点误差)
		m_fError = CalcError();

		m_fAccumError = 0.0f;
		m_fAccumError = max(m_pkQuadNodeChild[E_LT_CHILD]->GetAccumError(), m_fAccumError);
		m_fAccumError = max(m_pkQuadNodeChild[E_RT_CHILD]->GetAccumError(), m_fAccumError);
		m_fAccumError = max(m_pkQuadNodeChild[E_LB_CHILD]->GetAccumError(), m_fAccumError);
		m_fAccumError = max(m_pkQuadNodeChild[E_RB_CHILD]->GetAccumError(), m_fAccumError);
		m_fAccumError += m_fError;


		// 节点包围盒(从下往上合并)
		CalcBound(m_kAABB);
		m_kAABB.Merge(m_pkQuadNodeChild[E_LT_CHILD]->GetBound());
		m_kAABB.Merge(m_pkQuadNodeChild[E_RT_CHILD]->GetBound());
		m_kAABB.Merge(m_pkQuadNodeChild[E_LB_CHILD]->GetBound());
		m_kAABB.Merge(m_pkQuadNodeChild[E_RB_CHILD]->GetBound());
	}
	else
	{
		// 当前节点为叶子节点
		//
		// 节点层级
		m_uiLevel = 0;	

		// 节点采样间距
		m_uiSpacing = 1;

		// 节点误差
		m_fError = 0.0f;
		m_fAccumError = 0.0f;

		// 计算节点包围盒
		CalcBound(m_kAABB);
	}
	// @}

	// 
	uint uiMeshLevel = pkTerrain->GetMeshLevel();
	if (m_uiLevel < uiMeshLevel)
	{
		// 构建当前Node的Mesh
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

	// 对当前节点进行裁剪,检测
	//	1.是否可见
	//	2.是否继续向下遍历

	// 
	sdTerrain* pkTerrain = sdTerrain::InstancePtr();
	NIASSERT(pkTerrain);

	// 包围盒
	sdVector3 kCenter, kHalfSize;
	m_kAABB.GetCenter(kCenter);
	m_kAABB.GetSize(kHalfSize);

	NiPoint3 kTCenter(kCenter.m_fX, kCenter.m_fY, kCenter.m_fZ);
	NiPoint3 kTHalfSize(kHalfSize.m_fX, kHalfSize.m_fY, kHalfSize.m_fZ);

	// 相机裁剪(参考自Ogre)
	//
	// 依次用视形体各个面进行裁剪,只有均不在各个面的背面才有可能可见
	// (考虑找一些更严密的AABB与Frustum求交算法)
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

	//////////		// 只要包围盒位于任一平面负面,直接剔除
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


	// 度量是否需要进一步向下遍历
	// @{
	bool bShouldSubdivide = false;
	if (bVisible)
	{
		if (!pkTerrain->GetEnableLOD() || m_uiLevel < pkTerrain->GetMeshLevel())
		{
			// 1.地形禁用LOD
			// 2.当前层级没有生成Mesh
			bShouldSubdivide = true;
		}
		else
		{
			// 1.计算节点包围盒到视点的距离
			// 2.根据节点累积误差计算允许观察距离
			// 3.比较两个距离确定是否细分
			//********************
			bShouldSubdivide = true;
			//********************
		}
	}
	// @}


	// 处理
	// @{
	if (bVisible)
	{
		if (bShouldSubdivide)
		{
			// 向下遍历
			if (IsLeaf())
			{
				// 叶子节点直接提取Mesh
				if (m_pkQuadMesh)
					kMeshVec.push_back(m_pkQuadMesh);
			}
			else
			{
				// 中间节点,依照从近到远的次序依次处理子节点
				//
				// 计算子节点中心到观察点距离
				float vfDistance[NUM_CHILD_TYPE] =  { 0 };

				sdVector3 kCamPosition(kCamera.GetWorldTranslate().x, kCamera.GetWorldTranslate().y, kCamera.GetWorldTranslate().z);
				for (int i = 0; i < NUM_CHILD_TYPE; ++i)
				{
					vfDistance[i] = (m_pkQuadNodeChild[i]->GetBound().GetCenter() - kCamPosition).SquaredLength();
				}

				// 每次取出距离数组中最小值的Node处理, 并将其距离置为无穷大
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
			// 停止遍历提取Mesh
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

	// 叶子节点无误差
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