//*************************************************************************************************
// 内容:	StaticMesh的MRTZGeometryPass着色器,共有2^6 = 64种不同的具体着色器
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-14
// 最后修改:
//*************************************************************************************************
#define CHANNELS_FLAG	0x00000000
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000001
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000002
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000003
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000004
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000005
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000006
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000007
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000008
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000009
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000000a
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000000b
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000000c
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000000d
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000000e
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000000f
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000010
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000011
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000012
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000013
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000014
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000015
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000016
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000017
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000018
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000019
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000001a
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000001b
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000001c
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000001d
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000001e
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000001f
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000020
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000021
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000022
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000023
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000024
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000025
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000026
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000027
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000028
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000029
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000002a
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000002b
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000002c
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000002d
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000002e
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000002f
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000030
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000031
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000032
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000033
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000034
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000035
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000036
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000037
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000038
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x00000039
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000003a
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000003b
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000003c
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000003d
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000003e
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG

#define CHANNELS_FLAG	0x0000003f
#include "MRT3GeometryImplement.h"
#undef 	CHANNELS_FLAG