#include "CUI_Inventory.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"
#include "CAbstractPlayer.h"
#include "CInventory.h"

#include "CUI_InventorySlot.h"
#include "CUI_Image.h"

CUI_Inventory::CUI_Inventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CUI_Inventory::CUI_Inventory(const CUI_Inventory& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CUI_Inventory::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Inventory::Initialize(void* pArg)
{
    CUI_DEFAULT_DESC Desc{};
    Desc.fScaleRatioX = 1.f;
    Desc.fScaleRatioY = 1.f;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI_Image(TEXT("Layer_UI_Image"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI_Inventory(TEXT("Layer_UI_Inventory"))))
        return E_FAIL;

    if (FAILED(Initialize_Inventory()))
        return E_FAIL;    

    return S_OK;
}

void CUI_Inventory::Priority_Update(_float fTimeDelta)
{
}

void CUI_Inventory::Parallel_Update(_float fTimeDelta)
{
}

void CUI_Inventory::Update(_float fTimeDelta)
{
}

void CUI_Inventory::Late_Update(_float fTimeDelta)
{
    Sync_Inventory();
}

HRESULT CUI_Inventory::Render()
{
    return S_OK;
}

HRESULT CUI_Inventory::Ready_Layer_UI_Image(const _wstring& strLayerTag)
{
    // Line
    CUI_Image::CUI_IMAGE_DESC Desc{};

    Desc.fScaleRatioX = 0.238594f;
    Desc.fScaleRatioY = 0.003056f;
    Desc.fPosRatioX = 0.205391f;
    Desc.fPosRatioY = -0.378333f;
    
    Desc.iUILayer = ETOUI(UILAYER::DECO_LAYER1);
    
    Desc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_WhiteBlock";
    
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.fImageAlpha = 0.2f;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
        return E_FAIL;


    // Crafting Deco
    CUI_Image::CUI_IMAGE_DESC CraftingDesc{};

    CraftingDesc.fScaleRatioX = 0.02f;
    CraftingDesc.fScaleRatioY = CraftingDesc.fScaleRatioX * g_iWinSizeX / g_iWinSizeY;
    CraftingDesc.fPosRatioX = 0.105313f;
    CraftingDesc.fPosRatioY = -0.346805f;
    
    CraftingDesc.iUILayer = ETOUI(UILAYER::DECO_LAYER1);
    
    CraftingDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    CraftingDesc.wstrTexturePrototypeTag = L"Prototype_Texture_Ico_Crafting";
    
    CraftingDesc.eBlendState = CUI_Default::ALPHABLEND;
    CraftingDesc.fImageAlpha = 0.4f;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &CraftingDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Inventory::Ready_Layer_UI_Inventory(const _wstring& strLayerTag)
{
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
            SlotDesc.fPosRatioX = fStartPosX + fGapCol * j;
            SlotDesc.fPosRatioY = fStartPosY - fGapRow * i;

            Slot_Creator(strLayerTag, &SlotDesc);
        }
    }  

    return S_OK;
}

HRESULT CUI_Inventory::Slot_Creator(const _wstring& strLayerTag, void* pSlotDesc)
{
    CUI_InventorySlot* pInventorySlot = { nullptr };

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_InventorySlot"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, pSlotDesc, reinterpret_cast<CGameObject**>(&pInventorySlot))))
        return E_FAIL;

    m_Slots.push_back(pInventorySlot);

    return S_OK;
}

HRESULT CUI_Inventory::Initialize_Inventory()
{
    const CAbstractPlayer* pPlayer = CInGame_Manager::GetInstance()->Get_Player();        

    if (pPlayer == nullptr)
    {
        MSG_BOX("Failed to Initialize Inventory: CUI_Inventory");
        return E_FAIL;
    }

    const CInventory* pInventory = pPlayer->Get_Inventory();

    if (pInventory == nullptr)
    {
        MSG_BOX("Failed to Initialize Inventory: CUI_Inventory");
        return E_FAIL;
    }

    const vector<INVENTORY_SLOT>& Inventory = pInventory->Get_InventoryVec();

    if (Inventory.size() == 0)
    {
        MSG_BOX("Failed to Initialize Inventory: CUI_Inventory");
        return E_FAIL;
    }

    m_UIInventory = Inventory;

    return S_OK;
}

void CUI_Inventory::Sync_Inventory()
{
    const CInventory* pInventory = CInGame_Manager::GetInstance()->Get_Player()->Get_Inventory();
    
    if (m_iChangeFlag == pInventory->Get_ChangeFlag())
    {
        return;
    }

    m_iChangeFlag = pInventory->Get_ChangeFlag();

    const vector<INVENTORY_SLOT>& Inventory = pInventory->Get_InventoryVec();
    m_UIInventory = Inventory;

    Sync_InventorySlot();
}

void CUI_Inventory::Sync_InventorySlot()
{
    for (_uint i = 0; i < m_Slots.size(); ++i)
    {        
        m_Slots[i]->Sync_Slot_Bg_Item(m_UIInventory[i].iItemId, m_UIInventory[i].iItemCnt);
    }
}

CUI_Inventory* CUI_Inventory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Inventory* pInstance = new CUI_Inventory(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_Inventory");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Inventory::Clone(void* pArg)
{
    CUI_Inventory* pInstance = new CUI_Inventory(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_Inventory");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Inventory::Free()
{
    for (auto& pSlot : m_Slots)
    {
        Safe_Release(pSlot);
    }
    m_Slots.clear();

    __super::Free();
}
