#include "CLiDailinAttack.h"

#include "CPlayer.h"
#include "CBody_Player.h"
#include "CWeapon.h"
#include "CGameInstance.h"

CLiDailinAttack::CLiDailinAttack()
{
}

void CLiDailinAttack::Enter(CPlayer* pPlayer)
{	
	//if(pPlayer->Get_CurActionCommand().pGameObject와 거리 비교)
	if (CGameInstance::GetInstance()->Key_Pressing(DIK_S) /* 범위 안 이면 */) {
		Attack(pPlayer);
	}
	else /* 범위 밖이면 */
	{
		Chase(pPlayer);
	}
}

void CLiDailinAttack::Update(CPlayer* pPlayer, _float fTimeDelta)
{
	if (m_bChase == true)
	{
		//if(pPlayer->Get_CurActionCommand().pGameObject와 거리 비교)
		if (CGameInstance::GetInstance()->Key_Pressing(DIK_S) /* 범위 안 이면 */) {
			Attack(pPlayer);
		}
		else /* 범위 밖이면 */
		{
			Chase(pPlayer);
			return;
		}
	}

	const ACTION_COMMAND& tAction_Command = pPlayer->Get_CurActionCommand();
	const CMyModel* pModel = pPlayer->Get_BodyPlayer()->Get_ModelCom();

	// 몬스터가 살아있으면
	if (CGameInstance::GetInstance()->Key_Pressing(DIK_D) && pModel->Get_CurAniPlayRatio() >= 0.75f)
	{
		pPlayer->Set_WaitActionState(L"CLiDailinAttack");
	}

	if (pPlayer->Get_BodyPlayer()->Get_ModelCom()->IsAnimationFinished() == true) {
		pPlayer->Set_ActionEnd();
	}
}

void CLiDailinAttack::Exit(CPlayer* pPlayer)
{
	pPlayer->Set_CurAni(LiDailin_Ani::Ani_None);
	pPlayer->Set_AniBlock(false);
}

void CLiDailinAttack::HandleActionCommand(CPlayer* pPlayer, ACTION_COMMAND& eAction_Command)
{
	switch (eAction_Command.eCommandType) {
		case ACTION_COMMAND_TYPE::ATTACK:
		{
			pPlayer->Set_CurActionCommand(eAction_Command);
			pPlayer->Set_WaitActionState(L"CLiDailinAttack");

			break;
		}
		case ACTION_COMMAND_TYPE::ATTACK_Q:
		{
			if (pPlayer->CanUseSkill(L"Q") == false) {
				return;
			}

			pPlayer->Set_CurActionCommand(eAction_Command);
			pPlayer->Set_WaitActionState(L"CLiDailin_Q");

			break;
		}
		case ACTION_COMMAND_TYPE::ATTACK_W:
		{
			if (pPlayer->CanUseSkill(L"W") == false) {
				return;
			}

			pPlayer->Set_CurActionCommand(eAction_Command);
			pPlayer->Set_WaitActionState(L"CLiDailin_W");
			break;
		}
		case ACTION_COMMAND_TYPE::ATTACK_E:
		{
			if (pPlayer->CanUseSkill(L"E") == false) {
				return;
			}

			pPlayer->Set_CurActionCommand(eAction_Command);
			pPlayer->Set_WaitActionState(L"CLiDailin_E");
			break;
		}
		case ACTION_COMMAND_TYPE::ATTACK_R:
		{

			if (pPlayer->CanUseSkill(L"R") == false) {
				return;
			}

			pPlayer->Set_CurActionCommand(eAction_Command);
			pPlayer->Set_WaitActionState(L"CLiDailin_R");

			break;
		}
	}
}

void CLiDailinAttack::Attack(CPlayer* pPlayer)
{
	// Ani
	if (rand() % 2 == 0) {
		pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(LiDailin_Ani::Ani_ATK_1), false);
		pPlayer->Set_CurAni(LiDailin_Ani::Ani_ATK_1);
		pPlayer->Get_Weapon()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(Nunchaku_Ani::ATK_1_WP), false);
	}
	else
	{
		pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(LiDailin_Ani::Ani_ATK_2), false);
		pPlayer->Set_CurAni(LiDailin_Ani::Ani_ATK_2);
		pPlayer->Get_Weapon()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(Nunchaku_Ani::ATK_2_WP), false);
	}

	pPlayer->Set_AniBlock(true);
	pPlayer->Set_WaitMovementState(L"Idle");
	m_bChase = false;
}

void CLiDailinAttack::Chase(CPlayer* pPlayer)
{
	MOVEMENT_COMMAND tMovement_Command{};
	tMovement_Command.eCommandType = MOVEMENT_COMMAND_TYPE::MOVE;
	tMovement_Command.vTargetPos = pPlayer->Get_CurActionCommand().vTargetPos;

	pPlayer->Process_MovementCommand(tMovement_Command);

	pPlayer->Set_AniBlock(false);
	m_bChase = true;
}

void CLiDailinAttack::DoAttackOrChase(CPlayer* pPlayer)
{
	//if(pPlayer->Get_CurActionCommand().pGameObject와 거리 비교)
	if (CGameInstance::GetInstance()->Key_Pressing(DIK_S) /* 범위 안 이면 */) {
		Attack(pPlayer);
	}
	else /* 범위 밖이면 */
	{
		Chase(pPlayer);
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
