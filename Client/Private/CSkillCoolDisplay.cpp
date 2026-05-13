#include "CSkillCoolDisplay.h"

#include "CGameInstance.h"

CSkillCoolDisplay::CSkillCoolDisplay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CSkillCoolDisplay::CSkillCoolDisplay(const CSkillCoolDisplay& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CSkillCoolDisplay::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkillCoolDisplay::Initialize(void* pArg)
{
    SKILLCOOLDISPLAY_DESC* pDesc = static_cast<SKILLCOOLDISPLAY_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_vSkillCoolColor = { 0.153f, 0.379f, 0.682f };
    m_fImageAlpha = 0.6f;

    _float fCenterX = (pDesc->fPosRatioX + 0.5f) * static_cast<_float>(g_iWinSizeX);
    _float fCenterY = -(pDesc->fPosRatioY - 0.5f) * static_cast<_float>(g_iWinSizeY);

    m_fHalfSizeX = pDesc->fScaleRatioX * static_cast<_float>(g_iWinSizeX) * 0.5f;
    m_fHalfSizeY = pDesc->fScaleRatioY * static_cast<_float>(g_iWinSizeY) * 0.5f;

    m_rcTextRange.left = static_cast<_long>(fCenterX - m_fHalfSizeX);
    m_rcTextRange.right = static_cast<_long>(fCenterX + m_fHalfSizeX);
    m_rcTextRange.top = static_cast<_long>(fCenterY - m_fHalfSizeY);
    m_rcTextRange.bottom = static_cast<_long>(fCenterY + m_fHalfSizeY);

    m_fScaleRatioX = pDesc->fScaleRatioX;
    m_fScaleRatioY = pDesc->fScaleRatioY;

    return S_OK;
}

void CSkillCoolDisplay::Priority_Update(_float fTimeDelta)
{
}

void CSkillCoolDisplay::Parallel_Update(_float fTimeDelta)
{
}

void CSkillCoolDisplay::Update(_float fTimeDelta)
{
}

void CSkillCoolDisplay::Late_Update(_float fTimeDelta)
{
    //if (m_fCurSkillCool <= 0.f)
    //    m_bIsInactive = true;

    if (m_bIsInactive == true)
        return;

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CSkillCoolDisplay::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(ETOUI(BLENDSTATE::SKILL_COOL))))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    if (FAILED(RenderText()))
        return E_FAIL;

    return S_OK;
}

void CSkillCoolDisplay::Set_CoolTime(const _float fMaxSkillCool, const _float fCurSkillCool)
{
    m_fMaxSkillCool = fMaxSkillCool;
    m_fCurSkillCool = fCurSkillCool;

    m_bIsInactive = false;
}

HRESULT CSkillCoolDisplay::Ready_Components()
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
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_WhiteBlock"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CSkillCoolDisplay::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    m_pShaderCom->Bind_RawValue("g_Alpha", &m_fImageAlpha, sizeof(m_fImageAlpha));

    _float fCoolRatio{};
    if (m_fMaxSkillCool == 0.f)
        fCoolRatio = 0.f;
    else
        fCoolRatio = m_fCurSkillCool / m_fMaxSkillCool;

    m_pShaderCom->Bind_RawValue("g_SkillCoolRatio", &fCoolRatio, sizeof(fCoolRatio));
    m_pShaderCom->Bind_RawValue("g_SkillCoolColor", &m_vSkillCoolColor, sizeof(m_vSkillCoolColor));

    return S_OK;
}

HRESULT CSkillCoolDisplay::RenderText()
{
    if (m_bTextRender == false)
        return S_OK;

    _float fAdjustFontSize = fDefaultFontSize * 0.6f;

    const _float2 vLength = m_pGameInstance->Cal_FontLength(L"Font_Pretendard_Middle", m_wstCoolText);
    
    const _float fTextStartX = static_cast<_float>((m_rcTextRange.right) + static_cast<_float>(m_rcTextRange.left)) * 0.5f
        - vLength.x * fAdjustFontSize * 0.5f;
    const _float fTextStartY = static_cast<_float>(m_rcTextRange.top) + m_fScaleRatioY * 0.25f * g_iWinSizeY;

    m_pGameInstance->Draw_Text(TEXT("Font_Pretendard_Middle"),
        m_wstCoolText.data(),
        _float2(fTextStartX, fTextStartY),
        XMVectorSet(1.f, 1.f, 1.f, 1.f),
        _float2(fAdjustFontSize, fAdjustFontSize)
    );

    return S_OK;
}

CSkillCoolDisplay* CSkillCoolDisplay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSkillCoolDisplay* pInstance = new CSkillCoolDisplay(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CSkillCoolDisplay");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSkillCoolDisplay::Clone(void* pArg)
{
    CSkillCoolDisplay* pInstance = new CSkillCoolDisplay(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CSkillCoolDisplay");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSkillCoolDisplay::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
