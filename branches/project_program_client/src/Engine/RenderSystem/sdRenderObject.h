//*************************************************************************************************
// 内容:	渲染对象封装
//---------------------------------------------------------
// 作者:		喻强
// 创建:		2012-06-30
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _RENDER_OBJECT_H__
#define _RENDER_OBJECT_H__
#include "sdTypes.h"

namespace RenderSystem
{

	class sdFixedString;

//***************************************************************************************
// 渲染对象
//	1.封装了渲染器渲染一个Mesh所需要的信息
//	2.调用渲染器根据当前保存的渲染参数进行渲染
// (注意对象尺寸,考虑Memory/Cache对齐)
// (注意不要在对象内部分配内存,因为析构函数永远不会被调用)
class sdRenderObject: public NiMemObject
{
public:
	// 渲染对象类型
	enum eRenderObjectType
	{
		E_STATIC_MESH,
		E_LIGHT_VOLUME,
		E_LOCALLIGHT_VOLUME,
		E_PROJECTOR_VOLUME,
		E_UNKNOWN,
	};

	// 当前Shader所需参数(用以代替NiExtraData和部分NiProperty)
	struct ShaderParamItem
	{
		const sdFixedString*	szName;
		unsigned int			uiByteSize;
		const void*				pData;
	};

	// 渲染对象排序关键字
	struct RenderSortKey
	{
		union
		{
			uint	key0, key1;	// 用于RenderObject类
			uint	key2, key3;	// 用于RenderObject扩展类
		};
		uint	keys[4];
	};

	sdRenderObject(eRenderObjectType eType, NiMesh* spMesh, NiMaterial* spMaterial, 
		NiPropertyList* spRenderProps, ShaderParamItem* pkShaderParams);

	const eRenderObjectType GetType() const { return m_eType;};
	const RenderSortKey& GetSortKey() const { return m_kRenderKey;};
	const NiMesh* GetMesh()	const { return m_spMesh;};

	//*****************************************************
	// 绘制接口
	virtual void Draw() = 0;

protected:
	// 渲染前后调用,用于使用指定渲染属性来替换掉默认属性
	// 这样就可以使同一个Geometry用于多种渲染情况,其实等于改掉了部分Render
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

	// 排序关键字
	RenderSortKey		m_kRenderKey;

	// 中间变量
	NiMaterial*				m_spOldMaterial;
	NiTexturingPropertyPtr	m_spOldTexturingProp;
};

//***************************************************************************************
// StaticMesh
class StaticMeshRenderObject : public sdRenderObject
{
public:
	StaticMeshRenderObject(NiMesh* spMesh, NiMaterial* spMaterial, NiPropertyList* spRenderProps, ShaderParamItem* pkShaderParams);

	// 虚函数继承
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
