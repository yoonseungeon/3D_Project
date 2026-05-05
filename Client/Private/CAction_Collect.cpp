#include "CAction_Collect.h"

#include "CGameInstance.h"

#include "CItem_Manager.h"

#include "CItemBox_Collectible.h"
#include "CLiDailin.h"

#include "CBody_Player.h"
#include "CWeapon.h"

CAction_Collect::CAction_Collect()
{
}

HRESULT CAction_Collect::Initialize(_uint iCollectAni)
{
    m_iCollectAni = iCollectAni;
    return S_OK;
}

void CAction_Collect::Enter(CLiDailin* pPlayer)
{
    pPlayer->Set_CanMoveCancle(true);

    Move_OR_Interact(pPlayer);
}

void CAction_Collect::Update(CLiDailin* pPlayer, _float fTimeDelta)
{
    m_fAccTime += fTimeDelta;

    if (m_bIsInteract == true)
    {
        if(pPlayer->Get_BodyPlayer()->Get_ModelCom()->IsAnimationFinished() == true
            || m_fAccTime >= m_fMaxTime)
        {
            pPlayer->TryEquip_AddInven(m_iItemId);
            pPlayer->Set_ActionEnd();
        }

        return;
    }

    Move_OR_Interact(pPlayer);

}

void CAction_Collect::Exit(CLiDailin* pPlayer)
{
    m_bIsInteract = false;

    m_iItemId = -1;
    m_fAccTime = 0.f;

    pPlayer->Set_CurAni(LiDailin_Ani::Ani_None);
    pPlayer->Get_Weapon()->Set_IsInactive(false);
    pPlayer->Set_MovementAniBlock(false);

    pPlayer->Set_CanMoveCancle(false);
}

void CAction_Collect::HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command)
{
    pPlayer->DefaultProcess_ActionCommand(eAction_Command);
}

void CAction_Collect::MoveToPos(CLiDailin* pPlayer, _float3& vPos)
{
    MOVEMENT_COMMAND tMovement_Command{};
    tMovement_Command.eCommandType = MOVEMENT_COMMAND_TYPE::MOVE;
    tMovement_Command.vTargetPos = vPos;

    pPlayer->Process_MovementCommand(tMovement_Command);
}

void CAction_Collect::Move_OR_Interact(CLiDailin* pPlayer)
{
    const ACTION_COMMAND& tAction_Command = pPlayer->Get_CurActionCommand();

    CItemBox_Collectible* pCollectible = dynamic_cast<CItemBox_Collectible*>(tAction_Command.pGameObject);
    if (pCollectible == nullptr)
        MSG_BOX("Bug Point 1: CAction_Collect");

    m_iItemId = pCollectible->Get_ItemId();
    const ITEM_DESC* pItemDesc = CItem_Manager::GetInstance()->Find_ItemInfo(m_iItemId);
    m_fMaxTime = CItem_Manager::GetInstance()->Get_CurItemCollectTime(m_iItemId);

    CTransform* pTransform = dynamic_cast<CTransform*>(pCollectible->Find_Component(g_strTransformTag));
    if (pTransform == nullptr)
        MSG_BOX("Bug Point 2: CAction_Collect");

    _vector vMyPos = pPlayer->Get_TransformCom()->Get_State(STATE::POSITION);

    const _float fDistance = 0.3f;
    if (pCollectible->IsInOpenRange(vMyPos, fDistance) == false)
    {
        _vector vTargetPos = pTransform->Get_State(STATE::POSITION);
        _float3 vPos{};
        XMStoreFloat3(&vPos, vTargetPos);
        MoveToPos(pPlayer, vPos);
    }
    else
    {
        pPlayer->Set_WaitMovementState(L"Idle");
        pPlayer->Set_MovementAniBlock(true);

        pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(m_iCollectAni, false);
        pPlayer->Get_Weapon()->Set_IsInactive(true);

        m_bIsInteract = true;
    }
}

CAction_Collect* CAction_Collect::Create(_uint iCollectAni)
{
    CAction_Collect* pInstance = new CAction_Collect();

    if (FAILED(pInstance->Initialize(iCollectAni)))
    {
        MSG_BOX("Failed to Created: CAction_Collect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAction_Collect::Free()
{
    __super::Free();
}
