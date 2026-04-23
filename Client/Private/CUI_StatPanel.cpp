#include "CUI_StatPanel.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"

#include "CAbstractPlayer.h"

#include "CUI_StatBox.h"

CUI_StatPanel::CUI_StatPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CUI_StatPanel::CUI_StatPanel(const CUI_StatPanel& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CUI_StatPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_StatPanel::Initialize(void* pArg)
{
    CUI_STATPANEL_DESC* pDesc = static_cast<CUI_STATPANEL_DESC*>(pArg);

    m_fScaleRatioX = pDesc->fScaleRatioX;
    m_fScaleRatioY = pDesc->fScaleRatioY;
    m_fPosRatioX = pDesc->fPosRatioX;
    m_fPosRatioY = pDesc->fPosRatioY;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI_Image(TEXT("Layer_UI_Image"))))
        return E_FAIL;

    return S_OK;
}

void CUI_StatPanel::Priority_Update(_float fTimeDelta)
{
}

void CUI_StatPanel::Parallel_Update(_float fTimeDelta)
{
}

void CUI_StatPanel::Update(_float fTimeDelta)
{
}

void CUI_StatPanel::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_StatPanel::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(ETOUI(m_eBlendState))))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_StatPanel::Ready_Components()
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

HRESULT CUI_StatPanel::Bind_ShaderResources()
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

HRESULT CUI_StatPanel::Ready_Layer_UI_Image(const _wstring& strLayerTag)
{
    CUI_StatBox::CUI_STATBOX_DESC StatBoxDesc{};

    StatBoxDesc.fScaleRatioX = 0.015f;
    StatBoxDesc.fScaleRatioY = StatBoxDesc.fScaleRatioX / g_iWinSizeY * g_iWinSizeX;
    StatBoxDesc.fPosRatioX = 0.f;
    StatBoxDesc.fPosRatioY = 0.f;

    StatBoxDesc.iUILayer = ETOUI(UILAYER::SLOT);

    StatBoxDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;

    StatBoxDesc.eBlendState = CUI_Default::COLOR_ALPHABLEND;
    StatBoxDesc.vColor = COLOR_TO_FLOAT(33, 45, 51);

    StatBoxDesc.eStatBoxType = CUI_StatBox::PenetrationDefense;


    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_StatBox"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &StatBoxDesc)))
        return E_FAIL;

    return S_OK;
}

CUI_StatPanel* CUI_StatPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_StatPanel* pInstance = new CUI_StatPanel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_StatPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_StatPanel::Clone(void* pArg)
{
    CUI_StatPanel* pInstance = new CUI_StatPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_StatPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_StatPanel::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
