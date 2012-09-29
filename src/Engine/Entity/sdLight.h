//*************************************************************************************************
// 内容:	场景光源对象
//---------------------------------------------------------
// 作者:		
// 创建:		2012-09-27
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_LIGHT_H__
#define _SD_ENGINE_LIGHT_H__
#include "sdEntity.h"

class sdLight : public sdEntity
{
public:
	// 灯光类型
	enum ELightType
	{
		E_LT_AMBIENT,	// 环境光
		E_LT_POINT,		// 点光源
		E_LT_DIR,		// 方向光
		E_LT_SPOT,		// 聚光灯
		E_LT_DUALDIR,	// 双向方向光
		E_LT_BOX,		// 盒式光照
		NUM_LIGHTTYPES = E_LT_BOX,
	};

public:
	sdLight(ELightType eLightType);
	virtual ~sdLight();

	ELightType	GetType() const;
	NiLight*	GetGBLight();

	// 通用光照参数设置
	// @{
	float	GetDimmer() const;
	void	SetDimmer(float fDimmer);

	const NiColor& GetAmbientColor() const;
	const NiColor& GetDiffuseColor() const;
	const NiColor& GetSpecularColor() const;

	void SetAmbientColor(const NiColor& kColor);
	void SetDiffuseColor(const NiColor& kColor);
	void SetSpecularColor(const NiColor& kColor);
	// @}

protected:
	// 虚函数继承
	virtual void UpdateWorldBound();
	virtual void UpdateDownwardPass(NiUpdateProcess& kUpdate);

protected:
	// 灯光类型
	ELightType	m_eLightType;

	// GB Light
	NiLightPtr	m_spLight;

	// 控制灯光随世间变化
	//sdLightModify(sdBezierTable)

	// 延迟光照的绘制Mesh
	NiMeshPtr	m_spDummyMesh;
};
NiSmartPointer(sdLight);
#include "sdLight.inl"
#endif