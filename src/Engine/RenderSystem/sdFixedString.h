//*************************************************************************************************
// 内容:	字符串的封装,用于只需要初始化一次之后就再也不变更的字符串
//---------------------------------------------------------
// 作者:		喻强
// 创建:		2012-06-30
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _FIXED_STRING_H__
#define _FIXED_STRING_H__
#include <NiFixedString.h>

namespace RenderSystem
{

	class sdFixedString: public NiFixedString
	{
	public:
		sdFixedString();
		sdFixedString(const char* szString);
		sdFixedString(const char* szString, bool bNeverRelease);
		sdFixedString(sdFixedString& szFixedString);
		sdFixedString(NiFixedString& szFixedString);
		~sdFixedString();

		static void Shutdown();

	protected:
		bool	m_bNeverRelease;

		static std::vector<sdFixedString*>	ms_szvDelayReleaseList;
	};

}
#endif
