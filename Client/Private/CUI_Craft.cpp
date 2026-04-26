#include "CUI_Craft.h"

#include "CGameInstance.h"
#include "CUI_CraftSlot.h"

CUI_Craft::CUI_Craft(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{
}

CUI_Craft::CUI_Craft(const CUI_Craft& Prototype)
    : CUI_Default{ Prototype }
{
}

HRESULT CUI_Craft::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Craft::Initialize(void* pArg)
{
    CUI_DEFAULT_DESC Desc{};
    Desc.fScaleRatioX = 1.f;
    Desc.fScaleRatioY = 1.f;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI_CraftSlot(TEXT("Layer_UI_CraftSlot"))))
        return E_FAIL;

    return S_OK;
}

void CUI_Craft::Priority_Update(_float fTimeDelta)
{
}

void CUI_Craft::Parallel_Update(_float fTimeDelta)
{
}

void CUI_Craft::Update(_float fTimeDelta)
{
}

void CUI_Craft::Late_Update(_float fTimeDelta)
{
}

HRESULT CUI_Craft::Render()
{
    return S_OK;
}

void CUI_Craft::Set_CraftItem(const vector<INVENTORY_SLOT>& UICraft)
{
    _uint iCanCraftCnt = static_cast<_uint>(UICraft.size());

    for (_uint i = 0; i < m_Slots.size(); ++i)
    {
        if (iCanCraftCnt < i + 1)
        {
            m_Slots[i]->Set_CraftItem(-1, 0);
            continue;
        }

        m_Slots[i]->Set_CraftItem(UICraft[i].iItemId, UICraft[i].iItemCnt);
    }
}

HRESULT CUI_Craft::Ready_Layer_UI_CraftSlot(const _wstring& strLayerTag)
{
    const _uint iMaxSlotCnt = { 5 };
    m_Slots.reserve(iMaxSlotCnt);

    CUI_CraftSlot::CUI_CRAFTSLOT_DESC SlotDesc{};

    SlotDesc.iUILayer = ETOUI(UILAYER::PANEL);

    SlotDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    SlotDesc.wstrTexturePrototypeTag = L"Prototype_Texture_Ico_ItemGradeBg";

    SlotDesc.eBlendState = CUI_Default::DEFAULT;

    SlotDesc.fScaleRatioX = 0.044219f * 0.8f;
    SlotDesc.fScaleRatioY = 0.045897f * 0.8f;

    const _float fGapCol = SlotDesc.fScaleRatioX * 1.1f;

    const _float fStartPosX = 0.15f;
    const _float fStartPosY = -0.35f;

    SlotDesc.fPosRatioY = fStartPosY;

    for (_uint i = 0; i < 5; ++i)
    {
        SlotDesc.fPosRatioX = fStartPosX + fGapCol * i;

        Slot_Creator(strLayerTag, &SlotDesc);
    }

    return S_OK;
}

HRESULT CUI_Craft::Slot_Creator(const _wstring& strLayerTag, void* pSlotDesc)
{
    CUI_CraftSlot* pCraftSlot = { nullptr };

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_CraftSlot"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, pSlotDesc, reinterpret_cast<CGameObject**>(&pCraftSlot))))
        return E_FAIL;

    pCraftSlot->Set_IsInactive(true);

    m_Slots.push_back(pCraftSlot);

    return S_OK;
}

CUI_Craft* CUI_Craft::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Craft* pInstance = new CUI_Craft(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_Craft");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Craft::Clone(void* pArg)
{
    CUI_Craft* pInstance = new CUI_Craft(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_Craft");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Craft::Free()
{
    for (auto& pSlot : m_Slots)
    {
        Safe_Release(pSlot);
    }
    m_Slots.clear();

    __super::Free();
}
