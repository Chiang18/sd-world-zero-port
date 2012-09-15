//*************************************************************************************************
// 内容:	抽取自Ogre/OgreMatrix3.h
//---------------------------------------------------------
// 作者:		
// 创建:		2012-09-10
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_BASE_MATH_MATRIX3_H__
#define _SD_BASE_MATH_MATRIX3_H__
#include "sdBase.h"
#include "sdVector3.h"

namespace Base
{
	namespace Math
	{
		class sdMatrix3
		{
		public:
			union 
			{
				float m[3][3];
				float _m[9];
				struct  
				{
					float _m00, _m01, _m02;
					float _m10, _m11, _m12;
					float _m20, _m21, _m22;
				};
				struct  
				{
					float _00, _01, _02;
					float _10, _11, _12;
					float _20, _21, _22;
				};
			};

		public:
			static const sdMatrix3 ZERO;
			static const sdMatrix3 IDENTITY;

		public:
			inline sdMatrix3()
			{
			}

			inline sdMatrix3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22)
			{
				m[0][0] = m00;	m[0][1] = m01;	m[0][2] = m02;
				m[1][0] = m10;	m[1][1] = m11;	m[1][2] = m12;
				m[2][0] = m20;	m[2][1] = m21;	m[2][2] = m22;
			}

			// 运算符重载
			// @{
			inline sdMatrix3 concatenate(const sdMatrix3 &kRight) const
			{
				sdMatrix3 kMatrix;
				kMatrix.m[0][0] = m[0][0] * kRight.m[0][0] + m[0][1] * kRight.m[1][0] + m[0][2] * kRight.m[2][0];
				kMatrix.m[0][1] = m[0][0] * kRight.m[0][1] + m[0][1] * kRight.m[1][1] + m[0][2] * kRight.m[2][1];
				kMatrix.m[0][2] = m[0][0] * kRight.m[0][2] + m[0][1] * kRight.m[1][2] + m[0][2] * kRight.m[2][2];

				kMatrix.m[1][0] = m[1][0] * kRight.m[0][0] + m[1][1] * kRight.m[1][0] + m[1][2] * kRight.m[2][0];
				kMatrix.m[1][1] = m[1][0] * kRight.m[0][1] + m[1][1] * kRight.m[1][1] + m[1][2] * kRight.m[2][1];
				kMatrix.m[1][2] = m[1][0] * kRight.m[0][2] + m[1][1] * kRight.m[1][2] + m[1][2] * kRight.m[2][2];

				kMatrix.m[2][0] = m[2][0] * kRight.m[0][0] + m[2][1] * kRight.m[1][0] + m[2][2] * kRight.m[2][0];
				kMatrix.m[2][1] = m[2][0] * kRight.m[0][1] + m[2][1] * kRight.m[1][1] + m[2][2] * kRight.m[2][1];
				kMatrix.m[2][2] = m[2][0] * kRight.m[0][2] + m[2][1] * kRight.m[1][2] + m[2][2] * kRight.m[2][2];
				return kMatrix;
			}

			inline sdMatrix3 operator * (const sdMatrix3 &kRight) const
			{
				return concatenate(kRight);
			}

			//
			friend sdVector3 operator* (const sdVector3& rkVector, const sdMatrix3& rkMatrix)
			{
				sdVector3 kProd;
				for (size_t iRow = 0; iRow < 3; iRow++)
				{
					kProd[iRow] =
						rkVector[0] * rkMatrix.m[0][iRow] +
						rkVector[1] * rkMatrix.m[1][iRow] +
						rkVector[2] * rkMatrix.m[2][iRow];
				}
				return kProd;
			}
			// @}
		};
	}
}
#endif