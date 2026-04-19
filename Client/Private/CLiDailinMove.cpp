#include "CLiDailinMove.h"

#include "CPlayer.h"
#include "CBody_Player.h"
#include "CWeapon.h"
#include "CGameInstance.h"

CLiDailinMove::CLiDailinMove()
{
}

void CLiDailinMove::Enter(CPlayer* pPlayer)
{
	// Ani
	if (pPlayer->Get_AniBlock() == false)
	{
		pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(LiDailin_Ani::Ani_Run), true);
		pPlayer->Set_CurAni(LiDailin_Ani::Ani_Run);
		pPlayer->Get_Weapon()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(Nunchaku_Ani::RUN_WP), true);
	}

	// 이동
	//if (pPlayer->Get_MoveBlock() == false)
	//{
		_float3 vTargetPos = pPlayer->Get_CurMovementCommand().vTargetPos;
		pPlayer->Get_MoveCom()->Move_To_Pos(vTargetPos, true);
	//}
}

void CLiDailinMove::Update(CPlayer* pPlayer, _float fTimeDelta)
{
	if (pPlayer->Get_MoveBlock() == true)
	{
		return;
	}

	const LiDailin_Ani& eCurAni = pPlayer->Get_CurAni();
	if (pPlayer->Get_AniBlock() == false && eCurAni == LiDailin_Ani::Ani_None)
	{
		pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(LiDailin_Ani::Ani_Run), true);
		pPlayer->Set_CurAni(LiDailin_Ani::Ani_Run);
		pPlayer->Get_Weapon()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(Nunchaku_Ani::RUN_WP), true);
	}

	// 도착하면
	if (pPlayer->Get_MoveCom()->Update_Move_To_Pos(fTimeDelta))
	{
		pPlayer->Set_WaitMovementState(L"Idle");
	}
}

void CLiDailinMove::Exit(CPlayer* pPlayer)
{
	if (pPlayer->Get_MoveBlock() == true) {
		return;
	}
}

void CLiDailinMove::HandleMovementCommand(CPlayer* pPlayer, MOVEMENT_COMMAND& eMovement_Command)
{
	switch (eMovement_Command.eCommandType) {
		case MOVEMENT_COMMAND_TYPE::MOVE:
		{
			if (pPlayer->Get_MoveBlock() == true && pPlayer->Get_CanMoveCancle() == false) {
				return;
			}

			pPlayer->Set_CurMovementCommand(eMovement_Command);
			pPlayer->Set_WaitMovementState(L"Move");
			break;
		}
	}
}

CLiDailinMove* CLiDailinMove::Create()
{
	return new CLiDailinMove;
}

void CLiDailinMove::Free()
{
	__super::Free();
}
