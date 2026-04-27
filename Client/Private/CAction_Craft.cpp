#include "CAction_Craft.h"

#include "CLiDailin.h"
#include "CBody_Player.h"
#include "CWeapon.h"
#include "CGameInstance.h"

CAction_Craft::CAction_Craft()
{
}

void CAction_Craft::Enter(CLiDailin* pPlayer)
{
    // Ani
    pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(LiDailin_Ani::Ani_Craft), false);
    pPlayer->Get_Weapon()->Set_IsInactive(true);
    pPlayer->Set_MovementAniBlock(true);

    // Ani Speed

    // 이동
    pPlayer->Set_MoveBlock(true);
    pPlayer->Set_WaitMovementState(L"Idle");
}

void CAction_Craft::Update(CLiDailin* pPlayer, _float fTimeDelta)
{

}

void CAction_Craft::Exit(CLiDailin* pPlayer)
{
    // Ani
    pPlayer->Set_CurAni(LiDailin_Ani::Ani_None);
    pPlayer->Get_Weapon()->Set_IsInactive(false);
    pPlayer->Set_MovementAniBlock(false);

    // 이동
    pPlayer->Set_MoveBlock(false);
}

void CAction_Craft::HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command)
{
 
}

CAction_Craft* CAction_Craft::Create()
{
    return new CAction_Craft;
}

void CAction_Craft::Free()
{
    __super::Free();
}
