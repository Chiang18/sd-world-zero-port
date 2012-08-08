#include "Stdafx.h"
#include "sdQuadMesh.h"
#include "sdTerrain.h"

uint sdQuadMesh::ms_uiGPUPostVertexCacheSize = 24;
NiDataStreamPtr sdQuadMesh::ms_spIndexStream = NULL;
std::vector<Base::Math::sdVector3us>	sdQuadMesh::ms_kIndexMap;
std::vector<short>	sdQuadMesh::ms_kVertexBuf;
//-------------------------------------------------------------------------------------------------
void sdQuadMesh::StaticInitialize()
{
	sdTerrain* pkTerrain = sdTerrain::InstancePtr();
	NIASSERT(pkTerrain);

	uint uiTileSize = 64;

	// ����
	CreateIndexStream(uiTileSize);
	CreateIndexMap(uiTileSize);

	// ���㻺��
	ms_kVertexBuf.resize(CalcVertexNum(uiTileSize) * 2, 0);
}
//-------------------------------------------------------------------------------------------------
void sdQuadMesh::StaticDestroy()
{
	using namespace Base::Math;

	ms_spIndexStream = 0;

	// �����ڴ�
	std::vector<sdVector3us> kTmp;
	ms_kIndexMap.swap(kTmp);
}
//-------------------------------------------------------------------------------------------------
uint sdQuadMesh::CalcIndexNum(uint uiSize)
{
	//
	//
	// ��������
	//
	//
	//uint uiCacheSize = ms_uiGPUPostVertexCacheSize;	// Cache��С(����)
	//uint uiCacheColume = uiCacheSize / 2;			// Cache����С(����)
	//uint uiTileSize = uiSize;						// Tile�ߴ�(����)(Power(2,n))
	//uint uiTileAllocSize =	uiTileSize + 2 + 1;		// Tile����Skirt��Ķ�����(����)

	//// ����Cache������(����/����)
	//uint uiColumeNum = (uiTileAllocSize - 1) / (uiCacheColume - 1);

	//// ����ÿCache��������
	//uint uiColumeIndexCount = uiCacheColume * (uiTileAllocSize - 1) * 2 + 
	//	(uiTileAllocSize - 1 - 1) * 2;

	//// �������ʣ����������(�������ż���)
	//uint uiLastColumeIndexCount = 0;
	//uint uiLastColume = (uiTileAllocSize - 1) - (uiCacheColume - 1) * uiColumeNum;
	//if (uiLastColume)
	//{
	//	uiLastColumeIndexCount = (uiLastColumeIndexCount + 1) * (uiTileAllocSize - 1) * 2 + 
	//		(uiTileAllocSize - 1 -1) * 2;
	//}

	//// ������֮�������������������
	//uint uiColumeZeroTriIndexCount = uiColumeNum * 2;
	//if (!uiLastColume)
	//{
	//	uiColumeZeroTriIndexCount -= 2;
	//}

	////
	//uint uiIndexCount = uiColumeIndexCount * uiColumeNum + 
	//	uiColumeZeroTriIndexCount + 
	//	uiLastColumeIndexCount;

	//return uiIndexCount;

	return 0;
}
//-------------------------------------------------------------------------------------------------
uint sdQuadMesh::CalcVertexNum(uint uiSize)
{
	uint uiCacheSize = ms_uiGPUPostVertexCacheSize;	// Cache��С(����)
	uint uiCacheColume = uiCacheSize / 2;			// Cache����С(����)
	uint uiTileSize = uiSize;						// Tile�ߴ�(����)(Power(2,n))
	uint uiTileAllocSize =	uiTileSize + 2 + 1;		// Tile����Skirt��Ķ�����(����)

	// ����Cache������(����/����)
	uint uiColumeNum = (uiTileAllocSize - 1) / (uiCacheColume - 1);

	// ����Cache����������(ǡ��Ϊ������ʱ-1,����+1-1����)
	if ((uiTileAllocSize - 1) == (uiCacheColume - 1) * uiColumeNum)
		--uiColumeNum;

	// Cache���ڽӴ������ݱ��ظ�
	uint uiVertexNum = uiTileAllocSize * (uiTileAllocSize + uiColumeNum);

	return uiVertexNum;
}
//-------------------------------------------------------------------------------------------------
void sdQuadMesh::CreateIndexStream(uint uiSize)
{
	using namespace std;

	// ����GPU���㻺���С��Tile��С��������
	//	1.ÿuiCacheColume������Ϊһ��,��֮�乲��һ�ж���
	//	2.���δ���ÿ��,��֮����������������ν�,�ظ��洢һ�ж�������������������
	//	3.ÿ�����д���,��֮����������������ν�
	//
	// ��һ����ʼ��������
	// (����CacheΪ24, uiSizeΪ64)
	//	60	61	62	63	64	65
	//	48	49	50	51	52	53	54	55	56	57	58	59
	//	36	37	38	39	40	41	42	43	44	45	46	47
	//	24	25	26	27	28	29	30	31	32	33	34	35
	//	1	3	5	7	9	11	13	15	17	19	21	23
	//	0	2	4	6	8	10	12	14	16	18	20	22
	//
	// @{
	vector<unsigned short>	kIndiceVec;
	uint uiCacheSize = ms_uiGPUPostVertexCacheSize;	// Cache��С(����)
	uint uiCacheColume = uiCacheSize / 2;			// Cache����С(����)
	uint uiTileSize = uiSize;						// Tile�ߴ�(����)(Power(2,n)), 64
	uint uiTileAllocSize =	uiTileSize + 2 + 1;		// Tile����Skirt��Ķ�����(����), 67
	uint uiVertex = 0;
	for (uint uiVertexXStart = 0; uiVertexXStart < uiTileAllocSize-1; uiVertexXStart += uiCacheColume-1)
	{
		// ÿ����ʼ��ֱ���:
		// 0-11-22-33-34(�ܼ�35��)
		// 0-11-22-33-44-55-66(�ܼ�67��,��ʼ��Ϊ���һ�еĲ�����)
		//
		// ÿ��12�ж���,��֮�乲��һ�ж���,�����ֱ���:
		// [0,11]	[11,22]	[22,33]	[33,34]
		// [0,11]	[11,22]	[22,33]	[33,44]	[44,55]	[55,66]
		//
		// �������X�����λ�úͿ��(������������������Tile����)
		uint uiVertexXEnd = min(uiVertexXStart + uiCacheColume, uiTileAllocSize);	
		uint uiVertexXDelta = uiVertexXEnd - uiVertexXStart;

		// ��ǰ����ʼ����
		uint uiVertexColumeStart = uiVertex;

		// ����0-1�й��ɵ�������
		// 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
		for (uint x = uiVertexXStart; x < uiVertexXEnd; ++x)
		{
			kIndiceVec.push_back(uiVertex++);	// 0��
			kIndiceVec.push_back(uiVertex++);	// 1��
		}

		// �ظ�ѹ���1�����һ��(23),�õ��˻�������(22,23,23)
		// �ظ�ѹ���1�е�һ��(1),�õ��˻�������(23,23,1)
		kIndiceVec.push_back(uiVertex - 1);
		kIndiceVec.push_back(uiVertexColumeStart + 1);

		// ����1-2�й��ɵ�������
		// 1 24 3 25 5 26 7 27 9 28 11 29 13 30 15 31 17 32 19 33 21 34 23 35
		for (uint x = uiVertexXStart; x < uiVertexXEnd; ++x)
		{
			kIndiceVec.push_back(uiVertexColumeStart + (x - uiVertexXStart) * 2 + 1);	// 1��
			kIndiceVec.push_back(uiVertex++);											// 2��
		}

		// ���δ���2-66�й���64��������
		for (uint y = 2; y < uiTileAllocSize - 1; ++y)
		{
			// �ظ�ѹ����һ�����һ����,�õ��˻�������
			// �ظ�ѹ����һ�е�һ����,�õ��˻�������
			kIndiceVec.push_back(uiVertex - 1);
			kIndiceVec.push_back(uiVertexColumeStart + y * uiVertexXDelta);

			// ѹ��һ��������
			for (uint x = uiVertexXStart; x < uiVertexXEnd; ++x)
			{
				kIndiceVec.push_back(uiVertex - uiVertexXDelta);	// ��ʼ��
				kIndiceVec.push_back(uiVertex++);					// ������
			}
		}

		// ������һ���Ľӷ�
		//
		// �ظ�ѹ����һ�����һ����,�õ��˻�������
		// ѹ����һ����һ����,�õ��˻�������
		kIndiceVec.push_back(uiVertex - 1);
		kIndiceVec.push_back(uiVertex);
	}
	// @}

	// ����������
	// @{
	// ȥ�����һ�����������������������
	int iIndexSize = kIndiceVec.size() - 2;

	// 
	NiDataStreamFactory* spDataStreamFactory = NiDataStream::GetFactory();
	NiDataStreamFactory::NiDataStreamFactoryCallback spOldCallback = spDataStreamFactory->GetCallback();
	spDataStreamFactory->SetCallback(NULL);

	// ��������ʼ������������
	ms_spIndexStream = NiDataStream::CreateSingleElementDataStream(
		NiDataStreamElement::F_UINT16_1,
		iIndexSize,
		NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC,
		NiDataStream::USAGE_VERTEX_INDEX,
		&(kIndiceVec[0]));

	//
	spDataStreamFactory->SetCallback(spOldCallback);
	// @}
}
//-------------------------------------------------------------------------------------------------
void sdQuadMesh::CreateIndexMap(uint uiSize)
{
	using namespace Base::Math;

	ms_kIndexMap.clear();

	//
	// IndexMap������Indexһһ��Ӧ�Ķ������������Vertex����,���ڸ���������������ռ�λ��
	//
	// NiPoint��xyֵ:	Tile����
	// NiPoint��zֵ:	0�����ȹ�ӵ�, 1����ȹ�ӵ�
	//
	uint uiCacheSize = ms_uiGPUPostVertexCacheSize;	// Cache��С(����)
	uint uiCacheColume = uiCacheSize / 2;			// Cache����С(����)
	uint uiTileSize = uiSize;						// Tile�ߴ�(����)(Power(2,n)), 64
	uint uiTileAllocSize =	uiTileSize + 2 + 1;		// Tile����Skirt��Ķ�����(����), 67
	for (uint uiVertexXStart = 0; uiVertexXStart < uiTileAllocSize-1; uiVertexXStart += uiCacheColume-1)
	{
		// �������X�����λ��(������������������Tile����)
		uint uiVertexXEnd = min(uiVertexXStart + uiCacheColume, uiTileAllocSize);	

		// ��ʼ��-1,0
		for (uint x = uiVertexXStart; x < uiVertexXEnd; ++x)
		{
			uint uiX = min((uint)max((int)x-1, 0), uiTileSize);	// ȹ�ӵ�ǯλ,[0,66]��[0,64]
			uint uiY = 0;										// ��(0)
			ms_kIndexMap.push_back(sdVector3us(uiX, uiY, 1));	// ȹ�����
			ms_kIndexMap.push_back(sdVector3us(uiX, uiY, 0));	// 
		}

		// �м���[1-64]��
		for (uint y = 1; y <= uiTileSize; ++y)
		{
			for (uint x = uiVertexXStart; x < uiVertexXEnd; ++x)
			{
				uint uiX = min((uint)max((int)x-1, 0), uiTileSize);	// ȹ�ӵ�ǯλ,[0,66]��[0,64]
				uint uiY = y;										// ��
				uint uiZ = ((x == 0 || x == uiTileSize+1) ? 1: 0);	// ��Ե��

				ms_kIndexMap.push_back(sdVector3us(uiX, uiY, uiZ));
			}
		}

		// ������65��
		for (uint x = uiVertexXStart; x < uiVertexXEnd; ++x)
		{
			uint uiX = min((uint)max((int)x-1, 0), uiTileSize);		// ȹ�ӵ�ǯλ,[0,66]��[0,64]
			uint uiY = 64;											// ��(64)
			ms_kIndexMap.push_back(sdVector3us(uiX, uiY, 1));		// ȹ�����
		}
	}
}
//-------------------------------------------------------------------------------------------------
sdQuadMesh::sdQuadMesh(uint uiX, uint uiY, uint uiSpacing, uint uiSize, float fSkirtDepth)
: m_uiX(uiX)
, m_uiY(uiY)
, m_uiSpacing(uiSpacing)
{
	NiRenderer* spRenderer = NiRenderer::GetRenderer();
	NIASSERT(spRenderer);

	// ��������������
	CreateVertexStream(uiX, uiY, uiSpacing, uiSize, fSkirtDepth);
	
	// ����Mesh
	// @{
	SetName("TerrainQuadMesh");
	SetPrimitiveType(NiPrimitiveType::PRIMITIVE_TRISTRIPS);
	
	// ����������������������
	AddStreamRef(ms_spIndexStream, NiCommonSemantics::INDEX(), 0);
	AddStreamRef(m_spVertexStream, NiCommonSemantics::POSITION(), 0);

	// ����


	// ��������
	// a_vTerrainQuadParam
	// [origin_x][origin_y][scale][morph_factor]
	float vExtraData[4] = { 0 };
	vExtraData[0] = (float)m_uiX;
	vExtraData[1] = (float)m_uiY;
	vExtraData[2] = (float)m_uiSpacing;
	vExtraData[3] = 1.0f;

	m_spExtraData = NiNew NiFloatsExtraData(4, vExtraData);
	AddExtraData("a_vTerrainQuadParam", m_spExtraData);

	// ��Χ��


	//
	UpdateProperties();
	UpdateEffects();
	Update(0.0f);

	// Ԥȡ
	spRenderer->PrecacheMesh(this);
	// @}
}
//-------------------------------------------------------------------------------------------------
sdQuadMesh::~sdQuadMesh()
{
	m_spVertexStream = 0;
}
//-------------------------------------------------------------------------------------------------
void sdQuadMesh::CreateVertexStream(uint uiX, uint uiY, uint uiSpacing, uint uiSize, float fSkirtDepth, bool bAllowEdit)
{
	using namespace Base::Math;

	sdTerrain* pkTerrain = sdTerrain::InstancePtr();
	NIASSERT(pkTerrain);

	sdHeightMap* pkHeightMap = pkTerrain->GetHeightMap();
	NIASSERT(pkHeightMap);

	// ��ʼ����������
	// @{
	uint uiVertexNum = CalcVertexNum(uiSize);
	ZeroMemory(&(ms_kVertexBuf[0]), uiVertexNum * 2);

	short* psVertexData = &(ms_kVertexBuf[0]);
	for (uint uiVertex = 0; uiVertex < uiVertexNum; ++uiVertex)
	{
		// ���Ҷ�Ӧ����λ�ö�Ӧ��Tile������
		const sdVector3us& kMapping = ms_kIndexMap[uiVertex];
		
		// ���Ҹ߶�(������ץȡ�Ķ���߶�,û�о�����ֵ)
		float fHeight = pkHeightMap->GetRawHeight(uiX + kMapping.m_kX, uiY + kMapping.m_kY);

		// ��ȥȹ�Ӹ߶�
		fHeight -= kMapping.m_kZ * fSkirtDepth;

		// ѹ�����㵽������,�Լ���DMA����
		//	1.������Ϊʲô�ǳ���129,������128(�����Ǵ�������,���Բ���256)
		//  2.�߶�ֵ����20��������,�Ӷ���С�ֱ�����0.05
		uchar ucX = (uchar)kMapping.m_kX;
		uchar ucY = (uchar)kMapping.m_kY;
		*psVertexData = short(ucX * 129 + ucY);
		++psVertexData;

		*psVertexData = (short)floor(fHeight * 20.0f + 0.5f);
		++psVertexData;
	}
	// @}

	// ��������������
	//
	// ����F_INT16_2ֻ����������,Debug�汾�ᱻ������,��ʱľ�д���.
	// ���Կ��Ǹ�ΪF_INT8_4,�Ӷ���ָ߶�ֵ�����
	//
	// @{
	NiDataStreamFactory* spDataStreamFactory = NiDataStream::GetFactory();
	NiDataStreamFactory::NiDataStreamFactoryCallback spOldCallback = spDataStreamFactory->GetCallback();
	spDataStreamFactory->SetCallback(NULL); 

	// �������Ȩ��
	NiUInt8 uiAccessMask = NiDataStream::ACCESS_GPU_READ;
	if (bAllowEdit)
		uiAccessMask |= NiDataStream::ACCESS_CPU_WRITE_MUTABLE;
	else
		uiAccessMask |= NiDataStream::ACCESS_CPU_WRITE_STATIC;

	//
	m_spVertexStream = NiDataStream::CreateSingleElementDataStream(
		NiDataStreamElement::F_INT16_2,
		uiVertexNum,
		uiAccessMask,
		NiDataStream::USAGE_VERTEX,
		&(ms_kVertexBuf[0]));

	spDataStreamFactory->SetCallback(spOldCallback);
	// @}
}
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------