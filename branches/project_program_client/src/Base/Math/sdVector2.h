//*************************************************************************************************
// ����:	�����CryEngine3/Ogre/Gamebryo
//---------------------------------------------------------
// ����:		
// ����:		2012-08-02
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_BASE_MATH_VECTOR2_H__
#define _SD_BASE_MATH_VECTOR2_H__
#include "sdBase.h"

namespace Base
{
	namespace Math
	{
		// Vec2��ģ��
		template <typename T> struct sdVector2T
		{
			T m_kX;
			T m_kY;

			sdVector2T() : m_kX(0), m_kY(0){};
			sdVector2T(T kX, T kY) : m_kX(kX), m_kY(kY){};
		};

		// �����ͱ��ʵ��
		typedef sdVector2T<int>				sdVector2i;
		typedef sdVector2T<unsigned int>	sdVector2ui;

		typedef sdVector2T<short>			sdVector2s;
		typedef sdVector2T<unsigned short>	sdVector2us;

		typedef sdVector2T<char>			sdVector2c;
		typedef sdVector2T<unsigned char>	sdVector2uc;

		//typedef sdVector2T<float>			sdVector2f;
		//typedef sdVector2T<double>		sdVector2d;

		// Vector��float�汾�ر�ʵ��(��ȡ��Ogre)
		class sdVector2 
		{
		public:
			float	m_fX, m_fY;

		public:
			// ����ֵ
			static const sdVector2 ZERO;
			static const sdVector2 UNIT_X;
			static const sdVector2 UNIT_Y;
			static const sdVector2 NEGATIVE_UNIT_X;
			static const sdVector2 NEGATIVE_UNIT_Y;
			static const sdVector2 UNIT_SCALE;

		public:
			inline sdVector2() {};
			inline sdVector2(const float fX, const float fY) : m_fX(fX), m_fY(fY){};
			inline sdVector2(const float fScaler) : m_fX(fScaler), m_fY(fScaler){};
		};
	}
}
#endif