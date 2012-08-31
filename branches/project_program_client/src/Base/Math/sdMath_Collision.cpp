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
			// ���������Χ��
			if (kAABB.isNull()) 
				return false;

			if (kAABB.isInfinite()) 
			{
				fStart = 0.0f;
				return true;
			}

			// Ray��ʼ��λ��AABB�ڲ�,ֱ�ӷ���
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
			// ���μ��AABB�ĸ�����, 
			// @{
			float fMinDistance = 0.0f;	// ��¼��ǰ������С����
			float fDistance = 0.0f;		// ��¼��ǰ�������
			bool bIntersect = false;	// ��¼��ǰ�Ƿ��н���
			sdVector3 kIntersect;		// ��¼��ǰ����

			// Min x
			if (kRayOrigin.m_fX <= kBoxMin.m_fX && kRayDir.m_fX > 0)
			{
				fDistance = (kBoxMin.m_fX - kRayOrigin.m_fX) / kRayDir.m_fX;
				if (fDistance >= 0)
				{
					// ���㽻��,��齻�㷶Χ
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
			// ���������Χ��
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

			// ��Ray�����������շ�������ֵ��С��������,
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

			// ����Ray����������ǿ�ȴ������С���д���
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

			// �����������
			_CALC_AXIS(iMax);

			if (kAbsDir[iMid] < std::numeric_limits<float>::epsilon())
			{
				// ����ƽ���ڴδ������С��
				if (kRayOrigin[iMid] < kBoxMin[iMid] || kRayOrigin[iMid] > kBoxMax[iMid] ||
					kRayOrigin[iMin] < kBoxMin[iMin] || kRayOrigin[iMin] > kBoxMax[iMin])
					return false;
			}
			else
			{
				// ����δ�����
				_CALC_AXIS(iMid);

				if (kAbsDir[iMin] < std::numeric_limits<float>::epsilon())
				{
					// ����ƽ������С��
					if (kRayOrigin[iMin] < kBoxMin[iMin] || kRayOrigin[iMin] > kBoxMax[iMin])
						return false;
				}
				else
				{
					// ������С����
					_CALC_AXIS(iMin);
				}
			}
#undef _CALC_AXIS
			// @}

			return true;
		}
	}
}