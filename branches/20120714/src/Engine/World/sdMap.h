//*************************************************************************************************
// 内容:	场景地图
//---------------------------------------------------------
// 作者:		
// 创建:		2012-06-29
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _MAP_H__
#define _MAP_H__
#include "sdGameObject.h"

// 前置声明
namespace RenderSystem
{
	class sdRenderParams;
}

// 场景地图
class sdMap: public sdGameObject
{

public:

	//*********************
	NiAVObjectPtr	m_pkRoot;
	//**********************

protected:

protected:



};
NiSmartPointer(sdMap);
#endif