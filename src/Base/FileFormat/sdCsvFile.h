//*************************************************************************************************
// ����:	csv�ļ������뱣��
//---------------------------------------------------------
// ����:		
// ����:		2012-09-05
// ����޸�:
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

			// ���ļ�
			void	Open(const std::string& szFullPath){};
		};
	}
}
#endif