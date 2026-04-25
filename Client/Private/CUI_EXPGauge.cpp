#include "CUI_EXPGauge.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"

#include "CAbstractPlayer.h"

CUI_EXPGauge::CUI_EXPGauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{
}

CUI_EXPGauge::CUI_EXPGauge(const CUI_EXPGauge& Prototype)
    : CUI_Default{ Prototype }
{
}

HRESULT CUI_EXPGauge::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_EXPGauge::Initialize(void* pArg)
{
    CUI_EXPGAUGE_DESC* pDesc = static_cast<CUI_EXPGAUGE_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CUI_EXPGauge::Priority_Update(_float fTimeDelta)
{
}

void CUI_EXPGauge::Parallel_Update(_float fTimeDelta)
{
}

void CUI_EXPGauge::Update(_float fTimeDelta)
{
    CAbstractPlayer* pPlayer = CInGame_Manager::GetInstance()->Get_Player();
    const CUnit::UNIT_STAT& tFinalStat = pPlayer->Get_FinalStat();
    const CUnit::UNIT_STAT& tCurStat = pPlayer->Get_CurStat();
    
    const _float fCurEXP = static_cast<_float>(tCurStat.iEXP) / static_cast<_float>(tFinalStat.iEXP);

    if (fCurEXP != m_fData)
    {
        m_fData = fCurEXP;
        m_fFillY = m_fData;
    }
}

void CUI_EXPGauge::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_EXPGauge::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(m_eBlendState)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_EXPGauge::Ready_Components()
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

HRESULT CUI_EXPGauge::Bind_ShaderResources()
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

    m_pShaderCom->Bind_RawValue("g_UVFillX", &m_fFillX, sizeof(m_fFillX));
    m_pShaderCom->Bind_RawValue("g_UVFillY", &m_fFillY, sizeof(m_fFillY));

    return S_OK;
}

CUI_EXPGauge* CUI_EXPGauge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_EXPGauge* pInstance = new CUI_EXPGauge(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_EXPGauge");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_EXPGauge::Clone(void* pArg)
{
    CUI_EXPGauge* pInstance = new CUI_EXPGauge(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_EXPGauge");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_EXPGauge::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}