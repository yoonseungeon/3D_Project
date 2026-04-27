#include "CAction_Craft.h"

#include "CLiDailin.h"
#include "CBody_Player.h"
#include "CWeapon.h"
#include "CGameInstance.h"

#include "CItem_Manager.h"
#include "CInventory.h"

CAction_Craft::CAction_Craft()
{
}

HRESULT CAction_Craft::Initialize(_uint iCurAni)
{
    m_iCurAni = iCurAni;

    return S_OK;
}

void CAction_Craft::Enter(CLiDailin* pPlayer)
{
    m_iItemIndex = pPlayer->Get_CurActionCommand().Data_UInt.iItemIdx;
    m_fMaxTime = CItem_Manager::GetInstance()->Get_CurItemCraftTime(m_iItemIndex);

    // Ani
    pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(m_iCurAni, false);
    pPlayer->Get_Weapon()->Set_IsInactive(true);
    pPlayer->Set_MovementAniBlock(true);

    // Ani Speed

    // 이동
    pPlayer->Set_MoveBlock(true);
    pPlayer->Set_WaitMovementState(L"Idle");

    pPlayer->Set_CanMoveCancle(true);
}

void CAction_Craft::Update(CLiDailin* pPlayer, _float fTimeDelta)
{
    m_fAccTime += fTimeDelta;

    if (pPlayer->Get_BodyPlayer()->Get_ModelCom()->IsAnimationFinished() == true
        || m_fAccTime >= m_fMaxTime)
    {
        pPlayer->Get_Inventory()->Craft_Item(m_iItemIndex);
        pPlayer->Set_ActionEnd();
    }
}

void CAction_Craft::Exit(CLiDailin* pPlayer)
{
    m_iItemIndex = -1;
    m_fAccTime = 0.f;

    // Ani
    pPlayer->Set_CurAni(LiDailin_Ani::Ani_None);
    pPlayer->Get_Weapon()->Set_IsInactive(false);
    pPlayer->Set_MovementAniBlock(false);

    // 이동
    pPlayer->Set_MoveBlock(false);

    pPlayer->Set_CanMoveCancle(false);
}

void CAction_Craft::HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command)
{
 
}

CAction_Craft* CAction_Craft::Create(_uint iCurAni)
{
    CAction_Craft* pInstance = new CAction_Craft();

    if (FAILED(pInstance->Initialize(iCurAni)))
    {
        MSG_BOX("Failed to Created: CAction_Craft");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAction_Craft::Free()
{
    __super::Free();
}
