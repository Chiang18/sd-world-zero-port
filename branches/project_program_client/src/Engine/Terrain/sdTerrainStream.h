//*************************************************************************************************
// ����:	���������������뱣��
//---------------------------------------------------------
// ����:		
// ����:		2012-09-05
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_TERRAIN_STREAM_H__
#define _SD_ENGINE_TERRAIN_STREAM_H__
#include "sdEngine.h"
#include "sdHeightMap.h"

// ���ڶ����ı�������ص�������
// (���ǸĽ�ΪGB��Stream��ʽ)
class sdTerrainStream : public NiRefObject
{
public:
	// �����뱣��߶�ͼ
	static bool SaveHeightMap(sdHeightMap* pkHeightMap, const std::string& szSceneFullPath);
	static sdHeightMap* LoadHeightMap(const std::string& szSceneFullPath);

};
NiSmartPointer(sdTerrainStream);
#endif