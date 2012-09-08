//*************************************************************************************************
// 内容:	csv文件加载与保存
//---------------------------------------------------------
// 作者:		
// 创建:		2012-09-05
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_BASE_FILE_CSV_H__
#define _SD_BASE_FILE_CSV_H__
#include "sdBase.h"

namespace Base
{
	namespace File
	{
		class sdCsvFile
		{
		public:
			virtual ~sdCsvFile(){};

			// 打开文件
			void	Open(const std::string& szFullPath){};
		};
	}
}
#endif