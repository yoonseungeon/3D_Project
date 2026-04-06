#include "CImage.h"

#include "DirectXTex.h"

CImage::CImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CImage::CImage(const CImage& Prototype)
	: CComponent{ Prototype }
	, m_iNumImages{ Prototype.m_iNumImages }
	, m_vecAlphaImages{ Prototype.m_vecAlphaImages }
{

}

HRESULT CImage::Initialize_Prototype(const _tchar* pImageFilePath, _uint iImages)
{
	_tchar szEXT[MAX_PATH] = {};

	_wsplitpath_s(
		pImageFilePath,	// 문자열
		nullptr, 0,			// 드라이브 경로, 버퍼 크기
		nullptr, 0,			// 디렉토리 경로, 버퍼 크기
		nullptr, 0,			// 파일 이름,	  버퍼 크기
		szEXT, MAX_PATH		// 확장자,        버퍼 크기
	);

	if (lstrcmp(szEXT, TEXT(".png")) != 0)
	{
		return E_FAIL;
	}

	ScratchImage image;

	// m_iNumImages = iImages;
	m_iNumImages = 1;

	HRESULT hr = LoadFromWICFile(pImageFilePath, WIC_FLAGS_NONE, nullptr, image);
	if (FAILED(hr)) {
		return E_FAIL;
	}

	const Image* pImage = image.GetImage(0, 0, 0);
	if (pImage == nullptr) {
		return E_FAIL;
	}

	shared_ptr<IMAGE_ALPHA> tImageAlpha = make_shared<IMAGE_ALPHA>();
	tImageAlpha->iWidth = pImage->width;
	tImageAlpha->iHeight = pImage->height;
	tImageAlpha->vecAlphaImage.reserve(pImage->width * pImage->height);

	for (size_t i = 0; i < pImage->height; ++i)
	{
		// 실제 픽셀 시작 주소 pImage->pixels
		_ubyte* pAlphaPixels = pImage->pixels + i * pImage->rowPitch;
		for (size_t j = 0; j < pImage->width; ++j)
		{
			tImageAlpha->vecAlphaImage.push_back(pAlphaPixels[j * 4 + 3]);
		}
	}

	m_vecAlphaImages.push_back(tImageAlpha);

	return S_OK;
}

HRESULT CImage::Initialize(void* pArg)
{
	return S_OK;
}

CImage* CImage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pImageFilePath, _uint iImages)
{
	CImage* pInstance = new CImage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pImageFilePath, iImages)))
	{
		MSG_BOX("Failed to Created: CImage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CImage::Clone(void* pArg)
{
	CImage* pInstance = new CImage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned: CImage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImage::Free()
{
	__super::Free();
}
