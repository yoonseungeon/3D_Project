#include "CLiDailinIdle.h"

#include "CPlayer.h"

CLiDailinIdle::CLiDailinIdle()
{
}

void CLiDailinIdle::Enter(CPlayer* pPlayer)
{
	pPlayer->Set_Animation(L"Body", static_cast<_uint>(LiDailin_Ani::Ani_Idle), true);
	pPlayer->Set_Animation(L"Weapon", static_cast<_uint>(Nunchaku_Ani::IDLE_WP), true);
}

void CLiDailinIdle::Update(CPlayer* pPlayer, _float fTimeDelta)
{
}

void CLiDailinIdle::Exit(CPlayer* pPlayer)
{

}

void CLiDailinIdle::HandleCommand(CPlayer* pPlayer, COMMAND& eCommand)
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
