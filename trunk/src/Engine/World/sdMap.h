//*************************************************************************************************
// ����:	������ͼ
//---------------------------------------------------------
// ����:		
// ����:		2012-06-29
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _MAP_H__
#define _MAP_H__
#include "sdGameObject.h"

// ǰ������
namespace RenderSystem
{
	class sdRenderParams;
}

// ������ͼ
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