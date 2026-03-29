#include "CUI_PickPanel.h"

#include "CGameInstance.h"
#include "CPickSlot.h"

CUI_PickPanel::CUI_PickPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CUI_PickPanel::CUI_PickPanel(const CUI_PickPanel& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CUI_PickPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_PickPanel::Initialize(void* pArg)
{
    CUI_PICKPANEL_DESC* pDesc = static_cast<CUI_PICKPANEL_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_fScaleRatioX = pDesc->fScaleRatioX;
    m_fScaleRatioY = pDesc->fScaleRatioY;

    m_fPosRatioX = pDesc->fPosRatioX;
    m_fPosRatioY = pDesc->fPosRatioY;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Layer_PickSlot(TEXT("Layer_PickSlot"))))
        return E_FAIL;

    return S_OK;
}

void CUI_PickPanel::Priority_Update(_float fTimeDelta)
{
}

void CUI_PickPanel::Parallel_Update(_float fTimeDelta)
{
}

void CUI_PickPanel::Update(_float fTimeDelta)
{
}

void CUI_PickPanel::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_PickPanel::Render()
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

HRESULT CUI_PickPanel::Ready_Components()
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

HRESULT CUI_PickPanel::Bind_ShaderResources()
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

HRESULT CUI_PickPanel::Ready_Layer_PickSlot(const _wstring& strLayerTag)
{
    CPickSlot* pPickSlot{ nullptr };

    constexpr _float fSlotsPerRow = 5.f;
    constexpr _float fMarginX = 0.003f;
    constexpr _float fEndMarginX = 0.003f;
    
    constexpr _float fSlotsPerCol = 3.5f;
    constexpr _float fStartMarginY = 0.015f;
    constexpr _float fMarginY = 0.003f;

    const _float fSlotWidth = (m_fScaleRatioX / fSlotsPerRow) - (fMarginX * 2.f) - fEndMarginX;
    const _float fSlotHeight = (m_fScaleRatioY / fSlotsPerCol) - (fMarginY * 2.f);

    const _float fStartPosX = m_fPosRatioX - (m_fScaleRatioX / 2.f) + (fSlotWidth / 2.f) + fMarginX;
    const _float fGapX = fMarginX + fSlotWidth;

    const _float fStartPosY = m_fPosRatioY + (m_fScaleRatioY / 2.f) - (fSlotHeight / 2.f) - fStartMarginY;
    const _float fGapY = fMarginY + fSlotHeight;


    CPickSlot::CPICKSLOT_DESC Desc{};

    Desc.fScaleRatioX = fSlotWidth;
    Desc.fScaleRatioY = fSlotHeight;
    Desc.fPosRatioX = fStartPosX;
    Desc.fPosRatioY = fStartPosY;

    Desc.iUILayer = ETOUI(UILAYER::SLOT);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_CharPickSlot";

    Desc.funcCallBack = [this]()->void
        {
            for (auto pPickSlot : m_PickSlots) {
                pPickSlot->Set_Deselect();
            }
        };

    for (_uint i = 0; i < ETOUI(CHAR_NAME::CHARNAME_END); ++i)
    {
        Desc.tCharInfo.wstrTexturePrototypeTag = CharLobbyTex[i];
        Desc.eCharName = static_cast<CHAR_NAME>(i);

        if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_PickSlot"),
            ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&pPickSlot))))
            return E_FAIL;

        m_PickSlots.push_back(pPickSlot);

        Desc.fPosRatioX += fGapX;

        if ((i + 1) % 5 == 0) {
            Desc.fPosRatioX = fStartPosX;
            Desc.fPosRatioY -= fGapY;
        }
    }

    return S_OK;
}

CUI_PickPanel* CUI_PickPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_PickPanel* pInstance = new CUI_PickPanel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_PickPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_PickPanel::Clone(void* pArg)
{
    CUI_PickPanel* pInstance = new CUI_PickPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_PickPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_PickPanel::Free()
{
    for (auto pPickSlot : m_PickSlots) {
        Safe_Release(pPickSlot);
    }
    m_PickSlots.clear();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
