#include "Stdafx.h"
#include "sdTerrainStream.h"

//
#include <sdFileUtil.h>

#define TERRAIN_DIRECTORY					"terrain"		// �����������Ŀ¼
#define TERRAIN_HEIGHTMAP_FILENAME			"heightmap.raw"	// �ر�߶�ͼ
#define TERRAIN_HEIGHTMAP_CONFIG_FILENAME	"heightmap.lua"	// �ر�߶�ͼ�����ļ�
#define TERRAIN_NORMALMAP_FILENAME			"normalmap.raw"	// �ر���ͼ(��������Ϸ)
#define TERRAIN_LAYERMAP_FILENAME			"layermap.bin"	// �ر���������Ϣ(�����ڱ༭��)
#define	TERRAIN_LAYERMAP_LIST_FILENAME		"layermap.lua"	// �ر������б�	
#define TERRAIN_BLENDMAP_FILENAME			"blendmap.dds"	// �ر�����ͼ(��������Ϸ)
#define TERRAIN_TILEMAP_FILENAME			"tilemap.dds"	// �ر���������ͼ(��������Ϸ)
#define	TERRAIN_DIFFUSE_ATLASMAP_FILENAME	"diffuseatlasmap.dds"	// (��������Ϸ)
#define TERRAIN_DIFFUSE_ATLASTABLE_FILENAME	"diffuseatlastable.dds"	// (��������Ϸ)
#define	TERRAIN_NORMAL_ATLASMAP_FILENAME	"normalatlasmap.dds"	// (��������Ϸ)
#define TERRAIN_NORMAL_ATLASTABLE_FILENAME	"normalatlastable.dds"	// (��������Ϸ)

using namespace Base;
//-------------------------------------------------------------------------------------------------
bool sdTerrainStream::SaveHeightMap(sdHeightMap* pkHeightMap, const std::string& szSceneFullPath)
{
	NIASSERT(pkHeightMap);;
	NIASSERT(!szSceneFullPath.empty());

	// �����ļ���·��
	char szBuffer[MAX_PATH];
	memset(szBuffer, 0, MAX_PATH * sizeof(char));
	strcpy(szBuffer, szSceneFullPath.c_str());
	strcat(szBuffer, "\\");
	strcat(szBuffer, TERRAIN_DIRECTORY);

	// ȷ��·������
	sdFileUtil::ConfimDir(szBuffer);

	
	// ����߶�ͼlua�����ļ�
	// @{
	
	// @}


	// ����߶�ͼraw�߶���Ϣ�ļ�
	// @{
	// ·��
	char szRawFullPath[MAX_PATH];
	strcpy(szRawFullPath, szBuffer);
	strcat(szRawFullPath, "\\");
	strcat(szRawFullPath, TERRAIN_HEIGHTMAP_FILENAME);

	// �����ļ�
	NiFile* pkFile = NiFile::GetFile(szRawFullPath, NiFile::WRITE_ONLY);
	NIASSERT(pkFile)

	// ��ȡ����,д���ļ�
	uint uiSize = pkHeightMap->GetSize();
	float* pfHeight = new float[uiSize * uiSize];
	float* pfCurHeight = pfHeight;
	for (uint uiY = 0; uiY < uiSize; ++uiY)
	{
		for (uint uiX = 0; uiX < uiSize; ++uiX)
		{
			*pfCurHeight++ = pkHeightMap->GetRawHeight(uiX, uiY);
		}
	}

	pkFile->Write(pfHeight, uiSize * uiSize);
	pkFile->Flush();
	delete pfHeight;

	// �����ļ�
	NiDelete pkFile;
	pkFile = NULL;
	// @}

	return true;
}
//-------------------------------------------------------------------------------------------------
sdHeightMap* sdTerrainStream::LoadHeightMap(const std::string& szSceneFullPath)
{
	NIASSERT(!szSceneFullPath.empty());

	// �����ļ���·��
	char szBuffer[MAX_PATH];
	memset(szBuffer, 0, MAX_PATH * sizeof(char));
	strcpy(szBuffer, szSceneFullPath.c_str());
	strcat(szBuffer, "\\");
	strcat(szBuffer, TERRAIN_DIRECTORY);

	// ��ȡ�߶�ͼlua�����ļ�
	// @{
	uint uiSize = 512;
	// @}


	// ��ȡ�߶�ͼraw�߶���Ϣ�ļ�
	// @{
	// ·��
	char szRawFullPath[MAX_PATH];
	strcpy(szRawFullPath, szBuffer);
	strcat(szRawFullPath, "\\");
	strcat(szRawFullPath, TERRAIN_HEIGHTMAP_FILENAME);

	// ���ļ�,��ȡ�ļ�����
	NiFile* pkFile = NiFile::GetFile(szRawFullPath, NiFile::READ_ONLY);
	NIASSERT(pkFile)

	float* pfHeight = new float[uiSize * uiSize];
	pkFile->Read(pfHeight, uiSize * uiSize);

	NiDelete pkFile;
	pkFile = NULL;

	// ���ø߶�ͼ
	sdHeightMap* pkHeightMap = NiNew sdHeightMap(uiSize);
	NIASSERT(pkHeightMap);
	float* pfCurHeight = pfHeight;
	for (uint uiY = 0; uiY < uiSize; ++uiY)
	{
		for (uint uiX = 0; uiX < uiSize; ++uiX)
		{
			pkHeightMap->SetRawHeight(uiX, uiY, *pfCurHeight++);
		}
	}
	delete pfHeight;
	pfHeight = NULL;
	// @}

	return pkHeightMap;
}
//-------------------------------------------------------------------------------------------------