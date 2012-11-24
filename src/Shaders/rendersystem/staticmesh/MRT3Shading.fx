//*************************************************************************************************
// 内容:	StaticMesh的MRT3ShadingPass着色器
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-19
// 最后修改:
//*************************************************************************************************
#define CHANNELS_FLAG	0x00000000
#include "MRT3ShadingImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000800
#include "MRT3ShadingImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00001000
#include "MRT3ShadingImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00001800
#include "MRT3ShadingImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00002000
#include "MRT3ShadingImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00002800
#include "MRT3ShadingImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00003000
#include "MRT3ShadingImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00003800
#include "MRT3ShadingImplement.h"
#undef 	CHANNELS_FLAG