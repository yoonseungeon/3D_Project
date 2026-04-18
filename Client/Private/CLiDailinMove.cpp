#include "CLiDailinMove.h"

#include "CPlayer.h"

CLiDailinMove::CLiDailinMove()
{
}

void CLiDailinMove::Enter(CPlayer* pPlayer)
{
	// Ani
	pPlayer->Set_Animation(L"Body", static_cast<_uint>(LiDailin_Ani::Ani_Run), true);
	pPlayer->Set_Animation(L"Weapon", static_cast<_uint>(Nunchaku_Ani::RUN_WP), true);

	// ÀÌµ¿
	_float3 vTargetPos = pPlayer->Get_CurCommand().vTargetPos;
	pPlayer->Move_To_Pos(vTargetPos, true);
}

void CLiDailinMove::Update(CPlayer* pPlayer, _float fTimeDelta)
{
	if (pPlayer->IsTargetInRange()) {
		pPlayer->Set_WaitState(L"CLiDailinAttack");
	}
	else if (pPlayer->Update_Move_To_Pos(fTimeDelta)) {
		pPlayer->Set_WaitState(L"Idle");
	}
}

void CLiDailinMove::Exit(CPlayer* pPlayer)
{
	//pPlayer->Stop_Move_To_Pos();
}

void CLiDailinMove::HandleCommand(CPlayer* pPlayer, COMMAND& eCommand)
{
	switch (eCommand.eCommandType) {
		case COMMAND_TYPE::MOVE:
		{
			pPlayer->Set_CurCommand(eCommand);
			pPlayer->Set_WaitState(L"Move");
			break;
		}
		case COMMAND_TYPE::ATTACK:
		{
			if (pPlayer->IsTargetInRange())
			{
				pPlayer->Set_WaitState(L"CLiDailinAttack");
			}
			else
			{
				pPlayer->Set_WaitState(L"Move");
			}
			pPlayer->Set_CurCommand(eCommand);

			break;
		}
		case COMMAND_TYPE::ATTACK_Q:
		{
			if (pPlayer->CanUseQ() == false) {
				return;
			}

			pPlayer->Set_CurCommand(eCommand);
			pPlayer->Set_WaitState(L"CLiDailin_Q");
			break;
		}
		case COMMAND_TYPE::ATTACK_W:
		{
			if (pPlayer->CanUseSkill(L"W") == false) {
				return;
			}

			pPlayer->Set_CurCommand(eCommand);
			pPlayer->Set_WaitState(L"CLiDailin_W");
			break;
		}
		case COMMAND_TYPE::ATTACK_E:
		{
			if (pPlayer->CanUseSkill(L"E") == false) {
				return;
			}

			pPlayer->Set_CurCommand(eCommand);
			pPlayer->Set_WaitState(L"CLiDailin_E");
			break;
		}
		case COMMAND_TYPE::ATTACK_R:
		{
			if (pPlayer->CanUseSkill(L"R") == false) {
				return;
			}

			pPlayer->Set_CurCommand(eCommand);
			pPlayer->Set_WaitState(L"CLiDailin_R");
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
