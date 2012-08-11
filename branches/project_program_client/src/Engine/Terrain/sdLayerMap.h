//*************************************************************************************************
// 内容:	地表资源图层
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-11
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _TERRAIN_LAYER_MAP_H__
#define _TERRAIN_LAYER_MAP_H__
#include "sdTypes.h"

// 表示地表一层资源贴图(仅仅在编辑器状态下使用,相当于Cry的SSurfaceType)
class sdLayerMap : public NiRefObject
{
public:
	sdLayerMap(const std::string& szName, const std::string& szDiffuseMap, 
		const std::string& szNormalMap, float fRepeatSize = 1.0f, float fMipmapBias = 0.0f);
	~sdLayerMap();

	//
	const std::string&	GetName() const { return m_szName;}

	const std::string&	GetDiffuseMapPath() const { return m_szDiffuseMap;}
	void				SetDiffuseMapPath(const std::string& szName);

	const std::string&	GetNormalMapPath() const { return m_szNormalMap;}
	void				SetNormalMapPath(const std::string& szName);

	NiTexture*	GetDiffuseMap() { return m_spDiffuseMap;}
	NiTexture*	GetNormalMap() { return m_spNormalMap;}

	float	GetRepeatSize() const { return m_fRepeatSize;}
	void	SetRepeatSize(float fRepeatSize) { m_fRepeatSize = fRepeatSize; }

	float	GetMipmapBias() const { return m_fMipmapBias;};
	void	SetMipmapBias(float fMipmapBias) { m_fMipmapBias = fMipmapBias; }

protected:
	// 图层名
	std::string		m_szName;			

	// 图层纹理
	std::string		m_szDiffuseMap;	
	std::string		m_szNormalMap;	
	NiTexturePtr	m_spDiffuseMap;
	NiTexturePtr	m_spNormalMap;

	// 图层信息
	float			m_fRepeatSize;
	float			m_fMipmapBias;
};
NiSmartPointer(sdLayerMap);
#endif