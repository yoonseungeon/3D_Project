#include "CSkillLevelUpBtn.h"

#include "CGameInstance.h"

CSkillLevelUpBtn::CSkillLevelUpBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Btn{ pDevice, pContext }
{

}

CSkillLevelUpBtn::CSkillLevelUpBtn(const CSkillLevelUpBtn& Prototype)
    : CUI_Btn{ Prototype }
{

}

HRESULT CSkillLevelUpBtn::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkillLevelUpBtn::Initialize(void* pArg)
{
    SKILL_LEVELUP_BTN_DESC* pDesc = static_cast<SKILL_LEVELUP_BTN_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CSkillLevelUpBtn::Priority_Update(_float fTimeDelta)
{
}

void CSkillLevelUpBtn::Parallel_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    __super::Update_BtnState();

    Execute_Btn(fTimeDelta);
}

void CSkillLevelUpBtn::Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    if (m_bIsClicked) {
        BtnClick();
        m_bIsClicked = false;
    }
}

void CSkillLevelUpBtn::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CSkillLevelUpBtn::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(BLENDSTATE::ALPHABLEND)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSkillLevelUpBtn::Ready_Components()
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
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_SkillLevelUp"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CSkillLevelUpBtn::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_eCurTexState)))
        return E_FAIL;

    m_pShaderCom->Bind_RawValue("g_FlipX", &m_iFlipX, sizeof(m_iFlipX));
    m_pShaderCom->Bind_RawValue("g_FlipY", &m_iFlipY, sizeof(m_iFlipY));
    m_pShaderCom->Bind_RawValue("g_Alpha", &m_fImageAlpha, sizeof(m_fImageAlpha));
    m_pShaderCom->Bind_RawValue("g_Color", &m_vColor, sizeof(m_vColor));

    return S_OK;
}

void CSkillLevelUpBtn::BtnClick()
{
    //m_funcCallBack();
    //m_bIsInactive = true;
}

void CSkillLevelUpBtn::Execute_Btn(_float fTimeDelta)
{
    switch (m_eCurBtnState) {
    case BTN_STATE::NORMAL:
    {
        m_eCurTexState = NORMAL;
        break;
    }

    case BTN_STATE::HOVER:
    {
        m_eCurTexState = HOVER;
        break;
    }

    case BTN_STATE::PRESSED:
    {
        m_eCurTexState = NORMAL;
        break;
    }

    case BTN_STATE::CLICKED:
    {
        m_eCurTexState = NORMAL;
        m_bIsClicked = true;
        break;
    }
    }
}

CSkillLevelUpBtn* CSkillLevelUpBtn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSkillLevelUpBtn* pInstance = new CSkillLevelUpBtn(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CSkillLevelUpBtn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSkillLevelUpBtn::Clone(void* pArg)
{
    CSkillLevelUpBtn* pInstance = new CSkillLevelUpBtn(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CSkillLevelUpBtn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSkillLevelUpBtn::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
