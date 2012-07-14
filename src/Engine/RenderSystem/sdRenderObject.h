//*************************************************************************************************
// ����:	��Ⱦ�����װ
//---------------------------------------------------------
// ����:		��ǿ
// ����:		2012-06-30
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _RENDER_OBJECT_H__
#define _RENDER_OBJECT_H__
#include "sdTypes.h"

namespace RenderSystem
{

	class sdFixedString;

//***************************************************************************************
// ��Ⱦ����
//	1.��װ����Ⱦ����Ⱦһ��Mesh����Ҫ����Ϣ
//	2.������Ⱦ�����ݵ�ǰ�������Ⱦ����������Ⱦ
// (ע�����ߴ�,����Memory/Cache����)
// (ע�ⲻҪ�ڶ����ڲ������ڴ�,��Ϊ����������Զ���ᱻ����)
class sdRenderObject: public NiMemObject
{
public:
	// ��Ⱦ��������
	enum eRenderObjectType
	{
		E_STATIC_MESH,
		E_LIGHT_VOLUME,
		E_LOCALLIGHT_VOLUME,
		E_PROJECTOR_VOLUME,
		E_UNKNOWN,
	};

	// ��ǰShader�������(���Դ���NiExtraData�Ͳ���NiProperty)
	struct ShaderParamItem
	{
		const sdFixedString*	szName;
		unsigned int			uiByteSize;
		const void*				pData;
	};

	// ��Ⱦ��������ؼ���
	struct RenderSortKey
	{
		union
		{
			uint	key0, key1;	// ����RenderObject��
			uint	key2, key3;	// ����RenderObject��չ��
		};
		uint	keys[4];
	};

	sdRenderObject(eRenderObjectType eType, NiMesh* spMesh, NiMaterial* spMaterial, 
		NiPropertyList* spRenderProps, ShaderParamItem* pkShaderParams);

	const eRenderObjectType GetType() const { return m_eType;};
	const RenderSortKey& GetSortKey() const { return m_kRenderKey;};
	const NiMesh* GetMesh()	const { return m_spMesh;};

	//*****************************************************
	// ���ƽӿ�
	virtual void Draw() = 0;

protected:
	// ��Ⱦǰ�����,����ʹ��ָ����Ⱦ�������滻��Ĭ������
	// �����Ϳ���ʹͬһ��Geometry���ڶ�����Ⱦ���,��ʵ���ڸĵ��˲���Render
	void ApplyRenderSetting();
	void RestoreRenderSetting();

	// 
	virtual void ApplyRenderProps();
	virtual void ApplyTexturingProp();
	virtual void RestoreRenderProps();
	virtual void RestoreTexturingProp();

	virtual void ApplyMaterial();
	virtual void RestoreMaterial();

	virtual void SetShaderParams();
	virtual void SetRenderStates();

	//
	NiProperty*	FindProperty(NiPropertyList* spPropList, int iPropType);

protected:
	eRenderObjectType	m_eType;
	NiMesh*				m_spMesh;
	NiMaterial*			m_spMaterial;
	NiPropertyList*		m_spProperties;
	ShaderParamItem*	m_pkShaderParams;
	//unsigned int		m_uiOcclusionId;

	// ����ؼ���
	RenderSortKey		m_kRenderKey;

	// �м����
	NiMaterial*				m_spOldMaterial;
	NiTexturingPropertyPtr	m_spOldTexturingProp;
};

//***************************************************************************************
// StaticMesh
class StaticMeshRenderObject : public sdRenderObject
{
public:
	StaticMeshRenderObject(NiMesh* spMesh, NiMaterial* spMaterial, NiPropertyList* spRenderProps, ShaderParamItem* pkShaderParams);

	// �麯���̳�
	virtual void Draw();
};

//***************************************************************************************
inline bool operator == (const sdRenderObject::RenderSortKey& lhs, const sdRenderObject::RenderSortKey& rhs)
{
	return 0 == memcmp(lhs.keys, rhs.keys, sizeof(lhs.keys));
}
//---------------------------------------------------------------------------------------
inline bool operator != (const sdRenderObject::RenderSortKey& lhs, const sdRenderObject::RenderSortKey& rhs)
{
	return 0 != memcmp(lhs.keys, rhs.keys, sizeof(lhs.keys));
}
//---------------------------------------------------------------------------------------
inline bool operator < (const sdRenderObject::RenderSortKey& lhs, const sdRenderObject::RenderSortKey& rhs)
{
	if (lhs.key0 != rhs.key0)	return lhs.key0 < rhs.key0;
	if (lhs.key1 != rhs.key1)	return lhs.key1 < rhs.key1;
	if (lhs.key2 != rhs.key2)	return lhs.key2 < rhs.key2;
	if (lhs.key3 != rhs.key3)	return lhs.key3 < rhs.key3;

	return false;
}
//---------------------------------------------------------------------------------------
inline bool operator > (const sdRenderObject::RenderSortKey& lhs, const sdRenderObject::RenderSortKey& rhs)
{
	if (lhs.key0 != rhs.key0)	return lhs.key0 > rhs.key0;
	if (lhs.key1 != rhs.key1)	return lhs.key1 > rhs.key1;
	if (lhs.key2 != rhs.key2)	return lhs.key2 > rhs.key2;
	if (lhs.key3 != rhs.key3)	return lhs.key3 > rhs.key3;

	return false;
}
//***************************************************************************************
}
#endif
