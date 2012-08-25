#include "Stdafx.h"
#include "sdTerrain.h"

using namespace Base::Math;
using namespace RenderSystem;

uint sdTerrain::ms_uiMaxLayerCount = 48;
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

	// 编辑器状态,创建混合掩码
	if (m_bEnableEditMaterial)
	{
		uchar* puiLayerAlpha = new uchar[m_uiBlendTexSize * m_uiBlendTexSize];
		NIASSERT(puiLayerAlpha);

		m_kLayerAlphaVec.push_back(puiLayerAlpha);
	}

	// 添加图层的纹理到图集工具
	m_pkDiffuseAtlas->AddTexture(pkLayerMap->GetDiffuseMap(), pkLayerMap->GetRepeatSize());
	m_pkNormalAtlas->AddTexture(pkLayerMap->GetNormalMap(), pkLayerMap->GetRepeatSize());

	// 设置脏标记
	m_bDirtyLayerMapVec = true;

	return true;
}
//-------------------------------------------------------------------------------------------------
void sdTerrain::SetBlendMapData(uint uiX, uint uiY, uint uiLayer, float fWeight)
{
	// 检查图层索引
	if (uiLayer >= m_kLayerAlphaVec.size())
		return;

	// 检查权值
	if (fWeight < 0 || fWeight > 1.0f)
		return;

	uchar* pucWeightData = m_kLayerAlphaVec[uiLayer];
	NIASSERT(pucWeightData);

	*(pucWeightData + uiY * m_uiBlendTexSize + uiX) = (uchar)(fWeight * 255.0f); 
}
//-------------------------------------------------------------------------------------------------
void sdTerrain::UpdateBlendMap()
{
	sdVector4 kRect;
	kRect.m_fX = m_kOrigin.x;
	kRect.m_fY = m_kOrigin.y;
	kRect.m_fZ = m_uiTerrainSize * m_fMeterPerGrid * m_fScale;
	kRect.m_fW = m_uiTerrainSize * m_fMeterPerGrid * m_fScale;

	UpdateBlendMap(kRect);
}
//-------------------------------------------------------------------------------------------------
void sdTerrain::UpdateBlendMap(const Base::Math::sdVector4& kRect)
{
	// 非编辑状态直接返回
	if (!m_bEnableEditMaterial)
		return;

	// 脏标记没有被设置则直接返回
	if (!m_bDirtyLayerMapVec)
		return;
	m_bDirtyLayerMapVec = false;

	NIASSERT(m_pkDynamicBlendMap);
	NIASSERT(m_pkDynamicTileMap);


	// 转换坐标系
	// @{
	// 转换到BlendMap的像素坐标系(左闭右开)
	float fTotalScale = m_fMeterPerPixel * m_fScale;

	float fStartX	= (kRect.m_fX - m_kOrigin.x) / fTotalScale;
	float fStartY	= (kRect.m_fY - m_kOrigin.y) / fTotalScale;
	float fWidth	= kRect.m_fZ / fTotalScale;
	float fHeight	= kRect.m_fW / fTotalScale;

	if (fStartX >= (float)m_uiBlendTexSize || fStartY >= m_uiBlendTexSize || fStartX + fWidth < 0 || fStartY + fHeight < 0)
		return;
 
	uint uiStartX	= (uint)min(floor(fStartX), 0.0f);
	uint uiStartY	= (uint)min(floor(fStartY), 0.0f);
	uint uiEndX		= (uint)max(floor(fStartX + fWidth), m_uiBlendTexSize);
	uint uiEndY		= (uint)max(floor(fStartY + fHeight), m_uiBlendTexSize);

	// 转换到TexTile像素坐标系(左闭右开)
	uint uiStartTileX	= uiStartX / m_uiTexTileSize;	// 向下取整
	uint uiStartTileY	= uiStartY / m_uiTexTileSize;
	uint uiEndTileX		= (uiEndX + m_uiTexTileSize - 1) / m_uiTexTileSize;	// 向上取整
	uint uiEndTileY		= (uiEndY + m_uiTexTileSize - 1) / m_uiTexTileSize;
	// @}


	// 更新混合贴图
	// @{
	// 锁定
	sdDynamicTexture::sdLockedRect kTileRect = m_pkDynamicTileMap->LockRegion(uiStartTileX, uiStartTileY, uiEndTileX - uiStartTileX, uiEndTileY - uiStartTileY);
	NIASSERT(kTileRect.pBits);

	sdDynamicTexture::sdLockedRect kBlendRect = m_pkDynamicBlendMap->LockRegion(uiStartX, uiStartY, uiEndX - uiStartX, uiEndY - uiStartY);
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

			// 计算当前TexTile对应的BlendMap位置
			uint uiStartBlendX = uiTileX * m_uiTexTileSize;
			uint uiStartBlendY = uiTileY * m_uiTexTileSize;

			// 计算当前TexTile对应的BlendMap的各个Pixel的各Layer权重信息
			// (在非边缘部分,Tile对应的BlendMap向外扩展一个单位)
			int iStartX = (0 == uiTileX) ? 0 : -1;
			int iStartY = (0 == uiTileY) ? 0 : -1;
			int iEndX = (m_uiTileMapSize - 1 == uiTileX) ? m_uiTexTileSize : m_uiTexTileSize + 1;
			int iEndY = (m_uiTileMapSize - 1 == uiTileY) ? m_uiTexTileSize : m_uiTexTileSize + 1;

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
			uchar* pucDstTileData = (uchar*)kTileRect.pBits + uiY * kTileRect.uiPitch + uiX * 4;
			for (uint uiLayer = 0; uiLayer < kVisLayers.size(); ++uiLayer)
			{
				pucDstTileData[uiLayer] = kVisLayers[uiLayer];
			}

			// 填充当前Tile的权重数据到BlendMap
			uchar* pucDstPixelWeightsLine = (uchar*)kBlendRect.pBits + uiY * m_uiTexTileSize * kBlendRect.uiPitch + uiX * m_uiTexTileSize * 2;
			for (uint uiBlendY = 0; uiBlendY < m_uiTexTileSize; ++uiBlendY)
			{
				uchar* pucDstPixelWeights = pucDstPixelWeightsLine;
				for (uint uiBlendX = 0; uiBlendX < m_uiTexTileSize; ++uiBlendX)
				{
					// 遍历可见图层
					uchar ucTempWeights[4] = { 0, 0, 0, 0};
					stFilterMap& kFilterMap = m_kTileFilterMapVec[uiBlendY * m_uiTexTileSize + uiBlendX];
					for (uint uiLayer = 0; uiLayer < kVisLayers.size(); ++uiLayer)
					{
						float fWeight = 0.0f;
						uint uiVisLayer = kVisLayers[uiLayer];
						fWeight += kWeights[kFilterMap.uiIdx[0] + uiVisLayer] * kFilterMap.fWeight[0];
						fWeight += kWeights[kFilterMap.uiIdx[1] + uiVisLayer] * kFilterMap.fWeight[1];
						fWeight += kWeights[kFilterMap.uiIdx[2] + uiVisLayer] * kFilterMap.fWeight[2];
						fWeight += kWeights[kFilterMap.uiIdx[3] + uiVisLayer] * kFilterMap.fWeight[3];

						// 钳位到[0.0, 255.0]
						fWeight = NiClamp(fWeight, 0.0f, 255.0f);

						// 钳位到[0, 16]
						ucTempWeights[uiLayer] = (uchar)(fWeight / 255.0f * 15.0f);
					}

					// 压缩到目标
					pucDstPixelWeights[0] = (ucTempWeights[1] << 4) | ucTempWeights[0];
					pucDstPixelWeights[1] = (ucTempWeights[3] << 4) | ucTempWeights[2];

					//
					pucDstPixelWeights += 2;
				}

				pucDstPixelWeightsLine += m_uiTexTileSize * kBlendRect.uiPitch;
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