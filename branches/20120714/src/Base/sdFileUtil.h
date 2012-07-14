//*************************************************************************************************
// ����:	Win32 File��һЩ����
//---------------------------------------------------------
// ����:		
// ����:		2012-07-13
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _UTIL_H__
#define _UTIL_H__

class sdFileUtil
{
public:


	
	// �Ӿ���·���ļ����н���·��(ĩβ����б��)
	static std::string ExtractPath(const std::string& szPathFileName)
	{
		int iNum = (int)szPathFileName.length() - 1;
		while (iNum > 0)
		{
			if (szPathFileName[iNum] == '\\' || szPathFileName[iNum] == '/')
				return szPathFileName.substr(0, iNum);
			
			--iNum;
		}

		return "";
	};

	// ȡ��ǰexe�ļ�·��(ĩβ����б��)
	static std::string GetModulePath()
	{
		char szPath[MAX_PATH + 1];
		::GetModuleFileName(NULL, szPath, MAX_PATH);
		return ExtractPath(szPath);
	};

	// ȡ��ǰexe/dll�ļ���(����·��)
	//static string GetModuleName()
	//{
	//	char szPath[MAX_PATH + 1];
	//	::GetModuleFileName(NULL, szPath, MAX_PATH);
	//	return Extract
	//}

};

#endif