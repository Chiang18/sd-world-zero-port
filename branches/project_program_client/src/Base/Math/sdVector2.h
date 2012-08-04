//*************************************************************************************************
// ����:	�����CryEngine3
//---------------------------------------------------------
// ����:		
// ����:		2012-08-02
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _MATH_VECTOR2_H__
#define _MATH_VECTOR2_H__

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
	}
}
#endif