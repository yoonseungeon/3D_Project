#include "CItem_Manager.h"

IMPLEMENT_SINGLETON(CItem_Manager)

CItem_Manager::CItem_Manager()
{
    if (FAILED(Initialize()))
        MSG_BOX("CItem_Manager: Error");
}

const ITEM_DESC* CItem_Manager::Find_ItemInfo(_uint iItemIdx) const
{
    auto iter = m_ItemInfos.find(iItemIdx);

    if(iter == m_ItemInfos.end())
        return nullptr;

    return &(iter->second);
}

const vector<_uint>* CItem_Manager::Get_CanCraftItemCandidates(_uint iItemIdx) const
{
    auto iter = m_Reversematerials.find(iItemIdx);

    if (iter == m_Reversematerials.end())
        return nullptr;

    return &(iter->second);
}

_float CItem_Manager::Get_CurItemCraftTime(_uint iItemIdx)
{
    const ITEM_DESC*  pItemDesc = Find_ItemInfo(iItemIdx);
    if (pItemDesc == nullptr) {
        return 0.f;
    }

    return m_fItemCraftTimes[ETOUI(pItemDesc->eGrade)];
}

HRESULT CItem_Manager::Initialize()
{   
    if (FAILED(Set_ItemInfos()))
        return E_FAIL;

    if (FAILED(Set_Reversematerials()))
        return E_FAIL;

    if (FAILED(Set_ItemCraftTime()))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CItem_Manager::Set_ItemInfos()
{    
    m_ItemInfos.clear();

    ITEM_DESC tItemDesc{};

    for (_uint i = 0; i < sizeof(ItemInfos) / sizeof(ITEM_DESC); ++i)
    {
        m_ItemInfos.emplace(ItemInfos[i].iItemID, ItemInfos[i]);
    }

    return S_OK;
}

HRESULT CItem_Manager::Set_Reversematerials()
{
    m_Reversematerials.clear();

    for (auto& pair : m_ItemInfos) {

        for (_uint i = 0; i < sizeof(pair.second.materials) / sizeof(_int); ++i)
        {
            if (pair.second.materials[i] == -1)
            {
                continue;
            }

            m_Reversematerials[pair.second.materials[i]].push_back(pair.second.iItemID);
        }        
    }

    return S_OK;
}

HRESULT CItem_Manager::Set_ItemCraftTime()
{
    memcpy(m_fItemCraftTimes, ItemCraftTime, sizeof(m_fItemCraftTimes));

    return S_OK;
}

void CItem_Manager::Free()
{
    m_ItemInfos.clear();
    m_Reversematerials.clear();

    __super::Free();
}