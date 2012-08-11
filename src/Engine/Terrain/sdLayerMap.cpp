#include "Stdafx.h"
#include "sdLayerMap.h"

using namespace std;
//-------------------------------------------------------------------------------------------------
sdLayerMap::sdLayerMap(const std::string& szName, const std::string& szDiffuseMap, const std::string& szNormalMap, float fRepeatSize, float fMipmapBias)
: m_szName(szName)
, m_szDiffuseMap(szDiffuseMap)
, m_szNormalMap(szNormalMap)
, m_fRepeatSize(fRepeatSize)
, m_fMipmapBias(fMipmapBias)
{
	SetDiffuseMapPath(szDiffuseMap);
	SetNormalMapPath(szNormalMap);
}
//-------------------------------------------------------------------------------------------------
sdLayerMap::~sdLayerMap()
{
	
}
//-------------------------------------------------------------------------------------------------
void sdLayerMap::SetDiffuseMapPath(const std::string& szName)
{
	if (szName == "")
	{
		m_szDiffuseMap = "";
		m_spDiffuseMap = 0;
		return;
	}

	//
}
//-------------------------------------------------------------------------------------------------
void sdLayerMap::SetNormalMapPath(const std::string& szName)
{
	if (szName == "")
	{
		m_szNormalMap = "";
		m_spNormalMap = 0;
		return;
	}

	//
}
//-------------------------------------------------------------------------------------------------