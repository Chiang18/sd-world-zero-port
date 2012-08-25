//*************************************************************************************************
// 内容: 整个Engine库的公共头文件
//---------------------------------------------------------
// 作者:	
// 创建:		2012-08-15
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_H__
#define _SD_ENGINE_H__


//*********************************************************
// PhysX头文件
//*********************************************************
#pragma warning(push) 
#pragma warning(disable:4819)
#include <NxUserOutputStream.h>
#include <NxPhysicsSDK.h>
#include <NxCooking.h>
#include <PhysXLoader.h>
#pragma warning(pop) 

//*********************************************************
// GB头文件
//*********************************************************
#include <NiSystem.h>
#include <NiMain.h>
#include <NiMesh.h>
#include <NiAnimation.h>
#include <NiDX9Renderer.h>
#include <NiViewMath.h>

//*********************************************************
// windows 头文件
//*********************************************************
#include <windows.h>

//*********************************************************
// C头文件
//*********************************************************
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//*********************************************************
// C++头文件
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
// 依赖库文件
//*********************************************************
#include <sdBase.h>

using namespace Base;

#endif