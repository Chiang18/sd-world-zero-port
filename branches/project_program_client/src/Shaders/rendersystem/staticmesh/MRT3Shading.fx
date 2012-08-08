//*************************************************************************************************
// 内容:	StaticMesh的MRTShadingPass着色器,2种不同的具体着色器
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-19
// 最后修改:
//*************************************************************************************************
#define CHANNELS_FLAG	0x00000000
#include "MRT3ShadingImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000008
#include "MRT3ShadingImplement.h"
#undef 	CHANNELS_FLAG