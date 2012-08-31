#include "Stdafx.h"
#include "sdMath.h"
#include "sdRay.h"
#include "sdAxisAlignedBox.h"

namespace Base
{
	namespace Math
	{
		//-----------------------------------------------------------------------------------------
		bool sdMath::Intersects(const sdRay& kRay, const sdAxisAlignedBox& kAABB, float& fStart)
		{
			// 处理特殊包围盒
			if (kAABB.isNull()) 
				return false;

			if (kAABB.isInfinite()) 
			{
				fStart = 0.0f;
				return true;
			}

			// Ray起始点位于AABB内部,直接返回
			const sdVector3& kRayOrigin = kRay.GetOrigin();
			const sdVector3& kRayDir = kRay.GetDirection();
			const sdVector3& kBoxMin = kAABB.GetMinimum();
			const sdVector3& kBoxMax = kAABB.GetMaximum();
			if (kRayOrigin > kBoxMin && kRayOrigin < kBoxMax)
			{
				fStart = 0.0f;
				return true;
			}

			// Check each face in turn, only check closest 3
			// 依次检查AABB的各个面, 
			// @{
			float fMinDistance = 0.0f;	// 记录当前交点最小距离
			float fDistance = 0.0f;		// 记录当前交点距离
			bool bIntersect = false;	// 记录当前是否有交点
			sdVector3 kIntersect;		// 记录当前交点

			// Min x
			if (kRayOrigin.m_fX <= kBoxMin.m_fX && kRayDir.m_fX > 0)
			{
				fDistance = (kBoxMin.m_fX - kRayOrigin.m_fX) / kRayDir.m_fX;
				if (fDistance >= 0)
				{
					// 计算交点,检查交点范围
					kIntersect = kRayOrigin + kRayDir * fDistance;
					if (kIntersect.m_fY >= kBoxMin.m_fY && kIntersect.m_fY <= kBoxMax.m_fY &&
						kIntersect.m_fZ >= kBoxMin.m_fZ && kIntersect.m_fZ <= kBoxMax.m_fZ &&
						(!bIntersect || fDistance < fMinDistance))
					{
						bIntersect = true;
						fMinDistance = fDistance;
					}
				}
			}

			// Max x
			if (kRayOrigin.m_fX >= kBoxMax.m_fX && kRayDir.m_fX < 0)
			{
				fDistance = (kBoxMax.m_fX - kRayOrigin.m_fX) / kRayDir.m_fX;
				if (fDistance >= 0)
				{
					// Substitute fDistance back into ray and check bounds and dist
					kIntersect = kRayOrigin + kRayDir * fDistance;
					if (kIntersect.m_fY >= kBoxMin.m_fY && kIntersect.m_fY <= kBoxMax.m_fY &&
						kIntersect.m_fZ >= kBoxMin.m_fZ && kIntersect.m_fZ <= kBoxMax.m_fZ &&
						(!bIntersect || fDistance < fMinDistance))
					{
						bIntersect = true;
						fMinDistance = fDistance;
					}
				}
			}

			// Min y
			if (kRayOrigin.m_fY <= kBoxMin.m_fY && kRayDir.m_fY > 0)
			{
				fDistance = (kBoxMin.m_fY - kRayOrigin.m_fY) / kRayDir.m_fY;
				if (fDistance >= 0)
				{
					// Substitute fDistance back into ray and check bounds and dist
					kIntersect = kRayOrigin + kRayDir * fDistance;
					if (kIntersect.m_fX >= kBoxMin.m_fX && kIntersect.m_fX <= kBoxMax.m_fX &&
						kIntersect.m_fZ >= kBoxMin.m_fZ && kIntersect.m_fZ <= kBoxMax.m_fZ &&
						(!bIntersect || fDistance < fMinDistance))
					{
						bIntersect = true;
						fMinDistance = fDistance;
					}
				}
			}

			// Max y
			if (kRayOrigin.m_fY >= kBoxMax.m_fY && kRayDir.m_fY < 0)
			{
				fDistance = (kBoxMax.m_fY - kRayOrigin.m_fY) / kRayDir.m_fY;
				if (fDistance >= 0)
				{
					// Substitute fDistance back into ray and check bounds and dist
					kIntersect = kRayOrigin + kRayDir * fDistance;
					if (kIntersect.m_fX >= kBoxMin.m_fX && kIntersect.m_fX <= kBoxMax.m_fX &&
						kIntersect.m_fZ >= kBoxMin.m_fZ && kIntersect.m_fZ <= kBoxMax.m_fZ &&
						(!bIntersect || fDistance < fMinDistance))
					{
						bIntersect = true;
						fMinDistance = fDistance;
					}
				}
			}

			// Min z
			if (kRayOrigin.m_fZ <= kBoxMin.m_fZ && kRayDir.m_fZ > 0)
			{
				fDistance = (kBoxMin.m_fZ - kRayOrigin.m_fZ) / kRayDir.m_fZ;
				if (fDistance >= 0)
				{
					// Substitute fDistance back into ray and check bounds and dist
					kIntersect = kRayOrigin + kRayDir * fDistance;
					if (kIntersect.m_fX >= kBoxMin.m_fX && kIntersect.m_fX <= kBoxMax.m_fX &&
						kIntersect.m_fY >= kBoxMin.m_fY && kIntersect.m_fY <= kBoxMax.m_fY &&
						(!bIntersect || fDistance < fMinDistance))
					{
						bIntersect = true;
						fMinDistance = fDistance;
					}
				}
			}

			// Max z
			if (kRayOrigin.m_fZ >= kBoxMax.m_fZ && kRayDir.m_fZ < 0)
			{
				fDistance = (kBoxMax.m_fZ - kRayOrigin.m_fZ) / kRayDir.m_fZ;
				if (fDistance >= 0)
				{
					// Substitute fDistance back into ray and check bounds and dist
					kIntersect = kRayOrigin + kRayDir * fDistance;
					if (kIntersect.m_fX >= kBoxMin.m_fX && kIntersect.m_fX <= kBoxMax.m_fX &&
						kIntersect.m_fY >= kBoxMin.m_fY && kIntersect.m_fY <= kBoxMax.m_fY &&
						(!bIntersect || fDistance < fMinDistance))
					{
						bIntersect = true;
						fMinDistance = fDistance;
					}
				}
			}
			// @}

			fStart = fMinDistance;
			return bIntersect;
		} 
		//-----------------------------------------------------------------------------------------
		bool sdMath::Intersects(const sdRay& kRay, const sdAxisAlignedBox& kAABB, float& fStart, float& fEnd)
		{
			// 处理特殊包围盒
			if (kAABB.isNull())
				return false;

			if (kAABB.isInfinite())
			{
				fStart = 0;
				fEnd = POS_INFINITY;
				return true;
			}

			//
			const sdVector3& kRayOrigin = kRay.GetOrigin();
			const sdVector3& kRayDir = kRay.GetDirection();
			const sdVector3& kBoxMin = kAABB.GetMinimum();
			const sdVector3& kBoxMax = kAABB.GetMaximum();

			// 对Ray的三个方向按照分量绝对值大小进行排序,
			sdVector3 kAbsDir;
			kAbsDir[0] = sdMath::Abs(kRayDir[0]);
			kAbsDir[1] = sdMath::Abs(kRayDir[1]);
			kAbsDir[2] = sdMath::Abs(kRayDir[2]);

			int iMax = 0, iMid = 1, iMin = 2;
			if (kAbsDir[0] < kAbsDir[2])
			{
				iMax = 2;
				iMin = 0;
			}
			if (kAbsDir[1] < kAbsDir[iMin])
			{
				iMid = iMin;
				iMin = 1;
			}
			else if (kAbsDir[1] > kAbsDir[iMax])
			{
				iMid = iMax;
				iMax = 1;
			}

			// 按照Ray的三个方向强度从最大到最小进行处理
			// @{
			fStart = 0;
			fEnd = sdMath::POS_INFINITY;

#define _CALC_AXIS(i)												\
	do {															\
		float fDenom = 1 / kRayDir[i];								\
		float fNewStart = (kBoxMin[i] - kRayOrigin[i]) * fDenom;	\
		float fNewEnd = (kBoxMax[i] - kRayOrigin[i]) * fDenom;      \
		if (fNewStart > fNewEnd) std::swap(fNewStart, fNewEnd);		\
		if (fNewStart > fEnd || fNewEnd < fStart) return false;		\
		if (fNewStart > fStart) fStart = fNewStart;					\
		if (fNewEnd < fEnd) fEnd = fNewEnd;							\
	} while(0)

			// 计算最大轴向
			_CALC_AXIS(iMax);

			if (kAbsDir[iMid] < std::numeric_limits<float>::epsilon())
			{
				// 射线平行于次大轴和最小轴
				if (kRayOrigin[iMid] < kBoxMin[iMid] || kRayOrigin[iMid] > kBoxMax[iMid] ||
					kRayOrigin[iMin] < kBoxMin[iMin] || kRayOrigin[iMin] > kBoxMax[iMin])
					return false;
			}
			else
			{
				// 计算次大轴向
				_CALC_AXIS(iMid);

				if (kAbsDir[iMin] < std::numeric_limits<float>::epsilon())
				{
					// 射线平行于最小轴
					if (kRayOrigin[iMin] < kBoxMin[iMin] || kRayOrigin[iMin] > kBoxMax[iMin])
						return false;
				}
				else
				{
					// 计算最小轴向
					_CALC_AXIS(iMin);
				}
			}
#undef _CALC_AXIS
			// @}

			return true;
		}
	}
}