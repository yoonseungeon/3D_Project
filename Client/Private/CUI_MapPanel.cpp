#include "CUI_MapPanel.h"

#include "CGameInstance.h"
#include "CCharData_Manager.h"
#include "CMapSelectBtn.h"

CUI_MapPanel::CUI_MapPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CUI_MapPanel::CUI_MapPanel(const CUI_MapPanel& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CUI_MapPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_MapPanel::Initialize(void* pArg)
{
    m_pCharData_Manager = CCharData_Manager::GetInstance();
    Safe_AddRef(m_pCharData_Manager);

    CUI_MAPPANEL_DESC* pDesc = static_cast<CUI_MAPPANEL_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL; 

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Layer_MapSelectBtn(TEXT("Layer_CMapSelectBtn"))))
        return E_FAIL;
    
    return S_OK;
}

void CUI_MapPanel::Priority_Update(_float fTimeDelta)
{
}

void CUI_MapPanel::Parallel_Update(_float fTimeDelta)
{
}

void CUI_MapPanel::Update(_float fTimeDelta)
{
}

void CUI_MapPanel::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_MapPanel::Render()
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

void CUI_MapPanel::Set_IsInactive(_bool bIsInactive)
{
    m_bIsInactive = bIsInactive;

}

HRESULT CUI_MapPanel::Ready_Components()
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

HRESULT CUI_MapPanel::Bind_ShaderResources()
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

    return S_OK;
}

HRESULT CUI_MapPanel::Ready_Layer_MapSelectBtn(const _wstring& strLayerTag)
{
    CMapSelectBtn::CMAPSELECTBTN_DESC Desc{};

    Desc.fScaleRatioX = 0.25f;
    Desc.fScaleRatioY = 0.25f;
    Desc.fPosRatioX = 0.f;
    Desc.fPosRatioY = 0.f;
    Desc.iUILayer = ETOUI(UILAYER::BUTTON);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.wstrTexturePrototypeTag = MAPS[0].TEX_OVER_TAG;


    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_MapSelectBtn"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

CUI_MapPanel* CUI_MapPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_MapPanel* pInstance = new CUI_MapPanel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_MapPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_MapPanel::Clone(void* pArg)
{
    CUI_MapPanel* pInstance = new CUI_MapPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_MapPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_MapPanel::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    Safe_Release(m_pCharData_Manager);

    __super::Free();
}
