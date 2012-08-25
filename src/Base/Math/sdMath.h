//*************************************************************************************************
// ����:	��ѧ��(��ȡ��Ogre)
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

		// ������ѧ�����뺯��(��ȡ��Ogre)
		class sdMath
		{
		public:

			//  
			static bool IsPowerOf2(uint uiNum)
			{
				return 0 == (uiNum & (uiNum -1));
			}

			// 
			static uint Power2_Floor(uint uiNum)
			{
				while (!IsPowerOf2(uiNum))	uiNum &= uiNum-1;
				return uiNum;
			}

			static uint Power2_Ceil(uint uiNum)
			{
				uint uiFloor = Power2_Floor(uiNum);
				if (uiFloor == uiNum)
					return uiFloor;
				else
					return uiFloor << 1;
			}

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