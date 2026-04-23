#include "CCustomFont.h"

CCustomFont::CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCustomFont::Initialize(const _tchar* pFontFilePath)
{
	m_pBatch = new SpriteBatch(m_pContext);
	m_pFont = new SpriteFont(m_pDevice, pFontFilePath);

	// 글자를 픽셀 경계에 맞춰서 그리게 강제하는 옵션
	m_pFont->SetPixelAlignment(true);

	return S_OK;
}

HRESULT CCustomFont::Draw(const _tchar* pText, const _float2& vPosition, _fvector vColor, const _float2& vScale, _float fRotation, const _float2& vOrigin)
{
	//m_pBatch->Begin();
	m_pBatch->Begin(
		DirectX::SpriteSortMode_Deferred,
		nullptr, nullptr, nullptr, nullptr,
		nullptr,
		XMMatrixIdentity()
	);

	m_pFont->DrawString(m_pBatch, pText, vPosition, vColor, fRotation, vOrigin, vScale);

	m_pBatch->End();

	return S_OK;
}

CCustomFont* CCustomFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath)
{
	CCustomFont* pInstance = new CCustomFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pFontFilePath)))
	{
		MSG_BOX("Failed to Created: CCustomFont");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustomFont::Free()
{
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pFont);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	__super::Free();
}
