//*************************************************************************************************
// 内容:	抽取自Ogre/OgreMatrix4.h
//---------------------------------------------------------
// 作者:		
// 创建:		2012-09-10
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_BASE_MATH_MATRIX4_H__
#define _SD_BASE_MATH_MATRIX4_H__
#include "sdBase.h"

namespace Base
{
	namespace Math
	{
		class sdMatrix4
		{
		public:
			union 
			{
				float m[4][4];
				float _m[16];
				struct  
				{
					float _m00, _m01, _m02, _m03;
					float _m10, _m11, _m12, _m13;
					float _m20, _m21, _m22, _m23;
					float _m30, _m31, _m32, _m33;
				};
				struct  
				{
					float _00, _01, _02, _03;
					float _10, _11, _12, _13;
					float _20, _21, _22, _23;
					float _30, _31, _32, _33;
				};
			};

		public:
			static const sdMatrix4 ZERO;
			static const sdMatrix4 ZEROAFFINE;
			static const sdMatrix4 IDENTITY;

		public:
			inline sdMatrix4()
			{
			}

			inline sdMatrix4(float m00, float m01, float m02, float m03,float m10, float m11, float m12, float m13,
				float m20, float m21, float m22, float m23,	float m30, float m31, float m32, float m33)
			{
				m[0][0] = m00;	m[0][1] = m01;	m[0][2] = m02;	m[0][3] = m03;
				m[1][0] = m10;	m[1][1] = m11;	m[1][2] = m12;	m[1][3] = m13;
				m[2][0] = m20;	m[2][1] = m21;	m[2][2] = m22;	m[2][3] = m23;
				m[3][0] = m30;	m[3][1] = m31;	m[3][2] = m32;	m[3][3] = m33;
			}

			// 运算符重载
			// @{
			inline sdMatrix4 concatenate(const sdMatrix4 &kRight) const
			{
				sdMatrix4 kMatrix;
				kMatrix.m[0][0] = m[0][0] * kRight.m[0][0] + m[0][1] * kRight.m[1][0] + m[0][2] * kRight.m[2][0] + m[0][3] * kRight.m[3][0];
				kMatrix.m[0][1] = m[0][0] * kRight.m[0][1] + m[0][1] * kRight.m[1][1] + m[0][2] * kRight.m[2][1] + m[0][3] * kRight.m[3][1];
				kMatrix.m[0][2] = m[0][0] * kRight.m[0][2] + m[0][1] * kRight.m[1][2] + m[0][2] * kRight.m[2][2] + m[0][3] * kRight.m[3][2];
				kMatrix.m[0][3] = m[0][0] * kRight.m[0][3] + m[0][1] * kRight.m[1][3] + m[0][2] * kRight.m[2][3] + m[0][3] * kRight.m[3][3];

				kMatrix.m[1][0] = m[1][0] * kRight.m[0][0] + m[1][1] * kRight.m[1][0] + m[1][2] * kRight.m[2][0] + m[1][3] * kRight.m[3][0];
				kMatrix.m[1][1] = m[1][0] * kRight.m[0][1] + m[1][1] * kRight.m[1][1] + m[1][2] * kRight.m[2][1] + m[1][3] * kRight.m[3][1];
				kMatrix.m[1][2] = m[1][0] * kRight.m[0][2] + m[1][1] * kRight.m[1][2] + m[1][2] * kRight.m[2][2] + m[1][3] * kRight.m[3][2];
				kMatrix.m[1][3] = m[1][0] * kRight.m[0][3] + m[1][1] * kRight.m[1][3] + m[1][2] * kRight.m[2][3] + m[1][3] * kRight.m[3][3];

				kMatrix.m[2][0] = m[2][0] * kRight.m[0][0] + m[2][1] * kRight.m[1][0] + m[2][2] * kRight.m[2][0] + m[2][3] * kRight.m[3][0];
				kMatrix.m[2][1] = m[2][0] * kRight.m[0][1] + m[2][1] * kRight.m[1][1] + m[2][2] * kRight.m[2][1] + m[2][3] * kRight.m[3][1];
				kMatrix.m[2][2] = m[2][0] * kRight.m[0][2] + m[2][1] * kRight.m[1][2] + m[2][2] * kRight.m[2][2] + m[2][3] * kRight.m[3][2];
				kMatrix.m[2][3] = m[2][0] * kRight.m[0][3] + m[2][1] * kRight.m[1][3] + m[2][2] * kRight.m[2][3] + m[2][3] * kRight.m[3][3];

				kMatrix.m[3][0] = m[3][0] * kRight.m[0][0] + m[3][1] * kRight.m[1][0] + m[3][2] * kRight.m[2][0] + m[3][3] * kRight.m[3][0];
				kMatrix.m[3][1] = m[3][0] * kRight.m[0][1] + m[3][1] * kRight.m[1][1] + m[3][2] * kRight.m[2][1] + m[3][3] * kRight.m[3][1];
				kMatrix.m[3][2] = m[3][0] * kRight.m[0][2] + m[3][1] * kRight.m[1][2] + m[3][2] * kRight.m[2][2] + m[3][3] * kRight.m[3][2];
				kMatrix.m[3][3] = m[3][0] * kRight.m[0][3] + m[3][1] * kRight.m[1][3] + m[3][2] * kRight.m[2][3] + m[3][3] * kRight.m[3][3];

				return kMatrix;
			}

			inline sdMatrix4 operator * (const sdMatrix4 &kRight) const
			{
				return concatenate(kRight);
			}
			// @}
		};
	}
}
#endif