#include "Stdafx.h"
#include "sdTerrain.h"

using namespace Base::Math;
using namespace RenderSystem;

uint sdTerrain::ms_uiMaxLayerCount = 48;
//-------------------------------------------------------------------------------------------------
sdLayerMap* sdTerrain::GetLayerMap(uint uiIndex)
{
	if (uiIndex >= m_kLayerMapVec.size())
		return NULL;

	return m_kLayerMapVec[uiIndex];
}
//-------------------------------------------------------------------------------------------------
sdLayerMap* sdTerrain::GetLayerMap(const char* szName)
{
	LayerMapVecItr itr_layer = m_kLayerMapVec.begin();
	LayerMapVecItr itr_layer_end = m_kLayerMapVec.end();
	for (; itr_layer != itr_layer_end; ++itr_layer)
	{
		if ((*itr_layer)->GetName() == szName)
			return *itr_layer;
	}

	return NULL;
}
//-------------------------------------------------------------------------------------------------
bool sdTerrain::AppendLayer(sdLayerMap* pkLayerMap)
{
	NIASSERT(pkLayerMap);

	// 检查图层数量
	if (m_kLayerMapVec.size() >= ms_uiMaxLayerCount)
		return false;

	// 检查是否与已有图层重名
	LayerMapVecItr itr_layer = m_kLayerMapVec.begin();
	LayerMapVecItr itr_layer_end = m_kLayerMapVec.end();
	for (; itr_layer != itr_layer_end; ++itr_layer)
	{
		if ((*itr_layer)->GetName() == pkLayerMap->GetName())
			return false;
	}

	// 添加到图层列表最后一层
	m_kLayerMapVec.push_back(pkLayerMap);

	// 编辑器状态
	if (m_bEnableEditMaterial)
	{
		// 创建混合掩码
		uchar* puiLayerAlpha = new uchar[m_uiBlendTexSize * m_uiBlendTexSize];
		NIASSERT(puiLayerAlpha);
		ZeroMemory(puiLayerAlpha, m_uiBlendTexSize * m_uiBlendTexSize);

		m_kLayerAlphaVec.push_back(puiLayerAlpha);

		// 添加图层的纹理到图集工具
		m_pkDiffuseAtlas->AddTexture(pkLayerMap->GetDiffuseMap(), pkLayerMap->GetRepeatSize(), pkLayerMap->GetMipmapBias());
		m_pkNormalAtlas->AddTexture(pkLayerMap->GetNormalMap(), pkLayerMap->GetRepeatSize(), pkLayerMap->GetMipmapBias());

	}

	// 设置脏标记
	m_bDirtyLayerMapVec = true;

	return true;
}
//-------------------------------------------------------------------------------------------------
uchar sdTerrain::GetBlendMapData(uint uiX, uint uiY, uint uiLayer)
{
	// 检查图层索引
	if (uiLayer >= m_kLayerAlphaVec.size())
		return 0;

	// 访问对应图层混合数据
	uchar* pucWeightData = m_kLayerAlphaVec[uiLayer];
	NIASSERT(pucWeightData);

	return *(pucWeightData + uiY * m_uiBlendTexSize + uiX); 
}
//-------------------------------------------------------------------------------------------------
void sdTerrain::SetBlendMapData(uint uiX, uint uiY, uint uiLayer, uchar uiWeight)
{
	// 检查图层索引
	if (uiLayer >= m_kLayerAlphaVec.size())
		return;

	// 修改对应图层混合数据
	uchar* pucWeightData = m_kLayerAlphaVec[uiLayer];
	NIASSERT(pucWeightData);

	*(pucWeightData + uiY * m_uiBlendTexSize + uiX) = uiWeight; 
}
//-------------------------------------------------------------------------------------------------
void sdTerrain::UpdateBlendMap(float fCenterX, float fCenterY, float fRadius)
{
	// 非编辑状态直接返回
	if (!m_bEnableEditMaterial)
		return;

	NIASSERT(m_pkDynamicBlendMap);
	NIASSERT(m_pkDynamicTileMap);


	// 转换坐标系
	// @{
	// 计算范围(世界坐标)
	float fStartX	= fCenterX - fRadius;
	float fStartY	= fCenterY - fRadius;
	float fEndX	= fCenterX + fRadius;
	float fEndY = fCenterY + fRadius;

	// 转换到BlendMap的像素坐标系(左闭右开)
	float fTotalScale = m_fScale * m_fMeterPerPixel;
	float fBStartX	= (fStartX - m_kOrigin.x) / fTotalScale;
	float fBStartY	= (fStartY - m_kOrigin.y) / fTotalScale;
	float fBEndX	= (fEndX - m_kOrigin.x) / fTotalScale;
	float fBEndY	= (fEndY - m_kOrigin.y) / fTotalScale;

	if (fBStartX > (float)m_uiBlendTexSize)	return;
	if (fBStartY > (float)m_uiBlendTexSize)	return;
	if (fBEndX < 0.0f)	return;
	if (fBEndY < 0.0f)	return;
 
	uint uiBStartX	= (uint)max(floor(fBStartX), 0.0f);
	uint uiBStartY	= (uint)max(floor(fBStartY), 0.0f);
	uint uiBEndX	= (uint)min(floor(fBEndX), m_uiBlendTexSize);
	uint uiBEndY	= (uint)min(floor(fBEndY), m_uiBlendTexSize);

	// 转换到TexTile像素坐标系(左闭右开)
	uint uiStartTileX	= uiBStartX / m_uiTexTileSize;	// 向下取整
	uint uiStartTileY	= uiBStartY / m_uiTexTileSize;
	uint uiEndTileX		= (uiBEndX + m_uiTexTileSize - 1) / m_uiTexTileSize;	// 向上取整
	uint uiEndTileY		= (uiBEndY + m_uiTexTileSize - 1) / m_uiTexTileSize;
	// @}


	// 更新混合贴图
	// @{
	// 锁定
	sdDynamicTexture::sdLockedRect kTileRect = m_pkDynamicTileMap->LockRegion(uiStartTileX, uiStartTileY, uiEndTileX - uiStartTileX, uiEndTileY - uiStartTileY);
	NIASSERT(kTileRect.pBits);

	sdDynamicTexture::sdLockedRect kBlendRect = m_pkDynamicBlendMap->LockRegion(uiBStartX, uiBStartY, uiBEndX - uiBStartX, uiBEndY - uiBStartY);
	NIASSERT(kBlendRect.pBits);

	// 清空区域
	uchar* pucTileData = (uchar*)kTileRect.pBits;
	for (uint i = uiStartTileY; i < uiEndTileY; ++i)
	{
		ZeroMemory(pucTileData, kTileRect.uiPitch);
		pucTileData += kTileRect.uiPitch;
	}

	uchar* pucBlendData = (uchar*)kBlendRect.pBits;
	for (uint i = uiStartTileY; i < uiEndTileY; ++i)
	{
		ZeroMemory(pucBlendData, kBlendRect.uiPitch);
		pucBlendData += kBlendRect.uiPitch;
	}

	if (m_kLayerMapVec.empty())
	{
		m_pkDynamicTileMap->UnlockRegion();
		m_pkDynamicBlendMap->UnlockRegion();
		return;
	}

	// 遍历受影响的Tile像素
	uint uiNumLayers = m_kLayerMapVec.size();
	uint uiTexTileSizeEx = m_uiTexTileSize + 2;

	std::vector<int> kVisLayers;
	kVisLayers.reserve(uiNumLayers);

	std::vector<uchar>	kWeights(uiTexTileSizeEx * uiTexTileSizeEx * uiNumLayers, 0);
	std::vector<uint>	kTotalWeights(uiNumLayers, 0);

	for (uint uiTileY = uiStartTileY, uiY = 0; uiTileY < uiEndTileY; ++uiTileY, ++uiY)
	{
		for (uint uiTileX = uiStartTileX, uiX = 0; uiTileX < uiEndTileX; ++ uiTileX, ++uiX)
		{
			//
			ZeroMemory(&kWeights[0], sizeof(uchar) * kWeights.size());
			ZeroMemory(&kTotalWeights[0], sizeof(uint) * kTotalWeights.size());
			kVisLayers.clear();

			// 计算当前TexTile对应的BlendMap的各个Pixel的各Layer权重信息
			// 
			// @{
			// 计算当前TexTile对应的BlendMap位置
			uint uiStartBlendX = uiTileX * m_uiTexTileSize;
			uint uiStartBlendY = uiTileY * m_uiTexTileSize;

			// 计算当前TexTile内部相对起始位置(在非边缘部分,Tile对应的BlendMap向外扩展一个单位)
			int iStartX = (0 == uiTileX) ? 0 : -1;
			int iStartY = (0 == uiTileY) ? 0 : -1;
			int iEndX = (m_uiTileMapSize - 1 == uiTileX) ? m_uiTexTileSize : m_uiTexTileSize + 1;
			int iEndY = (m_uiTileMapSize - 1 == uiTileY) ? m_uiTexTileSize : m_uiTexTileSize + 1;

			// 计算当前TexTile向外扩展之后的整个区域的所有权重信息
			uchar* pucPixelWeightsLine = &kWeights[0];
			for (int iY = iStartY; iY < iEndY; ++iY)
			{
				uchar* pucPixelWeights = pucPixelWeightsLine;
				for (int iX = iStartX; iX < iEndX; ++iX)
				{
					// 提取当前BlendMapTex的权重信息
					uint uiNumVisLayers = GetWeights((uint)(uiStartBlendX + iX), (uint)(uiStartBlendY + iY), pucPixelWeights);
				
					// 累积TexTile内BlendMapTex的权重信息
					if (iX != -1 && iX != m_uiTexTileSize  && iY != -1 && iY != m_uiTexTileSize)
					{
						for (uint uiLayer = uiNumLayers - uiNumVisLayers; uiLayer < uiNumLayers; ++uiLayer)
							kTotalWeights[uiLayer] += pucPixelWeights[uiLayer];
					}

					pucPixelWeights += uiNumLayers;
				}

				pucPixelWeightsLine += uiTexTileSizeEx * uiNumLayers;
			}
			// @}

			// 扫描累积权重数组,按从大到小的顺序插入图层
			uint uiNumVisLayers = 0;
			for (uint uiLayer = 0; uiLayer < uiNumLayers; ++uiLayer)
			{
				if (0 == kTotalWeights[uiLayer])
					continue;

				++uiNumVisLayers;
				
				uint uiNum = kVisLayers.size();
				for (uint j = 0; j < 3; ++j)
				{
					if (j >= uiNum || kTotalWeights[uiLayer] > kTotalWeights[kVisLayers[j]])
					{
						kVisLayers.insert(kVisLayers.begin() + j, uiLayer);

						if (kVisLayers.size() > 3)
							kVisLayers.resize(3);

						break;
					}
				}
			}

			// 填充当前Tile的索引数据到TileMap
			// (4是每Pixel有4Byte)
			uchar* pucDstTileData = (uchar*)kTileRect.pBits + uiY * kTileRect.uiPitch + uiX * 4;
			for (uint uiLayer = 0; uiLayer < kVisLayers.size(); ++uiLayer)
			{
				pucDstTileData[uiLayer] = kVisLayers[uiLayer];
			}

			// 填充当前Tile的权重数据到BlendMap
			// (2是每Pixel有2Byte)
			uchar* pucDstPixelWeightsLine = (uchar*)kBlendRect.pBits + uiY * m_uiTexTileSize * kBlendRect.uiPitch + uiX * m_uiTexTileSize * 2;
			for (uint uiBlendY = 0; uiBlendY < m_uiTexTileSize; ++uiBlendY)
			{	
				uchar* pucDstPixelWeightsData = pucDstPixelWeightsLine;
				for (uint uiBlendX = 0; uiBlendX < m_uiTexTileSize; ++uiBlendX)
				{
					// 遍历可见图层,取出各层权重信息,适当进行混合并保存
					uchar ucTempWeights[4] = { 0, 0, 0, 0};
					stFilterMap& kFilterMap = m_kTileFilterMapVec[uiBlendY * m_uiTexTileSize + uiBlendX];
					for (uint uiLayer = 0; uiLayer < kVisLayers.size(); ++uiLayer)
					{
						float fWeight = 0.0f;
						uint uiVisLayer = kVisLayers[uiLayer];

						fWeight = kWeights[(uiBlendY * uiTexTileSizeEx + uiBlendX) * uiNumLayers + uiVisLayer];
						//fWeight += kWeights[kFilterMap.uiIdx[0] + uiVisLayer] * kFilterMap.fWeight[0];
						//fWeight += kWeights[kFilterMap.uiIdx[1] + uiVisLayer] * kFilterMap.fWeight[1];
						//fWeight += kWeights[kFilterMap.uiIdx[2] + uiVisLayer] * kFilterMap.fWeight[2];
						//fWeight += kWeights[kFilterMap.uiIdx[3] + uiVisLayer] * kFilterMap.fWeight[3];

						// 钳位到[0.0, 255.0]
						fWeight = NiClamp(fWeight, 0.0f, 255.0f);

						// 缩放到[0, 16]
						ucTempWeights[uiLayer] = (uchar)(fWeight / 255.0f * 15.0f);
					}

					// 压缩数据并填充到到目标
					pucDstPixelWeightsData[0] = (ucTempWeights[1] << 4) | ucTempWeights[0];
					pucDstPixelWeightsData[1] = (ucTempWeights[3] << 4) | ucTempWeights[2];

					pucDstPixelWeightsData += 2;
				}

				pucDstPixelWeightsLine += kBlendRect.uiPitch;
			}
		}
	}

	// 解锁
	m_pkDynamicTileMap->UnlockRegion();
	m_pkDynamicBlendMap->UnlockRegion();
	// @}	
}
//-------------------------------------------------------------------------------------------------
void sdTerrain::RepackMaps()
{
	// 设置AtlasTexture的输入图层信息
	LayerMapVecItr itr_layer = m_kLayerMapVec.begin();
	LayerMapVecItr itr_layer_end = m_kLayerMapVec.end();
	for (; itr_layer != itr_layer_end; ++itr_layer)
	{
		sdLayerMap* pkLayerMap = *itr_layer;
		NIASSERT(pkLayerMap);

		// 
	}

	// 更新AtlasTexture
	if (m_pkDiffuseAtlas)	m_pkDiffuseAtlas->Update();
	if (m_pkNormalAtlas)	m_pkNormalAtlas->Update();

	// 更新TerrainMaterial
	if (m_pkDiffuseAtlas)
	{
		//m_kRenderParams.diffuseAtlasMap = m_pkDiffuseAtlas->GetA
	}


}
//-------------------------------------------------------------------------------------------------
uint sdTerrain::GetWeights(uint uiX, uint uiY, uchar* pucWeights)
{
	NIASSERT(pucWeights);

	// 从高ID到低ID提取混合值
	float fAlphaScale = 1.0f;
	int iIndex = m_kLayerAlphaVec.size() - 1;
	for (; iIndex > 0 && fAlphaScale > 0.01f; --iIndex)
	{
		uchar* pucAlpha = m_kLayerAlphaVec[iIndex];
		NIASSERT(pucAlpha);

		uchar ucLayerAlpha = *(pucAlpha + uiY * m_uiBlendTexSize + uiX);
		float fLayerAlpha = ucLayerAlpha / 255.0f;

		// 缩放,四舍五入
		pucWeights[iIndex] = (uchar)(fLayerAlpha * fAlphaScale * 255.0f + 0.5f);

		// 修订AlphaScale
		fAlphaScale *= (1.0f - fLayerAlpha);
	}

	if (fAlphaScale > 0.01f)
	{
		// 分配剩余Alpha值
		pucWeights[0] = (uchar)(fAlphaScale * 255.0f + 0.5f);
		return m_kLayerAlphaVec.size();
	}
	else
	{
		return m_kLayerAlphaVec.size() - (iIndex + 1);
	}
}
//-------------------------------------------------------------------------------------------------