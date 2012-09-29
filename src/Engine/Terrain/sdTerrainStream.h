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
	static bool SaveHeightMap(sdHeightMap* pkHeightMap, const char* szSceneFullPath);
	static sdHeightMap* LoadHeightMap(const char* szSceneFullPath);

	// ���غͱ�������ͼ
	// (����ʹ��D3DX������,WZԭʼ��ʽ���Լ��ֹ�����)
	static bool SaveBlendMap(NiTexture* spBlendMap, const char* szSceneFullPath);

	// ���غͱ�����������ͼ������������ͼ������
	static bool SaveDiffuseAtlasMap(NiTexture* spDiffuseMap, const char* szSceneFullPath);
	static bool SaveDiffuseAtlasTableMap(NiTexture* spDiffuseTableMap, const char* szSceneFullPath);

	// ���غͱ�������ͼ������ͼ����
	static bool SaveNormalAtlasMap(NiTexture* spNormalMap, const char* szSceneFullPath);
	static bool SaveNormalAtlasTableMap(NiTexture* spNormalTableMap, const char* szSceneFullPath);
};
NiSmartPointer(sdTerrainStream);
#endif