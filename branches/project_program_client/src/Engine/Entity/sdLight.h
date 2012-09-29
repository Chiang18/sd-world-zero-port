//*************************************************************************************************
// ����:	������Դ����
//---------------------------------------------------------
// ����:		
// ����:		2012-09-27
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_LIGHT_H__
#define _SD_ENGINE_LIGHT_H__
#include "sdEntity.h"

class sdLight : public sdEntity
{
public:
	// �ƹ�����
	enum ELightType
	{
		E_LT_AMBIENT,	// ������
		E_LT_POINT,		// ���Դ
		E_LT_DIR,		// �����
		E_LT_SPOT,		// �۹��
		E_LT_DUALDIR,	// ˫�����
		E_LT_BOX,		// ��ʽ����
		NUM_LIGHTTYPES = E_LT_BOX,
	};

public:
	sdLight(ELightType eLightType);
	virtual ~sdLight();

	ELightType	GetType() const;
	NiLight*	GetGBLight();

	// ͨ�ù��ղ�������
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
	// �麯���̳�
	virtual void UpdateWorldBound();
	virtual void UpdateDownwardPass(NiUpdateProcess& kUpdate);

protected:
	// �ƹ�����
	ELightType	m_eLightType;

	// GB Light
	NiLightPtr	m_spLight;

	// ���Ƶƹ�������仯
	//sdLightModify(sdBezierTable)

	// �ӳٹ��յĻ���Mesh
	NiMeshPtr	m_spDummyMesh;
};
NiSmartPointer(sdLight);
#include "sdLight.inl"
#endif