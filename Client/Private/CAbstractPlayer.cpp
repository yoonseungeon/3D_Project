#include "CAbstractPlayer.h"

#include "CGameInstance.h"

#include "CInventory.h"
#include "CEquipment.h"
#include "CCraftList.h"
#include "CItem_Manager.h"

#include "CPartObject.h"
#include "CCraftTool.h"
#include "CCraftHammer.h"
#include "CBurner.h"
#include "CFryingPan.h"

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

    m_fSoundLoopGap = 50.f;

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

    Check_CurArea();
    Update_GroundSound(fTimeDelta);
    Update_AreaVoiceTimer(fTimeDelta);
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
        m_pGameInstance->PlaySound_Once(ETOUI(SOUND_KEY::CONSUME_EAT_3));
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
        m_pGameInstance->PlaySound_Once(ETOUI(SOUND_KEY::ITEM_FAIL));
        return false;
    }

    _int iDummy{};
    m_pEquipment->Unequip_ItemBySlotIndex(iSlotIndex, iDummy);

    SetItemStat(iItemId, false);

    m_pCraftList->Sync_CraftList(m_pInvetory, m_pEquipment);
    return true;
}

_bool CAbstractPlayer::Craft_Item(_int iItemId)
{
    return m_pCraftList->Craft_Item(iItemId, this);
}

_bool CAbstractPlayer::TryEquip_AddInven(_int iItemId, _uint iItemCount, _bool bSoundOnOff)
{
    if (iItemCount == 1)
    {
        // 일단 장착 시도
        _int iPreItemIndex = { -1 };
        _bool bEquipResult = m_pEquipment->Equip_HigherItemByItemId(iItemId, iPreItemIndex);

        // 장착 성공한 경우
        if (bEquipResult == true)
        {
            SetItemStat(iItemId, true);

            // 이전 아이템이 없으면
            if (iPreItemIndex == -1)
            {
                if (bSoundOnOff == true)
                    m_pGameInstance->PlaySound_Once(ETOUI(SOUND_KEY::ITEM_PICKUP));

                m_pCraftList->Sync_CraftList(m_pInvetory, m_pEquipment);
                return true;
            }

            // 이전 아이템이 있으면

            // 인벤토리에 Add
            _bool bAddInvenResult = m_pInvetory->Add_Item(iPreItemIndex, 1);

            // Add 성공했으면
            if(bAddInvenResult == true)
            {
                if (bSoundOnOff == true)
                    m_pGameInstance->PlaySound_Once(ETOUI(SOUND_KEY::ITEM_PICKUP));

                SetItemStat(iPreItemIndex, false);

                m_pCraftList->Sync_CraftList(m_pInvetory, m_pEquipment);
                return true;
            }

            // 인벤토리 Add 실패 했으면
 
            if (bSoundOnOff == true)
                m_pGameInstance->PlaySound_Once(ETOUI(SOUND_KEY::ITEM_FAIL));

            // 다시 원상 복구
            _int iDummy = { -1 };
            m_pEquipment->Equip_ItemByItemId(iPreItemIndex, iDummy);
            SetItemStat(iItemId, false);

            return false;
        }
    }

    // 개수가 1개가 아니거나, 장착 실패하면

    _bool bResult = m_pInvetory->Add_Item(iItemId, iItemCount);

    if (bResult == true)
    {
        if (bSoundOnOff == true)
            m_pGameInstance->PlaySound_Once(ETOUI(SOUND_KEY::ITEM_PICKUP));

        m_pCraftList->Sync_CraftList(m_pInvetory, m_pEquipment);
    }
    else
    {
        if (bSoundOnOff == true)
            m_pGameInstance->PlaySound_Once(ETOUI(SOUND_KEY::ITEM_FAIL));
    }

    return bResult;
}

COOL_INFO* CAbstractPlayer::Get_CoolInfo(const SKILL_SLOT eType)
{
    return nullptr;
}

_bool CAbstractPlayer::CanUseSkill(const SKILL_SLOT eType)
{
    return false;
}

void CAbstractPlayer::Get_SkillSlotType(const SKILL_SLOT eType, SKILL_DESC& tDesc)
{
}

