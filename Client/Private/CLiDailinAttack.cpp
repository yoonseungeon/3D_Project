#include "CLiDailinAttack.h"

#include "CPlayer.h"

CLiDailinAttack::CLiDailinAttack()
{
}

void CLiDailinAttack::Enter(CPlayer* pPlayer)
{
	// Ani
	if (rand() % 2 == 0) {
		pPlayer->Set_Animation(L"Body", static_cast<_uint>(LiDailin_Ani::Ani_ATK_1), false);
		pPlayer->Set_Animation(L"Weapon", static_cast<_uint>(Nunchaku_Ani::ATK_1_WP), false);
	}
	else
	{
		pPlayer->Set_Animation(L"Body", static_cast<_uint>(LiDailin_Ani::Ani_ATK_2), false);
		pPlayer->Set_Animation(L"Weapon", static_cast<_uint>(Nunchaku_Ani::ATK_2_WP), false);
	}

}

void CLiDailinAttack::Update(CPlayer* pPlayer, _float fTimeDelta)
{
	if (pPlayer->IsAnimationFinished(L"Body")) {
		const COMMAND& tCommand = pPlayer->Get_CurCommand();

		// 몬스터 생기면 수정 필요
		if (tCommand.eCommandType == COMMAND_TYPE::ATTACK)
		{
			pPlayer->Set_WaitState(L"CLiDailinAttack");
		}
		else if (tCommand.eCommandType == COMMAND_TYPE::MOVE)
		{
			pPlayer->Set_WaitState(L"Move");
		}
		else
		{
			pPlayer->Set_WaitState(L"Idle");
		}
	}
}

void CLiDailinAttack::Exit(CPlayer* pPlayer)
{
}

void CLiDailinAttack::HandleCommand(CPlayer* pPlayer, COMMAND& eCommand)
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
			pPlayer->Set_CurCommand(eCommand);
			pPlayer->Set_WaitState(L"CLiDailin_Q");
			break;
		}
	}
}

CLiDailinAttack* CLiDailinAttack::Create()
{
	return new CLiDailinAttack;
}

void CLiDailinAttack::Free()
{
	__super::Free();
}
