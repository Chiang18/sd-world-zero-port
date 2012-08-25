//*************************************************************************************************
// ����:	������,��Ҫ�Ǹ�������D3D����
//---------------------------------------------------------
// ����:		
// ����:		2012-08-11
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_TEXTURE_UTILITY_H__
#define _SD_ENGINE_TEXTURE_UTILITY_H__
#include "sdTypes.h"

namespace RenderSystem
{
	// ������,������D3D
	class sdTextureUtility : public NiMemObject
	{
	public:
		// �������������Ŀ�껺��
		struct stBuffer
		{
			uint	uiSize;
			void*	pBits;
		};

	public:
		// ����ѹ��
		//	@kDstBuffer			: ���Ŀ�껺��
		//	@pSrcData			: ��������
		//	@uiWidth,uiHeight	: �����������ݳߴ�
		//	@uiBytesPerPixel	: ��������ÿ����ͨ����/�ֽ���(8:1 16:2 ...)
		//	@eFormat			: Ŀ��ѹ����ʽ,Ŀǰֻ֧��DXT1~DXT5
		//static bool Compress(stBuffer& kDstBuffer,void* pSrcData, uint uiWidth, uint uiHeight, uint uiBytesPerPixel, D3DFORMAT eFormat);
		
		// �����ѹ��
		//	@kDstBuffer			: ���Ŀ�껺��
		//	@kSrcBuffer			: ���뻺��
		//	@uiBytesPerPixel	: ��������ÿ����ͨ����
		//static bool Uncompress(stBuffer& kDstBuffer, stBuffer& kSrcBuffer, uint uiBytesPerPixel);


		// ��������ʽ
		static bool	IsDDSFormat(D3DFORMAT eFormat);
		static bool IsZStencilFormat(D3DFORMAT eFormat);
	};
}
#endif