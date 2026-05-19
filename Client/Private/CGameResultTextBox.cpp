#include "CGameResultTextBox.h"

#include "CGameInstance.h"

CGameResultTextBox::CGameResultTextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CGameResultTextBox::CGameResultTextBox(const CGameResultTextBox& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CGameResultTextBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGameResultTextBox::Initialize(void* pArg)
{
    GAMERESULT_TEXTBOX_DESC* pDesc = static_cast<GAMERESULT_TEXTBOX_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fScaleY = static_cast<_float>(g_iWinSizeY) / 4.f;
    m_wstrText = L"결과 로딩 중...";

    return S_OK;
}

void CGameResultTextBox::Priority_Update(_float fTimeDelta)
{
}

void CGameResultTextBox::Parallel_Update(_float fTimeDelta)
{
}

void CGameResultTextBox::Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    Update_GameResult();
}

void CGameResultTextBox::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CGameResultTextBox::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(ETOUI(m_eBlendState))))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    if (FAILED(RenderText()))
        return E_FAIL;

    return S_OK;
}

void CGameResultTextBox::GameResultStart()
{
    m_bIsInactive = false;
    m_pTransformCom->Set_Scale(0.0001f, m_fScaleY, 1.f);
}

void CGameResultTextBox::GameResultReset()
{
    m_bIsInactive = true;
    m_bRenderText = false;
    m_fRatio = 0.f;
}

HRESULT CGameResultTextBox::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* For.Com_Texture*/
    if (FAILED(__super::Add_Component(ETOUI(m_eTexPrototypeLV), m_wstrTexturePrototypeTag,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CGameResultTextBox::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIdx)))
        return E_FAIL;

    m_pShaderCom->Bind_RawValue("g_FlipX", &m_iFlipX, sizeof(m_iFlipX));
    m_pShaderCom->Bind_RawValue("g_FlipY", &m_iFlipY, sizeof(m_iFlipY));
    m_pShaderCom->Bind_RawValue("g_Alpha", &m_fImageAlpha, sizeof(m_fImageAlpha));
    m_pShaderCom->Bind_RawValue("g_Color", &m_vColor, sizeof(m_vColor));

    return S_OK;
}

void CGameResultTextBox::Update_GameResult()
{
    // TextBox
    const _float fStart = 0.16f;
    const _float fEnd = 0.28f;

    _float fTextBoxRatio = (m_fRatio - fStart) / (fEnd - fStart);
    // 0.0001f Scale x가 0이면 트랜스폼 벡터에 문제가 생김
    MyHelper::FloatClamp(fTextBoxRatio, 0.0001f, 1.f);

    const _float fScaleX = static_cast<_float>(g_iWinSizeX) * fTextBoxRatio;

    m_pTransformCom->Set_Scale(fScaleX, m_fScaleY, 1.f);

    // Alpha
    const _float fAlphaStart = 0.20f;
    const _float fAlphaEnd = 0.36f;

    _float fAlphaRatio = (m_fRatio - fAlphaStart) / (fAlphaEnd - fAlphaStart);
    MyHelper::FloatClamp(fTextBoxRatio, 0.0001f, 1.f);
    m_fImageAlpha = 1.f - fAlphaRatio;

    const _float fRenderTextRatio = 0.18f;
    if (m_fRatio >= fRenderTextRatio)
        m_bRenderText = true;
}

HRESULT CGameResultTextBox::RenderText()
{
    if (m_bRenderText == false)
        return S_OK;

    _float fAdjustFontSize = fDefaultFontSize * 1.3f;

    const _float2 vLength = m_pGameInstance->Cal_FontLength(L"Pretendard_Middle_100", m_wstrText);

    const _float fTextStartX = static_cast<_float>(g_iWinSizeX) * 0.5f - vLength.x * fAdjustFontSize * 0.5f;
    const _float fTextStartY = static_cast<_float>(g_iWinSizeY) * 0.5f - vLength.y * fAdjustFontSize * 0.5f;

    m_pGameInstance->Draw_Text(TEXT("Pretendard_Middle_100"),
        m_wstrText.data(),
        _float2(fTextStartX, fTextStartY),
        XMVectorSet(0.02f, 0.71f, 0.97f, 1.f),
        _float2(fAdjustFontSize, fAdjustFontSize)
    );

    return S_OK;
}

CGameResultTextBox* CGameResultTextBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGameResultTextBox* pInstance = new CGameResultTextBox(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CGameResultTextBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGameResultTextBox::Clone(void* pArg)
{
    CGameResultTextBox* pInstance = new CGameResultTextBox(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CGameResultTextBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGameResultTextBox::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}