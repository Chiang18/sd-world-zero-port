//*************************************************************************************************
// ����:	�ַ����ķ�װ,����ֻ��Ҫ��ʼ��һ��֮�����Ҳ��������ַ���
//---------------------------------------------------------
// ����:		��ǿ
// ����:		2012-06-30
// ����޸�:
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
