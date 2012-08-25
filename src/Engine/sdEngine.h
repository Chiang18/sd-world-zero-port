//*************************************************************************************************
// ����: ����Engine��Ĺ���ͷ�ļ�
//---------------------------------------------------------
// ����:	
// ����:		2012-08-15
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_H__
#define _SD_ENGINE_H__


//*********************************************************
// PhysXͷ�ļ�
//*********************************************************
#pragma warning(push) 
#pragma warning(disable:4819)
#include <NxUserOutputStream.h>
#include <NxPhysicsSDK.h>
#include <NxCooking.h>
#include <PhysXLoader.h>
#pragma warning(pop) 

//*********************************************************
// GBͷ�ļ�
//*********************************************************
#include <NiSystem.h>
#include <NiMain.h>
#include <NiMesh.h>
#include <NiAnimation.h>
#include <NiDX9Renderer.h>
#include <NiViewMath.h>

//*********************************************************
// windows ͷ�ļ�
//*********************************************************
#include <windows.h>

//*********************************************************
// Cͷ�ļ�
//*********************************************************
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//*********************************************************
// C++ͷ�ļ�
//*********************************************************
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <set>
#include <map>
//#include <hashmap>
#include <algorithm>

//*********************************************************
// �������ļ�
//*********************************************************
#include <sdBase.h>

using namespace Base;

#endif