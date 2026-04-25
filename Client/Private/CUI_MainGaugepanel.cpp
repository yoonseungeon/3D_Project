#include "CUI_MainGaugePanel.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"

#include "CAbstractPlayer.h"
#include "CUI_MainGaugeBar.h"

CUI_MainGaugePanel::CUI_MainGaugePanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{
}

CUI_MainGaugePanel::CUI_MainGaugePanel(const CUI_MainGaugePanel& Prototype)
    : CUI_Default{ Prototype }
{
}

HRESULT CUI_MainGaugePanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_MainGaugePanel::Initialize(void* pArg)
{
    CUI_MAINGAUGE_DESC* pDesc = static_cast<CUI_MAINGAUGE_DESC*>(pArg);

    m_fScaleRatioX = pDesc->fScaleRatioX;
    m_fScaleRatioY = pDesc->fScaleRatioY;
    m_fPosRatioX = pDesc->fPosRatioX;
    m_fPosRatioY = pDesc->fPosRatioY;

    m_eMainGaugeType = pDesc->eMainGaugeType;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Layer_MainGaugeBar(TEXT("Layer_UI_MainGaugeBar"))))
        return E_FAIL;

    return S_OK;
}

void CUI_MainGaugePanel::Priority_Update(_float fTimeDelta)
{
}

void CUI_MainGaugePanel::Parallel_Update(_float fTimeDelta)
{
}

void CUI_MainGaugePanel::Update(_float fTimeDelta)
{
}

void CUI_MainGaugePanel::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_MainGaugePanel::Render()
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

HRESULT CUI_MainGaugePanel::Ready_Components()
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

HRESULT CUI_MainGaugePanel::Bind_ShaderResources()
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

HRESULT CUI_MainGaugePanel::Ready_Layer_MainGaugeBar(const _wstring& strLayerTag)
{
    if (m_eMainGaugeType == MAINGAUGE_TYPE::NONE)
    {
        return S_OK;
    }

    CUI_MainGaugeBar::CUI_MAINGAUGEBAR_DESC Desc{};

    Desc.fScaleRatioX = m_fScaleRatioX;
    Desc.fScaleRatioY = m_fScaleRatioY * 0.95f;
    Desc.fPosRatioX = m_fPosRatioX;
    Desc.fPosRatioY = m_fPosRatioY;

    Desc.iUILayer = ETOUI(UILAYER::PANEL_SLOT_OVER);

    Desc.eTexPrototypeLV = LEVEL::GAMEPLAY;

    Desc.eBlendState = CUI_Default::ALPHABLEND_GAUGE;

    Desc.eGaugeType = m_eMainGaugeType;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_MainGaugeBar"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

CUI_MainGaugePanel* CUI_MainGaugePanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_MainGaugePanel* pInstance = new CUI_MainGaugePanel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_MainGaugePanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_MainGaugePanel::Clone(void* pArg)
{
    CUI_MainGaugePanel* pInstance = new CUI_MainGaugePanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_MainGaugePanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_MainGaugePanel::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
