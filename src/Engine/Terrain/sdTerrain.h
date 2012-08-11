//*************************************************************************************************
// ����:	����ϵͳ
//---------------------------------------------------------
// ����:		
// ����:		2012-07-31
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _TERRAIN_H__
#define _TERRAIN_H__
#include <sdSingleton.h>
#include "sdEntity.h"
#include "sdHeightMap.h"
#include "sdNormalMap.h"
#include "sdQuadNode.h"

// ��Ⱦϵͳ
#include "sdRenderSystem.h"
#include "sdDynamicTexture.h"

// ����ϵͳ,���ж���ӿھ��ɴ��౩¶��ȥ
//
// ���β���:
//	1.Tile����: �Ĳ�������,���м�ڵ㵽Ҷ�ӽڵ��Ԥ����Mesh,Node�ڲ�û��LOD
//			(���Կ������û���,��̬����Mesh���Ľ�Ч��)
//  2.Tile��Ե�ӷ�:����ȹ�ߵķ�ʽ���LOD��Ե�ӷ�,���������ǽڵ㵽��һ��QuadNode��������
//  3.Tile�ߴ�: 64*64��, 65*65��, ����ȹ��67*67�� 
//  3.Tile��Ⱦʱ���ж���ѹ��,�Լ������ݴ���
//			XY����UINT16��8�ֽں͵�8�ֽ�
//			Z�任Ϊ����С������
//  4.Tile��Ⱦʱʹ��TriangleStrip�����ж���,ʹ��Index, 
//		����GPU���㻺��Ĭ�ϴ�СΪ24���Ż�����,������Cache��С�ֳ�����
//		
// ������:
//	1.
class sdTerrain : public Base::sdTSingleton<sdTerrain>, public sdEntity
{
	friend class sdQuadMesh;
	friend class sdQuadNode;
public:
	sdTerrain();
	virtual ~sdTerrain();

	// 
	bool	CreateScene(uint uiTerrainSize, uint uiBlendTextureSize);
	void	DestroyScene();
	//bool	LoadScene();
	//void	SaveScene();
	bool	HasScene() { return m_bInitialized;};

	// ���βü�
	//(wz��װ��һ��TerrainTileEntity,ò��ûɶ��Ҫ,����ֱ�ӷ���Mesh)
	void	Cull(const NiCamera& kCamera, std::vector<NiMesh*>& kMeshVec);


	//
	bool	IsVisible() const { return m_bIsVisible;};

	uint	GetTileSize() const { return m_uiTileSize;};
	uint	GetMeshLevel() const { return m_uiMeshLevel;};
	bool	GetEnableLOD() const { return m_bEnableLOD;};

	const RenderSystem::sdTerrainParams& GetTerrainParams() const { return m_kRenderParams;};

protected:
	sdHeightMap*	GetHeightMap() { return m_pkHeightMap;};

protected:
	bool	m_bInitialized;			// �Ƿ��ʼ��
	bool	m_bIsVisible;			// �Ƿ�ɼ�(һ�����ڱ༭��)	
	bool	m_bEnableEditHeight;	// �Ƿ�����༭���θ߶�
	bool	m_bEnableEditMaterial;	// �Ƿ�����༭���β���

	// �ر�߶�ͼ
	sdHeightMapPtr	m_pkHeightMap;	

	// �ر���ͼ
	sdNormalMapPtr	m_pkNormalMap;

	// �ر�ͼ�㼯�Ϻ�ͼ������Ϣ����
	typedef std::vector<sdLayerMapPtr> LayerMapVec;
	typedef std::vector<sdLayerMapPtr>::iterator LayerMapVecItr;
	LayerMapVec		m_kLayerMapVec;								// �����ڱ༭��

	typedef std::vector<uchar*> LayerAlphaVec;
	typedef std::vector<uchar*>::iterator LayerAlphaVecItr;
	LayerAlphaVec	m_kLayerAlphaVec;							// �����ڱ༭��

	// �ر�����ͼ����ұ�
	RenderSystem::sdDynamicTexturePtr	m_spDynamicBlendMap;	// �����ڱ༭��
	NiTexturePtr	m_spBlendMap;

	RenderSystem::sdDynamicTexturePtr	m_spDynamicTileMap;		// �����ڱ༭��
	NiTexturePtr	m_spTileMap;	



	// �ر���������ͼ���Ͳ��ұ�
	NiTexturePtr	m_spDiffuseAtlasMap;
	NiTexturePtr	m_spDiffuseAtlasTable;

	// �ر�����ͼ���Ͳ��ұ�
	NiTexturePtr	m_spNormalAtlasMap;
	NiTexturePtr	m_spNormalAtlasTable;

	// �����Ĳ���
	sdQuadNodePtr	m_pkQuadRoot;

	// ���λ��Ʋ���(�����ṩ����Ⱦϵͳʹ��,�ڲ���Ӧ�ñ������ڴ�ͱ�����Դ)
	RenderSystem::sdTerrainParams	m_kRenderParams;

	uint	m_uiTerrainSize;	// Terrain�ĳߴ�(256/512/1024/2048,��λUnit)
	uint	m_uiMeterPerUnit;	// ÿ��λ��С(Ŀǰ��1m)
	uint	m_uiBlendTexSize;	// BlendMap�ߴ�(256/512/1024/2048,��λPixel)
	uint	m_uiTexTileSize;	// BlendMap��TexTile�ߴ�(Ŀǰ��4����)
	uint	m_uiTileSize;		// MeshTile�ĳߴ�
	uint	m_uiMeshLevel;		// �Ĳ�����,ֻ��С��һ���㼶�ĲŻṹ��Mesh
	bool	m_bEnableLOD;		// �Ĳ����Ƿ�����LOD��ʾ
	bool	m_bEnableNormal;	// �����Ƿ���������ͼ

	// ���ݵ�ǰ��׶����ֱ��ʼ��������Error��Distance֮���ת����,���ڿ���LOD���л�
	// (�༶�л�,�ӳ��л�,һ����һ������,�����д��ο�CryEngine)
	//float	m_fError2Distance;	
};
#endif