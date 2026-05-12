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
    if (m_bIsInactive == true) {
        return;
    }

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

    return S_OK;
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

    m_pShaderCom->Bind_RawValue("g_FlipX", &m_iFlipX, sizeof(m_iFlipX));
    m_pShaderCom->Bind_RawValue("g_FlipY", &m_iFlipY, sizeof(m_iFlipY));
    m_pShaderCom->Bind_RawValue("g_Alpha", &m_fImageAlpha, sizeof(m_fImageAlpha));
    m_pShaderCom->Bind_RawValue("g_Color", &m_vColor, sizeof(m_vColor));

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
