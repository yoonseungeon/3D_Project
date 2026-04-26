#include "CUI_Inventory.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"
#include "CItem_Manager.h"

#include "CAbstractPlayer.h"
#include "CInventory.h"

#include "CUI_InventorySlot.h"
#include "CUI_Image.h"
#include "CUI_Craft.h"

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
    m_pCItem_Manager = CItem_Manager::GetInstance();
    Safe_AddRef(m_pCItem_Manager);

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

    if (FAILED(Ready_Layer_UI_Craft(TEXT("Layer_UI_Craft"))))
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
    const _uint iMaxSlotCnt = { 10 };
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

HRESULT CUI_Inventory::Ready_Layer_UI_Craft(const _wstring& strLayerTag)
{
    // CUI_Craft
    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_Craft"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, nullptr, reinterpret_cast<CGameObject**>(&m_pCraft))))
        return E_FAIL;

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
        m_UICraft;
        return;
    }

    m_iChangeFlag = pInventory->Get_ChangeFlag();

    const vector<INVENTORY_SLOT>& Inventory = pInventory->Get_InventoryVec();
    m_UIInventory = Inventory;

    Sync_InventorySlot();

    // UIInventory 갱신된 후 호출
    Sync_Craft();
}

void CUI_Inventory::Sync_InventorySlot()
{
    for (_uint i = 0; i < m_Slots.size(); ++i)
    {        
        m_Slots[i]->Sync_Slot_Bg_Item(m_UIInventory[i].iItemId, m_UIInventory[i].iItemCnt);
    }
}

void CUI_Inventory::Sync_Craft()
{
    m_UICraft.clear();

    unordered_set<_uint> CandidateItems;

    for (_uint i = 0; i < m_UIInventory.size(); ++i)
    {
        _int iItemIdx = m_UIInventory[i].iItemId;

        if (iItemIdx == -1)
        {
            continue;
        }

        const vector<_uint>* CanCraftItemCandidates = m_pCItem_Manager->Get_CanCraftItemCandidates(iItemIdx);

        if (CanCraftItemCandidates == nullptr)
        {
            continue;
        }

        // 중복 제거
        for (_int iItemIdx : *CanCraftItemCandidates)
        {
            CandidateItems.insert(iItemIdx);
        }
    }
    
    for (_int iItemIdx : CandidateItems)
    {
        if (Can_Craft(iItemIdx) == true)
        {
            const ITEM_DESC* tItemDesc = m_pCItem_Manager->Find_ItemInfo(iItemIdx);
            m_UICraft.emplace_back(INVENTORY_SLOT{ iItemIdx, tItemDesc->iCraftCnt });
        }
    }

    std::sort(m_UICraft.begin(), m_UICraft.end(),
        [this](const INVENTORY_SLOT& tLeft, const INVENTORY_SLOT& tRight)->_bool
        {
            const ITEM_DESC* tLeftItemDesc = m_pCItem_Manager->Find_ItemInfo(tLeft.iItemId);
            const ITEM_DESC* tRightItemDesc = m_pCItem_Manager->Find_ItemInfo(tRight.iItemId);

            return tLeftItemDesc->eGrade > tRightItemDesc->eGrade;
        }
    );

    m_pCraft->Set_CraftItem(m_UICraft);
}

_bool CUI_Inventory::Can_Craft(_uint iItemIdx)
{
    // 같은 아이템이 제작에 두 개 필요한 아이템은 없음 -> 체크 로직 없음

    const ITEM_DESC* tItem_Desc = m_pCItem_Manager->Find_ItemInfo(iItemIdx);
        
    if (tItem_Desc == nullptr)
    {
        MSG_BOX("No ITEM_DESC In CItem_Manager: CUI_Inventory");
        return false;
    }

    for (_uint i = 0; i < sizeof(tItem_Desc->materials) / sizeof(_int); ++i)
    {
        if (tItem_Desc->materials[i] == -1)
        {
            continue;
        }

        _bool bFind{};

        for (_uint j = 0; j < m_UIInventory.size(); ++j)
        {
            if(m_UIInventory[j].iItemId == tItem_Desc->materials[i])
            {
                bFind = true;
                break;
            }
        }

        if (bFind == false)
        {
            return false;
        }
    }

    return true;
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
    Safe_Release(m_pCItem_Manager);

    for (auto& pSlot : m_Slots)
    {
        Safe_Release(pSlot);
    }
    m_Slots.clear();

    Safe_Release(m_pCraft);

    __super::Free();
}
