#include "Stdafx.h"
#include "sdTerrainStream.h"

//
#include <sdFileUtil.h>

//
#include <NiDX9TextureData.h>

#define TERRAIN_DIRECTORY					"terrain"		// 地形数据相对目录
#define TERRAIN_HEIGHTMAP_FILENAME			"heightmap.raw"	// 地表高度图
#define TERRAIN_HEIGHTMAP_CONFIG_FILENAME	"heightmap.lua"	// 地表高度图配置文件
#define TERRAIN_NORMALMAP_FILENAME			"normalmap.raw"	// 地表法线图(仅用于游戏)
#define TERRAIN_LAYERMAP_FILENAME			"layermap.bin"	// 地表纹理混合信息(仅用于编辑器)
#define	TERRAIN_LAYERMAP_LIST_FILENAME		"layermap.lua"	// 地表纹理列表	
#define TERRAIN_BLENDMAP_FILENAME			"blendmap.dds"	// 地表混合贴图(仅用于游戏)
#define TERRAIN_TILEMAP_FILENAME			"tilemap.dds"	// 地表混合索引贴图(仅用于游戏)
#define	TERRAIN_DIFFUSE_ATLASMAP_FILENAME	"diffuseatlasmap.dds"	// (仅用于游戏)
#define TERRAIN_DIFFUSE_ATLASTABLE_FILENAME	"diffuseatlastable.dds"	// (仅用于游戏)
#define	TERRAIN_NORMAL_ATLASMAP_FILENAME	"normalatlasmap.dds"	// (仅用于游戏)
#define TERRAIN_NORMAL_ATLASTABLE_FILENAME	"normalatlastable.dds"	// (仅用于游戏)

using namespace Base;
//-------------------------------------------------------------------------------------------------
bool sdTerrainStream::SaveHeightMap(sdHeightMap* pkHeightMap, const char* szSceneFullPath)
{
	NIASSERT(pkHeightMap);;
	NIASSERT(szSceneFullPath);

	// 构建文件夹路径
	char szBuffer[MAX_PATH];
	memset(szBuffer, 0, MAX_PATH * sizeof(char));
	strcpy(szBuffer, szSceneFullPath);
	strcat(szBuffer, "\\");
	strcat(szBuffer, TERRAIN_DIRECTORY);

	// 确定路径存在
	sdFileUtil::ConfimDir(szBuffer);

	
	// 保存高度图lua配置文件
	// @{
	
	// @}


	// 保存高度图raw高度信息文件
	// @{
	// 路径
	char szRawFullPath[MAX_PATH];
	strcpy(szRawFullPath, szBuffer);
	strcat(szRawFullPath, "\\");
	strcat(szRawFullPath, TERRAIN_HEIGHTMAP_FILENAME);

	// 创建文件
	NiFile* pkFile = NiFile::GetFile(szRawFullPath, NiFile::WRITE_ONLY);
	NIASSERT(pkFile)

	// 提取数据,写入文件
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

	// 保存文件
	NiDelete pkFile;
	pkFile = NULL;
	// @}

	return true;
}
//-------------------------------------------------------------------------------------------------
sdHeightMap* sdTerrainStream::LoadHeightMap(const char* szSceneFullPath)
{
	NIASSERT(szSceneFullPath);

	// 构建文件夹路径
	char szBuffer[MAX_PATH];
	memset(szBuffer, 0, MAX_PATH * sizeof(char));
	strcpy(szBuffer, szSceneFullPath);
	strcat(szBuffer, "\\");
	strcat(szBuffer, TERRAIN_DIRECTORY);

	// 读取高度图lua配置文件
	// @{
	uint uiSize = 512;
	// @}


	// 读取高度图raw高度信息文件
	// @{
	// 路径
	char szRawFullPath[MAX_PATH];
	strcpy(szRawFullPath, szBuffer);
	strcat(szRawFullPath, "\\");
	strcat(szRawFullPath, TERRAIN_HEIGHTMAP_FILENAME);

	// 打开文件,读取文件数据
	NiFile* pkFile = NiFile::GetFile(szRawFullPath, NiFile::READ_ONLY);
	NIASSERT(pkFile)

	float* pfHeight = new float[uiSize * uiSize];
	pkFile->Read(pfHeight, uiSize * uiSize);

	NiDelete pkFile;
	pkFile = NULL;

	// 设置高度图
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
bool sdTerrainStream::SaveBlendMap(NiTexture* spBlendMap, const char *szSceneFullPath)
{
	NIASSERT(spBlendMap);;
	NIASSERT(szSceneFullPath);

	// 构建文件夹路径
	char szBuffer[MAX_PATH];
	memset(szBuffer, 0, MAX_PATH * sizeof(char));
	strcpy(szBuffer, szSceneFullPath);
	strcat(szBuffer, "\\");
	strcat(szBuffer, TERRAIN_DIRECTORY);

	// 确定路径存在
	sdFileUtil::ConfimDir(szBuffer);


	// 保存
	// @{
	// 路径
	char szRawFullPath[MAX_PATH];
	strcpy(szRawFullPath, szBuffer);
	strcat(szRawFullPath, "\\");
	strcat(szRawFullPath, TERRAIN_BLENDMAP_FILENAME);

	// 获取D3D纹理
	NiDX9TextureData* spDX9TextureData = (NiDX9TextureData*)spBlendMap->GetRendererData();
	NIASSERT(spDX9TextureData);

	LPDIRECT3DBASETEXTURE9 spD3DBaseTexture = (LPDIRECT3DBASETEXTURE9)spDX9TextureData->GetD3DTexture();
	NIASSERT(spD3DBaseTexture);

	// 保存D3D纹理
	D3DXSaveTextureToFile(szRawFullPath, D3DXIFF_DDS, spD3DBaseTexture, NULL);
	// @}

	return true;
}
//-------------------------------------------------------------------------------------------------