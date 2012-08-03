//*************************************************************************************************
// ����:	���ڸ���ִ�б�����
//---------------------------------------------------------
// ����:	
// ����:		2012-07-24
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _STATIC_ASSERT_H__
#define _STATIC_ASSERT_H__

#pragma warning(push)
#pragma warning(error:4200)

namespace Base
{

	//
	template <int iBoolValue>
	struct sdStaticAssert
	{
		int iTmp[iBoolValue];
	};

	//
	template <int iValue, int iRef>
	struct sdStaticAssertLess
	{
		sdStaticAssert<iRef - iValue> kTmp;
	};

	template <int iValue, int iRef>
	struct sdStaticAssertMore
	{
		sdStaticAssert<iValue - iRef> kTmp;
	};
}

#pragma warning(pop)
#endif