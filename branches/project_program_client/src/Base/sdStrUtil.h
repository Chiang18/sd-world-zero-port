//*************************************************************************************************
// ����:	�ַ����ĸ�������һЩ����
//---------------------------------------------------------
// ����:		
// ����:		2012-07-09
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _STR_UTIL_H__
#define _STR_UTIL_H__

namespace Base
{

	class sdStrUtil
	{
	public:
			

		// ��ʽ���ַ����������string
		static std::string Format(const char* fmt, ...)
		{
			char buff[1024] = "";
			va_list ap;
			va_start(ap, fmt);
			vsprintf(buff, fmt, ap);	
			//vsnprintf(buff, 1024, fmt, ap);
			//::StringCchVPrintf(buff, 1024, fmt, ap);
			va_end(ap);

			return buff;
		};

	};

}
#endif