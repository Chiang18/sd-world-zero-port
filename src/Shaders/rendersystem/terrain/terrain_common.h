//*************************************************************************************************
// ����:	������ɫ���Ĺ�������
//---------------------------------------------------------
// ����:		
// ����:		2012-08-04
// ����޸�:
//*************************************************************************************************
#ifndef _TERRAIN_COMMON_H__
#define _TERRAIN_COMMON_H__

//
#include "../common.h"

// ����Tile������Ϣ
float4	a_vTerrainQuadParam 	: ATTRIBUTE;
#define	a_vQuadOrigin			float2(a_vTerrainQuadParam[0], a_vTerrainQuadParam[1])
#define	a_fQuadScale			a_vTerrainQuadParam[2]
#define	a_fQuadMorphFactor		a_vTerrainQuadParam[3]

// ���������С�ĵ���(���ڼ�����ε�UV����)
float2	g_vRecipTerrainSize		: GLOBAL;

// ���ν���Զƽ��ķֽ���
float	g_fTerrainFarStart		: GLOBAL;


// ���β�����Ϣ


// ������ͼ

#endif