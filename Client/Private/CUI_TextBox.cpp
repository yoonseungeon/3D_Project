#include "CUI_TextBox.h"

#include "CGameInstance.h"

CUI_TextBox::CUI_TextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Image{ pDevice, pContext }
{
}

CUI_TextBox::CUI_TextBox(const CUI_TextBox& Prototype)
    : CUI_Image{ Prototype }
{
}

HRESULT CUI_TextBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_TextBox::Initialize(void* pArg)
{
    CUI_TEXTBOX_DESC* pDesc = static_cast<CUI_TEXTBOX_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_wstrText = pDesc->wstrText;
    m_fTextureSize = pDesc->fTextureSize;

    _float fCenterX = (pDesc->fPosRatioX + 0.5f) * static_cast<_float>(g_iWinSizeX);
    _float fCenterY = -(pDesc->fPosRatioY - 0.5f) * static_cast<_float>(g_iWinSizeY);

    m_fHalfSizeX = pDesc->fScaleRatioX * static_cast<_float>(g_iWinSizeX) * 0.5f;
    m_fHalfSizeY = pDesc->fScaleRatioY * static_cast<_float>(g_iWinSizeY) * 0.5f;

    m_rcTextRange.left = static_cast<_long>(fCenterX - m_fHalfSizeX + pDesc->fOffsetX);
    m_rcTextRange.right = static_cast<_long>(fCenterX + m_fHalfSizeX + pDesc->fOffsetX);
    m_rcTextRange.top = static_cast<_long>(fCenterY - m_fHalfSizeY + pDesc->fOffsetY);
    m_rcTextRange.bottom = static_cast<_long>(fCenterY + m_fHalfSizeY + pDesc->fOffsetY);

    return S_OK;
}

void CUI_TextBox::Priority_Update(_float fTimeDelta)
{
}

void CUI_TextBox::Parallel_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

}

void CUI_TextBox::Update(_float fTimeDelta)
{

}

void CUI_TextBox::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_TextBox::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(ETOUI(m_eBlendState))))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    const _float fTextStartX = static_cast<_float>(m_rcTextRange.left);
    const _float fTextStartY = static_cast<_float>(m_rcTextRange.top);
    const _float fAdjustSize = 0.0452f;

    m_pGameInstance->Draw_Text(TEXT("Font_Pretendard_SemiBold"),
        m_wstrText.data(),
        _float2(fTextStartX, fTextStartY),
        XMVectorSet(1.f, 1.f, 1.f, 1.f),
        _float2(m_fTextureSize, m_fTextureSize)
    );

    return S_OK;
}

HRESULT CUI_TextBox::Ready_Components()
{
    return S_OK;
}

HRESULT CUI_TextBox::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(CUIObject::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(CUIObject::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIdx)))
        return E_FAIL;

    m_pShaderCom->Bind_RawValue("g_FlipX", &m_iFlipX, sizeof(m_iFlipX));
    m_pShaderCom->Bind_RawValue("g_FlipY", &m_iFlipY, sizeof(m_iFlipY));
    m_pShaderCom->Bind_RawValue("g_Alpha", &m_fImageAlpha, sizeof(m_fImageAlpha));

    return S_OK;
}

CUI_TextBox* CUI_TextBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_TextBox* pInstance = new CUI_TextBox(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_TextBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_TextBox::Clone(void* pArg)
{
    CUI_TextBox* pInstance = new CUI_TextBox(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_TextBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_TextBox::Free()
{
    __super::Free();
}
