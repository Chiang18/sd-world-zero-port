//*************************************************************************************************
// ����:	��ѧ��(��ȡ��Ogre)
//---------------------------------------------------------
// ����:		��ǿ
// ����:		2012-06-30
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _MATH_H__
#define _MATH_H__

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