void CAbstractPlayer::Look_MouseDir()
{
    _float4 vRayPos{}, vRayDir{};
    CGameInstance::GetInstance()->Get_WorldRay(vRayPos, vRayDir);

    _vector vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);
    _float fPlayerY = XMVectorGetY(vPlayerPos);

    // f(t) = vRayDir * t + vRayPos
    // 플레이어와 높이가 같으려면(y가 같을 때) 얼만큼(t) 가야 하나
    // y 도달은 y 방향이 결정
    // fPlayerY = vRayDir.y * t + vRayPos.y
    //t = (fPlayerY - vRayPos.y) / vRayDir.y
    if (vRayDir.y == 0.f)
        return;

    _float fTime = (fPlayerY - vRayPos.y) / vRayDir.y;

    // 플레이어와 높이가 같을 때 x, y 값
    _float4 vMouseWorld{ 0.f, 0.f, 0.f, 1.f };
    vMouseWorld.x = vRayDir.x * fTime + vRayPos.x;
    vMouseWorld.y = fPlayerY;
    vMouseWorld.z = vRayDir.z * fTime + vRayPos.z;

    _vector vDir = XMVector3Normalize(XMLoadFloat4(&vMouseWorld) - vPlayerPos);
    m_pTransformCom->LookDir(vDir);
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
        m_pGameInstance->PlaySound_Once(ETOUI(SOUND_KEY::ITEM_FAIL));
        return false;
    }

    // 장착 성공한 경우
    SetItemStat(iItemId, true);

    m_pInvetory->Subtract_ItemBySlotIndex(iSlotIndex);

    // 이전 아이템이 있는 경우
    if (iPreItemId != -1)
    {
        m_pInvetory->Add_Item(iPreItemId);
        SetItemStat(iPreItemId, false);
    }

    m_pGameInstance->PlaySound_Once(ETOUI(SOUND_KEY::EQUIP_ITEM));

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

    Add_Recovery(pItemDesc->iConsumableHP, 0);

    m_pInvetory->Subtract_ItemBySlotIndex(iSlotIndex, 1);
    m_pCraftList->Sync_CraftList(m_pInvetory, m_pEquipment);

    return true;
}

void CAbstractPlayer::SetItemStat(_int iItemId, _bool bAdd)
{
    if (iItemId == -1)
    {
        return;
    }

    const ITEM_DESC* pItemDesc = CItem_Manager::GetInstance()->Find_ItemInfo(iItemId);
    if (pItemDesc == nullptr)
    {
        MSG_BOX("No ItemInfo In CItem_Manger: CCraftList");
        return;
    }

    UNIT_STAT tAddStat{};

    tAddStat.iLevel = 0;
    tAddStat.iEXP = 0;

    tAddStat.iHP = pItemDesc->iHP;
    tAddStat.iMP = 0;

    tAddStat.fHPRecoveryP = pItemDesc->fHPRecoveryPercent;
    tAddStat.fMPRecoveryP = 0.f;

    tAddStat.iATKPower = pItemDesc->iATKPower;
    tAddStat.iSkillAmp = 0;
    tAddStat.iPenetrationDefense = pItemDesc->iPenetrationDefense;
    tAddStat.iPenetrationDefensePercent = 0;

    tAddStat.iDefense = pItemDesc->iDefense;
    tAddStat.fATKSpeed = pItemDesc->fATKSpeed;
    tAddStat.iCoolDown = 0;
    tAddStat.iCritical = pItemDesc->iCritical;
    tAddStat.fSpeed = pItemDesc->fSpeed;

    AddStat(m_tItemStat, tAddStat, bAdd);

    SetFinalStat();
}

void CAbstractPlayer::Check_CurArea()
{
    if (m_bAreaChange == true)
        m_bAreaChange = false;

    if (m_pNavigationCom == nullptr)
        return;

    _int iCurAreaIndex = m_pNavigationCom->Get_CurAreaIndex();

    if (m_iCurAreaIndex != iCurAreaIndex)
    {
        m_bAreaVoiceCoolIgnore = true;
        m_iCurAreaIndex = iCurAreaIndex;
        m_bAreaChange = true;
    }
}

