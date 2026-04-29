#include "CAbstractPlayer.h"

#include "CGameInstance.h"
#include "CInventory.h"
#include "CEquipment.h"
#include "CCraftList.h"
#include "CItem_Manager.h"

#include "CPartObject.h"

CAbstractPlayer::CAbstractPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUnit{ pDevice, pContext }
{
}

CAbstractPlayer::CAbstractPlayer(const CAbstractPlayer& Prototype)
    : CUnit{ Prototype }
{
}

HRESULT CAbstractPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAbstractPlayer::Initialize(void* pArg)
{
    CABSTRACTPLAYER_DESC* pDesc = static_cast<CABSTRACTPLAYER_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_pEquipment = CEquipment::Create(pDesc->eItemType);    
    m_pCraftList = CCraftList::Create();

    return S_OK;
}

void CAbstractPlayer::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CAbstractPlayer::Parallel_Update(_float fTimeDelta)
{
    __super::Parallel_Update(fTimeDelta);

}

void CAbstractPlayer::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CAbstractPlayer::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CAbstractPlayer::Render()
{
    return S_OK;
}

void CAbstractPlayer::Try_Craft(_uint iItemId)
{
    ACTION_COMMAND tAction_Command{};
    tAction_Command.eCommandType = ACTION_COMMAND_TYPE::CRAFT;
    tAction_Command.Data_UInt.iItemIdx = iItemId;

    Process_ActionCommand(tAction_Command);
}

_bool CAbstractPlayer::Use_Inventory(_uint iSlotIndex)
{
    _int iItemId = m_pInvetory->FindItemIdBySlotIndex(iSlotIndex);

    if (iItemId == -1)
    {
        return false;
    }

    const ITEM_DESC* pItemDesc = CItem_Manager::GetInstance()->Find_ItemInfo(iItemId);
    if (pItemDesc == nullptr)
    {
        MSG_BOX("No ItemInfo In CItem_Manger: CCraftList");
        return false;
    }

    if (pItemDesc->eType == ITEM_TYPE::FOOD)
    {
        return Use_Consumable(iSlotIndex);
    }
    else
    {
        return Equip(iSlotIndex);
    }
}

_bool CAbstractPlayer::Unequip(_uint iSlotIndex)
{
    _int iItemId = m_pEquipment->Find_ItemIdBySlotIndex(iSlotIndex);

    if (iItemId == -1)
        return false;

    _bool bResult = m_pInvetory->Add_Item(iItemId);
    if (bResult == false)
    {
        return false;
    }

    _int iDummy{};
    m_pEquipment->Unequip_ItemBySlotIndex(iSlotIndex, iDummy);

    m_pCraftList->Sync_CraftList(m_pInvetory, m_pEquipment);
    return true;
}

_bool CAbstractPlayer::Craft_Item(_int iItemId)
{
    return m_pCraftList->Craft_Item(iItemId, this);
}

_bool CAbstractPlayer::TryEquip_AddInven(_int iItemId, _uint iItemCount)
{
    if (iItemCount == 1)
    {
        // 일단 장착 시도
        _int iPreItemIndex = { -1 };
        _bool bEquipResult = m_pEquipment->Equip_HigherItemByItemId(iItemId, iPreItemIndex);

        // 장착 성공한 경우
        if (bEquipResult == true)
        {
            // 이전 아이템이 없으면
            if (iPreItemIndex == -1)
            {
                m_pCraftList->Sync_CraftList(m_pInvetory, m_pEquipment);
                return true;
            }

            // 이전 아이템이 있으면

            // 인벤토리에 Add
            _bool bAddInvenResult = m_pInvetory->Add_Item(iPreItemIndex, 1);

            // Add 성공했으면
            if(bAddInvenResult == true)
            {
                m_pCraftList->Sync_CraftList(m_pInvetory, m_pEquipment);
                return true;
            }

            // 인벤토리 Add 실패 했으면
 
            // 다시 원상 복구
            _int iDummy = { -1 };
            m_pEquipment->Equip_ItemByItemId(iPreItemIndex, iDummy);

            return false;
        }
    }

    // 개수가 1개가 아니거나, 장착 실패하면

    _bool bResult = m_pInvetory->Add_Item(iItemId, iItemCount);

    if (bResult == true)
    {
        m_pCraftList->Sync_CraftList(m_pInvetory, m_pEquipment);
    }

    return bResult;
}

COOL_INFO* CAbstractPlayer::Get_CoolInfo(const SKILL_SLOT eType)
{
    return nullptr;
}

_bool CAbstractPlayer::CanUseSkill(const SKILL_SLOT eType)
{
    return _bool();
}

void CAbstractPlayer::Get_SkillSlotType(const SKILL_SLOT eType, SKILL_DESC& tDesc)
{
}

HRESULT CAbstractPlayer::Initialize_Skill()
{
    return S_OK;
}

HRESULT CAbstractPlayer::Initialize_State()
{
    return S_OK;
}

_bool CAbstractPlayer::Equip(_uint iSlotIndex)
{
    _int iItemId = m_pInvetory->FindItemIdBySlotIndex(iSlotIndex);

    if (iItemId == -1)
    {
        return false;
    }

    _int iPreItemId = { -1 };
    _bool bResult = m_pEquipment->Equip_ItemByItemId(iItemId, iPreItemId);

    if (bResult == false)
    {
        return false;
    }

    m_pInvetory->Subtract_ItemBySlotIndex(iSlotIndex);

    if (iPreItemId != -1)
    {
        m_pInvetory->Add_Item(iPreItemId);
    }

    m_pCraftList->Sync_CraftList(m_pInvetory, m_pEquipment);
    return true;
}

_bool CAbstractPlayer::Use_Consumable(_uint iSlotIndex)
{
    _int iItemId = m_pInvetory->FindItemIdBySlotIndex(iSlotIndex);

    if (iItemId == -1)
    {
        return false;
    }

    const ITEM_DESC* pItemDesc = CItem_Manager::GetInstance()->Find_ItemInfo(iItemId);
    if (pItemDesc == nullptr)
    {
        MSG_BOX("No ItemInfo In CItem_Manger: CCraftList");
        return false;
    }

    Add_Recovery(pItemDesc->iConsumableHP);

    m_pInvetory->Subtract_ItemBySlotIndex(iSlotIndex, 1);
    m_pCraftList->Sync_CraftList(m_pInvetory, m_pEquipment);

    return true;
}

void CAbstractPlayer::Free()
{
    Safe_Release(m_pCraftList);
    Safe_Release(m_pEquipment);

    __super::Free();
}