//*************************************************************************************************
// 内容:	借鉴自CryEngine3/Ogre
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-03
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_BASE_MATH_VECTOR4_H__
#define _SD_BASE_MATH_VECTOR4_H__
#include "sdBase.h"

namespace Base
{
	namespace Math
	{
		// Vec4的模版
		template <typename T> struct sdVector4T
		{
			T m_kX;
			T m_kY;
			T m_kZ;
			T m_kW;

			sdVector4T() : m_kX(0), m_kY(0), m_kZ(0), m_kW(0) {};
			sdVector4T(T kX, T kY, T kZ, T kW) : m_kX(kX), m_kY(kY), m_kZ(kZ), m_kW(kW){};
		};

		// 基础型别的实现
		typedef sdVector4T<int>					sdVector4i;
		typedef sdVector4T<unsigned int>		sdVector4ui;

		typedef sdVector4T<short>				sdVector4s;
		typedef sdVector4T<unsigned short>		sdVector4us;

		typedef sdVector4T<char>				sdVector4c;
		typedef sdVector4T<unsigned char>		sdVector4uc;

		//typedef sdVector4T<float>				sdVector4f;
		//typedef sdVector4T<double>			sdVector4d;

		// Vector的float版本特别实现(抽取自Ogre)
		class sdVector4 
		{
		public:
			float	m_fX, m_fY, m_fZ, m_fW;

		public:
			// 特殊值
			static const sdVector4 ZERO;
			static const sdVector4 UNIT_SCALE;

		public:
			inline sdVector4() {};
			inline sdVector4(const float fX, const float fY, const float fZ, const float fW) : m_fX(fX), m_fY(fY), m_fZ(fZ), m_fW(fW){};
			inline sdVector4(const float fScaler) : m_fX(fScaler), m_fY(fScaler), m_fZ(fScaler), m_fW(fScaler){};
		};
	}
}
#endif