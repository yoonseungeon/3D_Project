#include "CLiDailinAttack.h"

#include "CGameInstance.h"
#include "CPlayer.h"
#include "CBody_Player.h"

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
	const COMMAND& tCommand = pPlayer->Get_CurCommand();
	const CMyModel* pModel = pPlayer->Get_BodyPlayer()->Get_ModelCom();

	// 몬스터 생기면 수정 필요
	if (tCommand.eCommandType == COMMAND_TYPE::ATTACK && pModel->Get_CurAniPlayRatio() >= 0.75f)
	{
		pPlayer->Set_WaitState(L"CLiDailinAttack");
	}

	if (pPlayer->IsAnimationFinished(L"Body")) {

		if (tCommand.eCommandType == COMMAND_TYPE::MOVE)
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
			if (pPlayer->CanUseQ() == false) {
				return;
			}

			pPlayer->Set_CurCommand(eCommand);
			pPlayer->Set_WaitState(L"CLiDailin_Q");
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

CLiDailinAttack* CLiDailinAttack::Create()
{
	return new CLiDailinAttack;
}

void CLiDailinAttack::Free()
{
	__super::Free();
}
