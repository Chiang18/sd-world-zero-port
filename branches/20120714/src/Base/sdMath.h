//*************************************************************************************************
// 内容:	数学库(目前只有一部分)
//---------------------------------------------------------
// 作者:		喻强
// 创建:		2012-06-30
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _MATH_H__
#define _MATH_H__

namespace Math
{

#define	sd_math_pi			3.14159265358979323846264338327950288419716939937510582f
#define sd_math_e			2.71828182845904523536028747135266249775724709369995957f

#define sd_math_to_rad		math_pi / 180.0f
#define sd_math_to_deg		180.0f  / sd_math_pi
#define sd_math_eps			10e-6f
#define sd_math_big_eps		10e-2f
#define sd_math_small_eps	10e-6f
#define sd_math_sqrthalf	0.7071067811865475244f

//Vector2
//Vector3
//Vector4
//Matrix3
//Matrix4
//Quaternion
//Plane
//BoundBox
//Ray
//AABB
//OBB

}

#endif