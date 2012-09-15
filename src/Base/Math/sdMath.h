//*************************************************************************************************
// ����:	��ѧ��(��ȡ��Ogre/OgreMath.h)
//---------------------------------------------------------
// ����:		
// ����:		2012-06-30
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_BASE_MATH_H__
#define _SD_BASE_MATH_H__
#include "sdBase.h"

namespace Base
{
	namespace Math
	{
		// һЩ����(����wz,��ʱ����)
		//#define	sd_math_pi			3.14159265358979323846264338327950288419716939937510582f
		//#define	sd_math_e			2.71828182845904523536028747135266249775724709369995957f
		//#define	sd_math_to_rad		math_pi / 180.0f
		//#define	sd_math_to_deg		180.0f  / sd_math_pi
		//#define	sd_math_eps			10e-6f
		//#define	sd_math_big_eps		10e-2f
		//#define	sd_math_small_eps	10e-6f
		//#define	sd_math_sqrthalf	0.7071067811865475244f

		//
		class sdRay;
		class sdAxisAlignedBox;

		// ������ѧ�����뺯��(��ȡ��Ogre)
		class sdMath
		{
		public:
			// ����ֵ
			static inline float Abs(float fValue){return (float)fabs(fValue);}

			// ƽ��/ƽ����
			// @{
			static inline float Sqr(float fValue) { return fValue*fValue;}
			static inline float Sqrt(float fValue) { return (float)sqrt(fValue);}

			static inline float FastSqrt(float fValue)
			{
				const float fZero = 0.0f;
				const float fThree = 3.0f;
				const float fOneHalf = 0.5f;
				float fTemp;

				__asm
				{
					movss	xmm1, [fValue]
					rsqrtss	xmm0, xmm1		// 1/sqrt estimate(12bits)

					// Newton-Raphson iteration(X1 = 0.5 * x0 * (3 - (y*x0) * x0))
					movss	xmm3, [fThree]
					movss	xmm2, xmm0
					mulss	xmm0, xmm1			// y * x0
					mulss	xmm0, xmm2			// y * x0 * x0
					mulss	xmm2, [fOneHalf];	// 0.5 * x0
					subss	xmm3, xmm0			// 3 - (y*x0) * x0)
					mulss	xmm3, xmm2			// 0.5 * x0 * (3 - (y*x0) * x0))

					movss	xmm4, [fZero];
					cmpss	xmm4, xmm1, 4		// not equal
					mulss	xmm3, xmm1			// sqrt(f) = f * 1/sqrt(f);
					andps	xmm3, xmm4			// seet result to zero if input is zero
					movss	[fTemp], xmm3
				}

				return fTemp;
			}

			static inline float FastInvSqrt(float fValue)
			{
				const float fThree = 3.0f;
				const float fOneHalf = 0.5f;
				float fTemp;

				__asm
				{
					movss	xmm1, [fValue]
					rsqrtss	xmm0, xmm1		// 1/sqrt estimate(12bits)

					// Newton-Raphson iteration(X1 = 0.5 * x0 * (3 - (y*x0) * x0))
					movss	xmm3, [fThree]
					movss	xmm2, xmm0
						mulss	xmm0, xmm1			// y * x0
						mulss	xmm0, xmm2			// y * x0 * x0
						mulss	xmm2, [fOneHalf];	// 0.5 * x0
					subss	xmm3, xmm0			// 3 - (y*x0) * x0)
						mulss	xmm3, xmm2			// 0.5 * x0 * (3 - (y*x0) * x0))
						movss	[fTemp], xmm3
				}

				return fTemp;	
			}
			// @}

			// ȡ����
			// @{
			static inline int Ceil (float fValue) { return int(ceil(fValue)); }
			static inline int Floor (float fValue) { return int(floor(fValue)); }
			
			static inline int QuickTrunc(float fValue)
			{
				__asm cvttss2si	eax, [fValue];
				// return value in eax.
			}

			static inline int QuickRound(float fValue)
			{
				__asm cvtss2si	eax, [fValue];
				// return value in eax.
			}

			static inline int QuickFloor(float fValue)
			{
				const uint mxcsr_floor = 0x00003f80;
				const uint mxcsr_default = 0x00001f80;

				__asm ldmxcsr	[mxcsr_floor];		// Round toward -infinity.
				__asm cvtss2si	eax, [fValue];
				__asm ldmxcsr	[mxcsr_default];	// Round to nearest
				
			}
			// @}

			// �����
			static inline int Rand() { return rand();}
			static inline float Rand(float fMaximum) { return rand() * (fMaximum / RAND_MAX);}
			static inline float Frand() { return rand() / (float)RAND_MAX;}

			//  
			static inline bool IsPowerOf2(uint uiNum)
			{
				return 0 == (uiNum & (uiNum -1));
			}

			// 
			static inline uint Power2_Floor(uint uiNum)
			{
				while (!IsPowerOf2(uiNum))	uiNum &= uiNum-1;
				return uiNum;
			}

			//
			static inline uint Power2_Ceil(uint uiNum)
			{
				uint uiFloor = Power2_Floor(uiNum);
				if (uiFloor == uiNum)
					return uiFloor;
				else
					return uiFloor << 1;
			}

			// ���㷨
			// @{
			// ����λ�ڰ�Χ���ⲿ
			static bool Intersects(const sdRay& kRay, const sdAxisAlignedBox& kAABB, float& fStart);

			// 
			static bool Intersects(const sdRay& kRay, const sdAxisAlignedBox& kAABB, float& fStart, float& fEnd);
			// @}

			// ǯλ
			template <typename T>
			static inline T Clamp(const T kValue, const T kMinValue, const T kMaxValue)
			{
				return kValue < kMinValue ? kMinValue : (kValue > kMaxValue ? kMaxValue : kValue);
			}

			// ��ֵ
			// @{
			// ���Բ�ֵ
			template <typename T>
			static inline T Lerp(const T& kStartValue, const T& kEndValue, const T kWeight)
			{
				return kStartValue + static_cast<T>((kEndValue - kStartValue) * kWeight);
			}

			// ˫���Բ�ֵ
			//template <typename T>
			//static inline T Bilerp(const T& kStartValue, const T& kEndValue, const T kWeight)
			//{
			//	
			//}

			// @}


		public:
			static const float	POS_INFINITY;	// float�ܱ�ʾ����󸡵���
			static const float	NEG_INFINITY;	// float�ܱ�ʾ����С������

			static const float	E;				// ��Ȼ�����ĵ�
			static const float	PI;				// Բ����
			static const float	TWO_PI;
			static const float	HALF_PI;
			static const float	Deg2Rad;
			static const float	Rad2Deg;
		};
	}
}

#endif