#include "CAbstractPlayer.h"

#include "CGameInstance.h"
#include "CInventory.h"
#include "CEquipment.h"
#include "CCraftList.h"

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
    // PartObject들은 GameObject_Manager에 안 들어간다.
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Priority_Update(fTimeDelta);
    }
}

void CAbstractPlayer::Parallel_Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Parallel_Update(fTimeDelta);
    }
}

void CAbstractPlayer::Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Update(fTimeDelta);
    }
}

void CAbstractPlayer::Late_Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Late_Update(fTimeDelta);
    }
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
        _bool bEquipResult = m_pEquipment->Equip_ItemByItemId(iItemId, iPreItemIndex);

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

void CAbstractPlayer::Free()
{
    Safe_Release(m_pCraftList);
    Safe_Release(m_pEquipment);

    __super::Free();
}