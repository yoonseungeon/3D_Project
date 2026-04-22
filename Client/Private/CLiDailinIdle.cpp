#include "CLiDailinIdle.h"

#include "CLiDailin.h"
#include "CBody_Player.h"
#include "CWeapon.h"
#include "CGameInstance.h"

CLiDailinIdle::CLiDailinIdle()
{
}

void CLiDailinIdle::Enter(CLiDailin* pPlayer)
{
	// Ani
	if (pPlayer->Get_AniBlock() == false)
	{
		pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(LiDailin_Ani::Ani_Idle), true);
		pPlayer->Set_CurAni(LiDailin_Ani::Ani_Idle);
		pPlayer->Get_Weapon()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(Nunchaku_Ani::IDLE_WP), true);
	}

	// ÀÌµ¿
	pPlayer->Get_MoveCom()->Stop_Move_To_Pos();
}

void CLiDailinIdle::Update(CLiDailin* pPlayer, _float fTimeDelta)
{
	const LiDailin_Ani& eCurAni = pPlayer->Get_CurAni();
	if (pPlayer->Get_AniBlock() == false && eCurAni == LiDailin_Ani::Ani_None)
	{
		pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(LiDailin_Ani::Ani_Idle), true);
		pPlayer->Set_CurAni(LiDailin_Ani::Ani_Idle);
		pPlayer->Get_Weapon()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(Nunchaku_Ani::IDLE_WP), true);
	}
}

void CLiDailinIdle::Exit(CLiDailin* pPlayer)
{
}

void CLiDailinIdle::HandleMovementCommand(CLiDailin* pPlayer, MOVEMENT_COMMAND& eMovement_Command)
{
	switch (eMovement_Command.eCommandType) {
		case MOVEMENT_COMMAND_TYPE::MOVE:
		{
			pPlayer->Set_CurMovementCommand(eMovement_Command);
			pPlayer->Set_WaitMovementState(L"Move");
			break;
		}	
	}
}

CLiDailinIdle* CLiDailinIdle::Create()
{
	return new CLiDailinIdle;
}

void CLiDailinIdle::Free()
{
	__super::Free();
}