void CAbstractPlayer::Update_GroundSound(_float fTimeDelta)
{
    if (m_bAreaChange) {
        
        m_fAccSoundLoopTime = 0.f;

        SOUND_KEY eSoundKey{};
        Choose_Area(eSoundKey);

        CGameInstance::GetInstance()->PlaySound_OnceFixed(ETOUI(eSoundKey), ETOUI(SOUND_CHANNEL_GAMEPLAY::BGM), 0.1f);
    }

    m_fAccSoundLoopTime += fTimeDelta;
    if (m_fAccSoundLoopTime >= m_fSoundLoopGap)
    {
        m_fAccSoundLoopTime =0.f;

        SOUND_KEY eSoundKey{};
        Choose_Area(eSoundKey);

        CGameInstance::GetInstance()->PlaySound_OnceFixed(ETOUI(eSoundKey), ETOUI(SOUND_CHANNEL_GAMEPLAY::BGM), 0.1f);
    }
}

void CAbstractPlayer::Choose_Area(SOUND_KEY& eSoundKey)
{
    switch (static_cast<AREA_INDEX>(m_iCurAreaIndex)) {
    case AREA_INDEX::ALLEY:
    case AREA_INDEX::GAS_STATION:
        eSoundKey = SOUND_KEY::BGM_ALLEY;
        break;

    case AREA_INDEX::ARCHERY:
        eSoundKey = SOUND_KEY::BGM_ARCHERY;
        break;

    case AREA_INDEX::CEMETERY:
        eSoundKey = SOUND_KEY::BGM_CEMETERY;
        break;

    case AREA_INDEX::CHURCH:
        eSoundKey = SOUND_KEY::BGM_CHURCH;
        break;

    case AREA_INDEX::FACTORY:
        eSoundKey = SOUND_KEY::BGM_FACTORY;
        break;

    case AREA_INDEX::FIRE_STATION:
    case AREA_INDEX::POLICE_STATION:
        eSoundKey = SOUND_KEY::BGM_DOWNTOWN;
        break;

    case AREA_INDEX::FOREST:
        eSoundKey = SOUND_KEY::BGM_FOREST;
        break;

    case AREA_INDEX::HARBOR:
    case AREA_INDEX::WAREHOUSE:
        eSoundKey = SOUND_KEY::BGM_HARBOR;
        break;

    case AREA_INDEX::HOSPITAL:
        eSoundKey = SOUND_KEY::BGM_HOSPITAL;
        break;

    case AREA_INDEX::HOTEL:
        eSoundKey = SOUND_KEY::BGM_HOTEL;
        break;

    case AREA_INDEX::LABORATORY:
        eSoundKey = SOUND_KEY::BGM_LABORATORY;
        break;

    case AREA_INDEX::POND:
    case AREA_INDEX::STREAM:
        eSoundKey = SOUND_KEY::BGM_POND;
        break;

    case AREA_INDEX::SANDY_BEACH:
        eSoundKey = SOUND_KEY::BGM_SANDY_BEACH;
        break;

    case AREA_INDEX::SCHOOL:
        eSoundKey = SOUND_KEY::BGM_SCHOOL;
        break;

    case AREA_INDEX::TEMPLE:
        eSoundKey = SOUND_KEY::BGM_TEMPLE;
        break;

    case AREA_INDEX::UPTOWN:
        eSoundKey = SOUND_KEY::BGM_UPTOWN;
        break;
    }
}

void CAbstractPlayer::Update_AreaVoiceTimer(_float fTimeDelta)
{
    if (m_bCanPlayAreaVoiceTime == true)
        return;

    m_fAccAreaVoiceTime += fTimeDelta;

    const _float fCanPlayAreaVoiceTime = 35.f;
    if(m_fAccAreaVoiceTime >= fCanPlayAreaVoiceTime)
    {
        m_fAccAreaVoiceTime = 0.f;

        m_bCanPlayAreaVoiceTime = true;
    }
}

void CAbstractPlayer::PlayAreaVoice()
{
}

void CAbstractPlayer::Reset_AreaVoiceTimer()
{
    m_fAccAreaVoiceTime = 0.f;
    m_bCanPlayAreaVoiceTime = false;
}

void CAbstractPlayer::Free()
{
    Safe_Release(m_pNavigationCom);

    Safe_Release(m_pBurner);
    Safe_Release(m_pFryingPan);
    Safe_Release(m_pCraftTool);
    Safe_Release(m_pCraftHammer);

    Safe_Release(m_pCraftList);
    Safe_Release(m_pEquipment);

    __super::Free();
}