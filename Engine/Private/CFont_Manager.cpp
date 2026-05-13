#include "CFont_Manager.h"

#include "CCustomFont.h"
#include "CGameInstance.h"

CFont_Manager::CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CFont_Manager::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
    CCustomFont* pFont = CCustomFont::Create(m_pDevice, m_pContext, pFontFilePath);
    if (pFont == nullptr)
    {
        MSG_BOX("Add_Font Error");
        return E_FAIL;
    }

    auto iter = m_Fonts.emplace(strFontTag, pFont);

    if (iter.second == false) {
        MSG_BOX("Add_Font Error");
        Safe_Release(pFont);
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CFont_Manager::Draw(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, const _float2& vScale, _float fRotation, const _float2& vOrigin)
{
    CCustomFont* pFont = Find_Font(strFontTag);
    if (pFont == nullptr)
    {
        MSG_BOX("Not Found Font");
        return E_FAIL;
    }

    return pFont->Draw(pText, vPosition, vColor, vScale, fRotation, vOrigin);
}

_float2 CFont_Manager::Cal_Length(const _wstring& strFontTag, const _wstring& Text)
{
    CCustomFont* pFont = Find_Font(strFontTag);
    if (pFont == nullptr)
    {
        MSG_BOX("Not Found Font");
        return _float2{ 0.f, 0.f };
    }    

    return pFont->Cal_TextLength(Text);
}

CCustomFont* CFont_Manager::Find_Font(const _wstring& strFontTag)
{
    auto iter = m_Fonts.find(strFontTag);
    if (iter == m_Fonts.end())
        return nullptr;

    return iter->second;
}

CFont_Manager* CFont_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return new CFont_Manager(pDevice, pContext);
}

void CFont_Manager::Free()
{
    for (auto& Pair : m_Fonts)
        Safe_Release(Pair.second);
    m_Fonts.clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    __super::Free();
}