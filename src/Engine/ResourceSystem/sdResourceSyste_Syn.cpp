#include "Stdafx.h"
#include "sdResourceSystem.h"

namespace ResourceSystem
{
//-------------------------------------------------------------------------------------------------
NiSourceTexturePtr sdResourceSystem::LoadTexture(const std::string& szFileName, bool bMipmap)
{
	// ��ȡ����·��
	NiFixedString kFileName(szFileName.c_str());

	// ���·���Ϸ���


	// ��������
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
	// ��ȡ����·��
	NiFixedString kFileName(szFileName.c_str());

	// ���·���Ϸ���


	// ��������
	NiImageConverter* pkImageConverter = NiDevImageConverter::GetImageConverter();
	NIASSERT(pkImageConverter);

	NiPixelData* pkPixelData = pkImageConverter->ReadImageFile(szFileName.c_str(), NULL);
	NIASSERT(pkPixelData); 

	return pkPixelData;
}
//-------------------------------------------------------------------------------------------------
}