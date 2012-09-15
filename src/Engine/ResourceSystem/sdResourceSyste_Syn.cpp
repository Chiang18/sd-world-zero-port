#include "Stdafx.h"
#include "sdResourceSystem.h"

namespace ResourceSystem
{
//-------------------------------------------------------------------------------------------------
NiSourceTexturePtr sdResourceSystem::LoadTexture(const std::string& szFileName, bool bMipmap)
{
	// 获取完整路径
	NiFixedString kFileName(szFileName.c_str());

	// 检查路径合法性


	// 加载纹理
	NiSourceTexture::FormatPrefs kFormatPrefs;
	if (!bMipmap)
	{
		kFormatPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::SMOOTH;
		kFormatPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
	}

	NiSourceTexturePtr spTexture = NiSourceTexture::Create(kFileName, kFormatPrefs);
	NIASSERT(spTexture);

	return spTexture;
}
//-------------------------------------------------------------------------------------------------
NiPixelDataPtr sdResourceSystem::LoadTextureToMemory(const std::string& szFileName)
{
	// 获取完整路径
	NiFixedString kFileName(szFileName.c_str());

	// 检查路径合法性


	// 加载纹理
	NiImageConverter* pkImageConverter = NiDevImageConverter::GetImageConverter();
	NIASSERT(pkImageConverter);

	NiPixelData* pkPixelData = pkImageConverter->ReadImageFile(szFileName.c_str(), NULL);
	NIASSERT(pkPixelData); 

	return pkPixelData;
}
//-------------------------------------------------------------------------------------------------
}