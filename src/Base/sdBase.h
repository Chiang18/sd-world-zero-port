//*************************************************************************************************
// 内容: 整个Base库的公共头文件(本文件抽取自Ogre/OgrePrerequisites.h和Ogre/OgreStdHeaders.h)
//---------------------------------------------------------
// 作者:	
// 创建:		2012-08-04
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_BASE_H__
#define _SD_BASE_H__

//*********************************************************
// windows 头文件
//*********************************************************
#include <windows.h>

//*********************************************************
// C头文件
//*********************************************************
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>

//*********************************************************
// C++头文件
//*********************************************************
// 数据流
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

// 容器
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <set>
#include <map>
#include <hash_set>
#include <hash_map>

// 算法
#include <algorithm>
#include <functional>
#include <utility>
#include <xlocale>
#include <limits>

//*********************************************************
// 当前库文件
//*********************************************************
#include "sdConfig.h"
#include "sdPlatform.h"
#include "sdTypes.h"

#endif