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
#include "sdLayerMap.h"
#include "sdQuadNode.h"
#include "sdTerrainPick.h"

// ��Ⱦϵͳ
#include "sdRenderSystem.h"
#include "sdDynamicTexture.h"
#include "sdTextureAtlas.h"

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
	bool	SaveScene(const std::string& szSceneFullPath);
	bool	HasScene() { return m_bInitialized;};

	// ���βü�(��������)
	//(wz��װ��һ��TerrainTileEntity,ò��ûɶ��Ҫ,����ֱ�ӷ���Mesh)
	void	Cull(const NiCamera& kCamera, std::vector<NiMesh*>& kMeshVec);

	// ����ʰȡ(��������)
	bool	Pick(const Base::Math::sdRay& kRay, Base::Math::sdVector3& kIntersect, float fLimit = FLT_MAX);	

	// ���θ߶Ȼ�ȡ(��������)
	float	GetHeight(float fX, float fY);

	// ���α༭(���������ڵ��α༭��)
	// @{
	// ���θ߶Ȼ�ȡ������(�߶�ͼ����,ֱ�ӴӸ߶�ͼȡ��������,û�н��б任)
	float	GetRawHeight(uint uiX, uint uiY);
	void	SetRawHeight(uint uiX, uint uiY, float fHeight);

	// ���¼�����(��������)
	void	UpdateGeometry(float fCenterX, float fCenterY, float fRadius);
	// @}


	// ͼ��༭
	// @{
	bool	AppendLayer(sdLayerMap* pkLayerMap);
	//bool	Insert(sdLayerMap* pkLayerMap, uint uiIndex);

	//bool	RemoveLayer();
	//bool	RemoveLayer(uint uiIndex);

	//bool	SwapLayer(uint uiLIndex, uint uiRIndex);


	// ����Layerָ�����ص�Ȩֵ(���ڱ༭)
	void	SetBlendMapData(uint uiX, uint uiY, uint uiLayer, float fWeight);

	// ����ָ����Χ�Ļ����ͼ(���ڱ༭)
	//	@kRect	m_fX,m_fYΪ������ʼλ��, m_fZ,m_fWΪ���γߴ�(��������ϵ)
	void	UpdateBlendMap();
	void	UpdateBlendMap(const Base::Math::sdVector4& kRect);

	// ����ͼ�������´��
	void	RepackMaps();
	// @}


	// �������Է���
	// @{
	bool	IsVisible() const { return m_bIsVisible;}

	bool	GetEnableEditHeight() const { return m_bEnableEditHeight;}
	bool	GetEnableEditMaterial() const { return m_bEnableEditMaterial;}

	uint	GetTileSize() const { return m_uiTileSize;}
	uint	GetTerrainSize() const { return m_uiTerrainSize;}
	uint	GetMeshLevel() const { return m_uiMeshLevel;}
	bool	GetEnableLOD() const { return m_bEnableLOD;}

	float	GetScale() const { return m_fScale;}
	const NiPoint3& GetOrigin() const { return m_kOrigin;}

	const RenderSystem::sdTerrainParams& GetTerrainParams() const { return m_kRenderParams;};
	// @}

protected:
	// ��ȡ�߶�ͼ(�ڲ�ʹ��,�༭��ʹ��)
	sdHeightMap*	GetHeightMap() { return m_pkHeightMap;};

	// @{
	// ��ȡָ�����ص��Ȩ���б�,wzԭʼ�汾��ȡ��ʽ
	uint	GetWeights(uint uiX, uint uiY, uchar* pucWeights);
	// @}

	// ���ݼ����뱣��
	// @{
	
	// @}

