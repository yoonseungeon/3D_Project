#include "CAbstractPlayer.h"

#include "CGameInstance.h"
#include "CInventory.h"

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
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

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

void CAbstractPlayer::Try_Craft(_uint iItemIndex)
{
    ACTION_COMMAND tAction_Command{};
    tAction_Command.eCommandType = ACTION_COMMAND_TYPE::CRAFT;
    tAction_Command.Data_UInt.iItemIdx = iItemIndex;

    Process_ActionCommand(tAction_Command);
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
    __super::Free();
}