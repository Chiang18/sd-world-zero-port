//*************************************************************************************************
// 内容:	借鉴自CryEngine3/Ogre/Gamebryo
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-02
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_BASE_MATH_VECTOR3_H__
#define _SD_BASE_MATH_VECTOR3_H__
#include "sdBase.h"

namespace Base
{
	namespace Math
	{
		// Vec3的模版简单实现(借鉴自CryEngine3)
		template <typename T> struct sdVector3T
		{
			T m_kX;
			T m_kY;
			T m_kZ;

			sdVector3T() : m_kX(0), m_kY(0), m_kZ(0) {};
			sdVector3T(T kX, T kY, T kZ) : m_kX(kX), m_kY(kY), m_kZ(kZ){};
		};

		// 基础型别的实现
		typedef sdVector3T<int>					sdVector3i;
		typedef sdVector3T<unsigned int>		sdVector3ui;

		typedef sdVector3T<short>				sdVector3s;
		typedef sdVector3T<unsigned short>		sdVector3us;

		typedef sdVector3T<char>				sdVector3c;
		typedef sdVector3T<unsigned char>		sdVector3uc;

		//typedef sdVector3T<float>				sdVector3f;
		//typedef sdVsdVector3Tec3T<double>		sdVector3d;

		// Vector的float版本特别实现(抽取自Ogre)
		class sdVector3 
		{
		public:
			float	m_fX, m_fY, m_fZ;

		public:
			// 特殊值
			static const sdVector3 ZERO;
			static const sdVector3 UNIT_X;
			static const sdVector3 UNIT_Y;
			static const sdVector3 UNIT_Z;
			static const sdVector3 NEGATIVE_UNIT_X;
			static const sdVector3 NEGATIVE_UNIT_Y;
			static const sdVector3 NEGATIVE_UNIT_Z;
			static const sdVector3 UNIT_SCALE;

		public:
			inline sdVector3() {};
			inline sdVector3(const float fX, const float fY, const float fZ) : m_fX(fX), m_fY(fY), m_fZ(fZ){};
			inline sdVector3(const float fScaler) : m_fX(fScaler), m_fY(fScaler), m_fZ(fScaler){};

			// 运算符重载
			// @{
			inline float operator [] (const size_t i) const
			{
				assert(i < 3);
				return *(&m_fX + i);
			}

			inline float& operator [] (const size_t i)
			{
				assert(i < 3);
				return *(&m_fX + i);
			}

			inline sdVector3& operator = (const sdVector3& rkVector)
			{
				m_fX = rkVector.m_fX;
				m_fY = rkVector.m_fY;
				m_fZ = rkVector.m_fZ;

				return *this;
			}

			inline sdVector3& operator = (const float fScaler)
			{
				m_fX = fScaler;
				m_fY = fScaler;
				m_fZ = fScaler;

				return *this;
			}

			inline bool operator == (const sdVector3& rkVector) const
			{
				return (m_fX == rkVector.m_fX && m_fY == rkVector.m_fY && m_fZ == rkVector.m_fZ);
			}

			inline bool operator != (const sdVector3& rkVector) const
			{
				return (m_fX != rkVector.m_fX || m_fY != rkVector.m_fY || m_fZ != rkVector.m_fZ);
			}

			// arithmetic operations
			inline sdVector3 operator + (const sdVector3& rkVector) const
			{
				return sdVector3(
					m_fX + rkVector.m_fX,
					m_fY + rkVector.m_fY,
					m_fZ + rkVector.m_fZ);
			}

			inline sdVector3 operator - (const sdVector3& rkVector) const
			{
				return sdVector3(
					m_fX - rkVector.m_fX,
					m_fY - rkVector.m_fY,
					m_fZ - rkVector.m_fZ);
			}

			inline sdVector3 operator * (const float fScalar) const
			{
				return sdVector3(
					m_fX * fScalar,
					m_fY * fScalar,
					m_fZ * fScalar);
			}

			inline sdVector3 operator * (const sdVector3& rhs) const
			{
				return sdVector3(
					m_fX * rhs.m_fX,
					m_fY * rhs.m_fY,
					m_fZ * rhs.m_fZ);
			}

			inline sdVector3 operator / (const float fScalar) const
			{
				assert(fScalar != 0.0);

				float fInv = 1.0f / fScalar;

				return sdVector3(
					m_fX * fInv,
					m_fY * fInv,
					m_fZ * fInv);
			}

			inline sdVector3 operator / (const sdVector3& rhs) const
			{
				return sdVector3(
					m_fX / rhs.m_fX,
					m_fY / rhs.m_fY,
					m_fZ / rhs.m_fZ);
			}

			inline const sdVector3& operator + () const
			{
				return *this;
			}

			inline sdVector3 operator - () const
			{
				return sdVector3(-m_fX, -m_fY, -m_fZ);
			}

			// overloaded operators to help sdVector3
			inline friend sdVector3 operator * (const float fScalar, const sdVector3& rkVector)
			{
				return sdVector3(
					fScalar * rkVector.m_fX,
					fScalar * rkVector.m_fY,
					fScalar * rkVector.m_fZ);
			}

			inline friend sdVector3 operator / (const float fScalar, const sdVector3& rkVector)
			{
				return sdVector3(
					fScalar / rkVector.m_fX,
					fScalar / rkVector.m_fY,
					fScalar / rkVector.m_fZ);
			}

