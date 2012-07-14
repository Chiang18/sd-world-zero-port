//*************************************************************************************************
// 内容:	Win32 File的一些功能
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-13
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _UTIL_H__
#define _UTIL_H__

class sdFileUtil
{
public:


	
	// 从绝对路径文件名中解析路径(末尾不带斜杠)
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

	// 取当前exe文件路径(末尾不带斜杠)
	static std::string GetModulePath()
	{
		char szPath[MAX_PATH + 1];
		::GetModuleFileName(NULL, szPath, MAX_PATH);
		return ExtractPath(szPath);
	};

	// 取当前exe/dll文件名(不带路径)
	//static string GetModuleName()
	//{
	//	char szPath[MAX_PATH + 1];
	//	::GetModuleFileName(NULL, szPath, MAX_PATH);
	//	return Extract
	//}

};

#endif