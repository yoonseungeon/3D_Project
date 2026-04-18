#include "CLiDailin_W.h"

#include "CPlayer.h"
#include "CBody_Player.h"
#include "CGameInstance.h"

CLiDailin_W::CLiDailin_W()
{
}

void CLiDailin_W::Enter(CPlayer* pPlayer)
{
	// Cool
	//COOL_INFO* pECoolInfo = pPlayer->Get_CoolInfo(L"E");
	//pECoolInfo->bCoolWait = true;
	//pECoolInfo->fAccCoolDown = pECoolInfo->fCurCoolDown;

	// Ani
	pPlayer->Set_Animation(L"Body", static_cast<_uint>(LiDailin_Ani::Ani_W2), false);
	pPlayer->Set_Animation(L"Weapon", static_cast<_uint>(Nunchaku_Ani::IDLE_WP), false);

	// Ani Speed


	// ÀÌµ¿
	//pPlayer->Stop_Move_To_Pos();
}

void CLiDailin_W::Update(CPlayer* pPlayer, _float fTimeDelta)
{
	pPlayer->Update_Move_To_Pos(fTimeDelta);

	if (pPlayer->IsAnimationFinished(L"Body")) {
		if (pPlayer->Get_CurCommand().eCommandType == COMMAND_TYPE::MOVE) {
			pPlayer->Set_WaitState(L"Move");
		}
		else
		{
			pPlayer->Set_WaitState(L"Idle");
		}
	}
}

void CLiDailin_W::Exit(CPlayer* pPlayer)
{
}

void CLiDailin_W::HandleCommand(CPlayer* pPlayer, COMMAND& eCommand)
{
	switch (eCommand.eCommandType) {
	case COMMAND_TYPE::MOVE:
	{
		break;
	}
	case COMMAND_TYPE::ATTACK:
	{
		break;
	}
	case COMMAND_TYPE::ATTACK_Q:
	{
		break;
	}
	case COMMAND_TYPE::ATTACK_W:
	{
		break;
	}
	case COMMAND_TYPE::ATTACK_E:
	{
		break;
	}
	case COMMAND_TYPE::ATTACK_R:
	{
		break;
	}
	}
}

CLiDailin_W* CLiDailin_W::Create()
{
	return new CLiDailin_W;
}

void CLiDailin_W::Free()
{
	__super::Free();
}