			inline friend sdVector3 operator + (const sdVector3& lhs, const float rhs)
			{
				return sdVector3(
					lhs.m_fX + rhs,
					lhs.m_fY + rhs,
					lhs.m_fZ + rhs);
			}

			inline friend sdVector3 operator + (const float lhs, const sdVector3& rhs)
			{
				return sdVector3(
					lhs + rhs.m_fX,
					lhs + rhs.m_fY,
					lhs + rhs.m_fZ);
			}

			inline friend sdVector3 operator - (const sdVector3& lhs, const float rhs)
			{
				return sdVector3(
					lhs.m_fX - rhs,
					lhs.m_fY - rhs,
					lhs.m_fZ - rhs);
			}

			inline friend sdVector3 operator - (const float lhs, const sdVector3& rhs)
			{
				return sdVector3(
					lhs - rhs.m_fX,
					lhs - rhs.m_fY,
					lhs - rhs.m_fZ);
			}

			// arithmetic updates
			inline sdVector3& operator += (const sdVector3& rkVector)
			{
				m_fX += rkVector.m_fX;
				m_fY += rkVector.m_fY;
				m_fZ += rkVector.m_fZ;

				return *this;
			}

			inline sdVector3& operator += (const float fScalar)
			{
				m_fX += fScalar;
				m_fY += fScalar;
				m_fZ += fScalar;
				return *this;
			}

			inline sdVector3& operator -= (const sdVector3& rkVector)
			{
				m_fX -= rkVector.m_fX;
				m_fY -= rkVector.m_fY;
				m_fZ -= rkVector.m_fZ;

				return *this;
			}

			inline sdVector3& operator -= (const float fScalar)
			{
				m_fX -= fScalar;
				m_fY -= fScalar;
				m_fZ -= fScalar;
				return *this;
			}

			inline sdVector3& operator *= (const float fScalar)
			{
				m_fX *= fScalar;
				m_fY *= fScalar;
				m_fZ *= fScalar;
				return *this;
			}

			inline sdVector3& operator *= (const sdVector3& rkVector)
			{
				m_fX *= rkVector.m_fX;
				m_fY *= rkVector.m_fY;
				m_fZ *= rkVector.m_fZ;

				return *this;
			}

			inline sdVector3& operator /= (const float fScalar)
			{
				assert(fScalar != 0.0);

				float fInv = 1.0f / fScalar;

				m_fX *= fInv;
				m_fY *= fInv;
				m_fZ *= fInv;

				return *this;
			}

			inline sdVector3& operator /= (const sdVector3& rkVector)
			{
				m_fX /= rkVector.m_fX;
				m_fY /= rkVector.m_fY;
				m_fZ /= rkVector.m_fZ;

				return *this;
			}

			inline bool operator < (const sdVector3& rhs) const
			{
				if (m_fX < rhs.m_fX && m_fY < rhs.m_fY && m_fZ < rhs.m_fZ )
					return true;
				return false;
			}

			inline bool operator > (const sdVector3& rhs) const
			{
				if (m_fX > rhs.m_fX && m_fY > rhs.m_fY && m_fZ > rhs.m_fZ )
					return true;
				return false;
			}
			// @}

			// 常用
			// @{
			// 长度
			inline float Length() const
			{
				return sqrt(m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ);
			}

			inline float SquaredLength() const
			{
				return m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ;
			}

			// 内积
			inline float DotProduct(const sdVector3& kVec) const
			{
				return m_fX * kVec.m_fX + m_fY * kVec.m_fY + m_fZ * kVec.m_fZ;
			}

			// 外积
			inline sdVector3 CrossProduct(const sdVector3& rkVector) const
			{
				return sdVector3(
					m_fY * rkVector.m_fZ - m_fZ * rkVector.m_fY,
					m_fZ * rkVector.m_fX - m_fX * rkVector.m_fZ,
					m_fX * rkVector.m_fY - m_fY * rkVector.m_fX);
			}

			inline void CrossProduct(const sdVector3& rkVector, sdVector3& kVector) const
			{
				kVector.m_fX = m_fY * rkVector.m_fZ - m_fZ * rkVector.m_fY;
				kVector.m_fY = m_fZ * rkVector.m_fX - m_fX * rkVector.m_fZ;
				kVector.m_fZ = m_fX * rkVector.m_fY - m_fY * rkVector.m_fX;
			}

			// 归一化
			inline float Normalise()
			{
				float fLength = sdMath::Sqrt(m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ);
				if (fLength > 0.0f)
				{
					float fInvLength = 1.0f / fLength;
					m_fX *= fInvLength;
					m_fY *= fInvLength;
					m_fZ *= fInvLength;
				}

				return fLength;
			}

			// 取两个Vector每个分量的较小值
			inline void MakeMinimum(const sdVector3& kVector)
			{
				if(kVector.m_fX < m_fX)	m_fX = kVector.m_fX;
				if(kVector.m_fY < m_fY) m_fY = kVector.m_fY;
				if(kVector.m_fZ < m_fZ) m_fZ = kVector.m_fZ;
			}

			// 取两个Vector每个分量的较大值 
			inline void MakeMaximum(const sdVector3& kVector)
			{
				if(kVector.m_fX > m_fX)	m_fX = kVector.m_fX;
				if(kVector.m_fY > m_fY) m_fY = kVector.m_fY;
				if(kVector.m_fZ > m_fZ) m_fZ = kVector.m_fZ;
			}
			// @}
		};
	}
}
#endif