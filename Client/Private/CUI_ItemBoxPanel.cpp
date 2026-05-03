#include "CUI_ItemBoxPanel.h"

#include "CGameInstance.h"
#include "CItem_Manager.h"

#include "CUI_InventorySlot.h"

CUI_ItemBoxPanel::CUI_ItemBoxPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CUI_ItemBoxPanel::CUI_ItemBoxPanel(const CUI_ItemBoxPanel& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CUI_ItemBoxPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_ItemBoxPanel::Initialize(void* pArg)
{
    m_pCItem_Manager = CItem_Manager::GetInstance();
    Safe_AddRef(m_pCItem_Manager);

    CUI_ITEMBOXPANEL_DESC* pDesc = static_cast<CUI_ITEMBOXPANEL_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI_InventorySlot(TEXT("Layer_UI_InventorySlot"))))
        return E_FAIL;

    if (FAILED(Initialize_Inventory()))
        return E_FAIL;



    return S_OK;
}

void CUI_ItemBoxPanel::Priority_Update(_float fTimeDelta)
{
}

void CUI_ItemBoxPanel::Parallel_Update(_float fTimeDelta)
{
}

void CUI_ItemBoxPanel::Update(_float fTimeDelta)
{
}

void CUI_ItemBoxPanel::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_ItemBoxPanel::Render()
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

HRESULT CUI_ItemBoxPanel::Ready_Components()
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

HRESULT CUI_ItemBoxPanel::Bind_ShaderResources()
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

HRESULT CUI_ItemBoxPanel::Ready_Layer_UI_InventorySlot(const _wstring& strLayerTag)
{
   /* const _uint iMaxSlotCnt = { 10 };
    m_Slots.reserve(iMaxSlotCnt);

    CUI_InventorySlot::CUI_INVENTORYSLOT_DESC SlotDesc{};

    SlotDesc.iUILayer = ETOUI(UILAYER::PANEL);

    SlotDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    SlotDesc.wstrTexturePrototypeTag = L"Prototype_Texture_WhiteBlock";

    SlotDesc.eBlendState = CUI_Default::COLOR_ALPHABLEND;
    SlotDesc.vColor = COLOR_TO_FLOAT(33, 45, 51);
    SlotDesc.fImageAlpha = 0.5f;

    SlotDesc.fScaleRatioX = 0.044219f;
    SlotDesc.fScaleRatioY = 0.045897f;

    const _float fGapCol = SlotDesc.fScaleRatioX * 1.1f;
    const _float fGapRow = SlotDesc.fScaleRatioY * 1.2f;

    const _float fStartPosX = 0.107578f;
    const _float fStartPosY = -0.415139f;

    for (_uint i = 0; i < 2; ++i)
    {
        for (_uint j = 0; j < 5; ++j)
        {
            SlotDesc.iSlotIndex = i * 5 + j;
            SlotDesc.fPosRatioX = fStartPosX + fGapCol * j;
            SlotDesc.fPosRatioY = fStartPosY - fGapRow * i;

            Slot_Creator(strLayerTag, &SlotDesc);
        }
    }*/

    return S_OK;
}

HRESULT CUI_ItemBoxPanel::Slot_Creator(const _wstring& strLayerTag, void* pSlotDesc)
{
    //CUI_InventorySlot* pInventorySlot = { nullptr };

    //if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_InventorySlot"),
    //    ETOUI(LEVEL::GAMEPLAY), strLayerTag, pSlotDesc, reinterpret_cast<CGameObject**>(&pInventorySlot))))
    //    return E_FAIL;

    //m_Slots.push_back(pInventorySlot);

    return S_OK;
}

HRESULT CUI_ItemBoxPanel::Initialize_Inventory()
{
    m_ItemBoxInventory.reserve(10);

    return S_OK;
}

void CUI_ItemBoxPanel::Sync_InventorySlot()
{
    //for (_uint i = 0; i < m_Slots.size(); ++i)
    //{
    //    m_Slots[i]->Sync_Slot_Bg_Item(m_ItemBoxInventory[i].iItemId, m_ItemBoxInventory[i].iItemCnt);
    //}
}

CUI_ItemBoxPanel* CUI_ItemBoxPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_ItemBoxPanel* pInstance = new CUI_ItemBoxPanel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_ItemBoxPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_ItemBoxPanel::Clone(void* pArg)
{
    CUI_ItemBoxPanel* pInstance = new CUI_ItemBoxPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_ItemBoxPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_ItemBoxPanel::Free()
{
    //for (auto& pSlot : m_Slots)
    //{
    //    Safe_Release(pSlot);
    //}
    //m_Slots.clear();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    Safe_Release(m_pCItem_Manager);

    __super::Free();
}