protected:
	bool	m_bInitialized;			// �Ƿ��ʼ��
	bool	m_bIsVisible;			// �Ƿ�ɼ�(һ�����ڱ༭��)	
	bool	m_bEnableEditHeight;	// �Ƿ�����༭���θ߶�
	bool	m_bEnableEditMaterial;	// �Ƿ�����༭���β���

	// ��������ƫ��������
	NiPoint3	m_kOrigin;	// ��������ƫ��(Ĭ��Ϊ(0,0,0))
	float		m_fScale;	// ������������(Ĭ��1.0f)

	// �ر�߶�ͼ
	sdHeightMapPtr	m_pkHeightMap;	
	sdTerrainPickPtr m_pkPick;

	// �ر���ͼ
	sdNormalMapPtr	m_pkNormalMap;

	// �ر�ͼ�㼯�Ϻ�ͼ������Ϣ����
	typedef std::vector<sdLayerMapPtr> LayerMapVec;
	typedef std::vector<sdLayerMapPtr>::iterator LayerMapVecItr;
	LayerMapVec		m_kLayerMapVec;								// �����ڱ༭��
	bool			m_bDirtyLayerMapVec;				

	typedef std::vector<uchar*> LayerAlphaVec;
	typedef std::vector<uchar*>::iterator LayerAlphaVecItr;
	LayerAlphaVec	m_kLayerAlphaVec;							// �����ڱ༭��

	// �ر�����ͼ����ұ�
	RenderSystem::sdDynamicTexturePtr	m_pkDynamicBlendMap;	// �����ڱ༭��
	NiTexturePtr	m_spBlendMap;

	RenderSystem::sdDynamicTexturePtr	m_pkDynamicTileMap;		// �����ڱ༭��
	NiTexturePtr	m_spTileMap;	

	// �ر���������ͼ���Ͳ��ұ�
	RenderSystem::sdTextureAtlasPtr m_pkDiffuseAtlas;
	NiTexturePtr	m_spDiffuseAtlasMap;
	NiTexturePtr	m_spDiffuseAtlasTable;

	// �ر�����ͼ���Ͳ��ұ�
	RenderSystem::sdTextureAtlasPtr m_pkNormalAtlas;
	NiTexturePtr	m_spNormalAtlasMap;
	NiTexturePtr	m_spNormalAtlasTable;

	// �����Ĳ���
	sdQuadNodePtr	m_pkQuadRoot;

	// ���λ��Ʋ���(�����ṩ����Ⱦϵͳʹ��,�ڲ���Ӧ�ñ������ڴ�ͱ�����Դ)
	RenderSystem::sdTerrainParams	m_kRenderParams;

	uint	m_uiTerrainSize;	// Terrain�ĳߴ�(256/512/1024/2048,��λGrid)
	float	m_fMeterPerGrid;	// ÿ��λ��С(0.125/0.25/0.5/1/2/4/8,Ŀǰ��1m)
	uint	m_uiTileSize;		// Terrain��Tile�ĳߴ�(Ŀǰ��64Unit)

	uint	m_uiBlendTexSize;	// BlendMap�ߴ�(256/512/1024/2048,��λPixel)
	float	m_fMeterPerPixel;	// ÿ���ش�С(0.125/0.25/0.5/1/2/4/8,Ŀǰ��0.5f)
	uint	m_uiTexTileSize;	// BlendMap��TexTile�ߴ�(Ŀǰ��4Pixel)
	uint	m_uiTileMapSize;	// TileMap�ĳߴ�

	uint	m_uiMeshLevel;		// �Ĳ�����,ֻ��С��һ���㼶�ĲŻṹ��Mesh
	bool	m_bEnableLOD;		// �Ĳ����Ƿ�����LOD��ʾ
	bool	m_bEnableNormal;	// �����Ƿ���������ͼ

	// ���ݵ�ǰ��׶����ֱ��ʼ��������Error��Distance֮���ת����,���ڿ���LOD���л�
	// (�༶�л�,�ӳ��л�,һ����һ������,�����д��ο�CryEngine)
	//float	m_fError2Distance;	

protected:
	// Tile����Ȩ�ػ��ʱ�Ĺ�����(���ڱ༭��,���ʱ༭)
	// @{
	struct stFilterMap
	{
		uint	uiIdx[4];
		float	fWeight[4];
	};
	typedef std::vector<stFilterMap> FilterMapVec;;
	typedef std::vector<stFilterMap>::iterator FilterMapVecItr;
	FilterMapVec m_kTileFilterMapVec;
	// @}

protected:
	static uint	ms_uiMaxLayerCount;
};
#endif