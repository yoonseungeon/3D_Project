#include "CTexture.h"
#include "CShader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CTexture::CTexture(const CTexture& Prototype)
	: CComponent{ Prototype }
	, m_iNumTextures{ Prototype.m_iNumTextures }
	, m_Textures{ Prototype.m_Textures }
{
	// 사본 만들 때 얕은 복사로 텍스처 공유
	for (auto& pTexture : m_Textures)
		Safe_AddRef(pTexture);
}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{
	m_iNumTextures = iNumTextures;

	_tchar szTextureFilePath[MAX_PATH] = TEXT("");

	for (size_t i = 0; i < iNumTextures; ++i)
	{
		ID3D11ShaderResourceView* pSRV = { nullptr };
		wsprintf(szTextureFilePath, pTextureFilePath, i);

		_tchar szEXT[MAX_PATH] = {};

		_wsplitpath_s(
			szTextureFilePath,	// 문자열
			nullptr, 0,			// 드라이브 경로, 버퍼 크기
			nullptr, 0,			// 디렉토리 경로, 버퍼 크기
			nullptr, 0,			// 파일 이름,	  버퍼 크기
			szEXT, MAX_PATH		// 확장자,        버퍼 크기
		);

		HRESULT hr = {};

		if (lstrcmp(szEXT, TEXT(".dds")) == 0)
		{
			// 1. 디바이스 2. 경로 3. 리턴 값 ID3D11Resource(ID3D11Texture2D의 부모) 4. 리턴 값 SRV
			// 내부적으로 D3D11Resource 만들고, SRV 필요하다면 만들어 줌.
			hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
		}
		else if (lstrcmp(szEXT, TEXT(".tga")) == 0)
		{
			hr = E_FAIL;
		}
		else
		{
			// WIC: Windows Imaging Component (jpeg, png, bmp 등이 WIC가 지원하는 파일 형식.)
			hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
		}

		if (FAILED(hr))
			return E_FAIL;

		m_Textures.push_back(pSRV);
	}

	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iTextureIndex)
{
	if (iTextureIndex >= m_iNumTextures)
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_Textures[iTextureIndex]);
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Created: CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned: CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	for (auto& pTexture : m_Textures)
		Safe_Release(pTexture);
	m_Textures.clear();

	__super::Free();
}
