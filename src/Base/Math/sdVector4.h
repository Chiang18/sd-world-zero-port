//*************************************************************************************************
// ����:	�����CryEngine3
//---------------------------------------------------------
// ����:		
// ����:		2012-08-03
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_BASE_MATH_VECTOR4_H__
#define _SD_BASE_MATH_VECTOR4_H__
#include "sdBase.h"

namespace Base
{
	namespace Math
	{
		// Vec4��ģ��
		template <typename T> struct sdVector4T
		{
			T m_kX;
			T m_kY;
			T m_kZ;
			T m_kW;

			sdVector4T() : m_kX(0), m_kY(0), m_kZ(0), m_kW(0) {};
			sdVector4T(T kX, T kY, T kZ, T kW) : m_kX(kX), m_kY(kY), m_kZ(kZ), m_kW(kW){};
		};

		// �����ͱ��ʵ��
		typedef sdVector4T<int>					sdVector4i;
		typedef sdVector4T<unsigned int>		sdVector4ui;

		typedef sdVector4T<short>				sdVector4s;
		typedef sdVector4T<unsigned short>		sdVector4us;

		typedef sdVector4T<char>				sdVector4c;
		typedef sdVector4T<unsigned char>		sdVector4uc;

		//typedef sdVector4T<float>				sdVector4f;
		//typedef sdVector4T<double>			sdVector4d;
	}
}
#endif