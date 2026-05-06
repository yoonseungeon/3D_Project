#include "CLiDailinAttack.h"

#include "CGameInstance.h"

#include "CLiDailin.h"
#include "CBody_Player.h"
#include "CWeapon.h"

#include "CAbstractMonster.h"

CLiDailinAttack::CLiDailinAttack()
{
}

void CLiDailinAttack::Enter(CLiDailin* pPlayer)
{	
	pPlayer->Set_CanMoveCancle(true);

	if (CanAttack(pPlayer)) {
		Attack(pPlayer);
	}
	else /* 범위 밖이면 */
	{
		Chase(pPlayer);
	}
}

void CLiDailinAttack::Update(CLiDailin* pPlayer, _float fTimeDelta)
{
	if (m_bIsChasing == true)
	{
		if (CanAttack(pPlayer)) {
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

	CAbstractMonster* pMonster = dynamic_cast<CAbstractMonster*>(tAction_Command.pGameObject);
	if (pMonster == nullptr)
		MSG_BOX("Bug Point 4: CLiDailinAttack");

	if (pMonster->Get_IsInvincible() == false && pModel->Get_CurAniPlayRatio() >= 0.75f)
	{
		pPlayer->Set_WaitActionState(L"CLiDailinAttack");
	}

	if (pPlayer->Get_BodyPlayer()->Get_ModelCom()->IsAnimationFinished() == true) {
		pPlayer->Set_ActionEnd();
	}
}

void CLiDailinAttack::Exit(CLiDailin* pPlayer)
{
	pPlayer->Set_CanMoveCancle(false);

	pPlayer->Set_CurAni(LiDailin_Ani::Ani_None);
	pPlayer->Set_MovementAniBlock(false);
}

void CLiDailinAttack::HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command)
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
			if (pPlayer->CanUseSkill(SKILL_SLOT::Q) == false) {
				return;
			}

			pPlayer->Set_CurActionCommand(eAction_Command);
			pPlayer->Set_WaitActionState(L"CLiDailin_Q");

			break;
		}
		case ACTION_COMMAND_TYPE::ATTACK_W:
		{
			if (pPlayer->CanUseSkill(SKILL_SLOT::W) == false) {
				return;
			}

			pPlayer->Set_CurActionCommand(eAction_Command);
			pPlayer->Set_WaitActionState(L"CLiDailin_W");
			break;
		}
		case ACTION_COMMAND_TYPE::ATTACK_E:
		{
			if (pPlayer->CanUseSkill(SKILL_SLOT::E) == false) {
				return;
			}

			pPlayer->Set_CurActionCommand(eAction_Command);
			pPlayer->Set_WaitActionState(L"CLiDailin_E");
			break;
		}
		case ACTION_COMMAND_TYPE::ATTACK_R:
		{

			if (pPlayer->CanUseSkill(SKILL_SLOT::R) == false) {
				return;
			}

			pPlayer->Set_CurActionCommand(eAction_Command);
			pPlayer->Set_WaitActionState(L"CLiDailin_R");

			break;
		}
	}
}

void CLiDailinAttack::Attack(CLiDailin* pPlayer)
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

	pPlayer->Set_MovementAniBlock(true);
	pPlayer->Set_WaitMovementState(L"Idle");
	m_bIsChasing = false;
}

void CLiDailinAttack::Chase(CLiDailin* pPlayer)
{
	const ACTION_COMMAND& tAction_Command = pPlayer->Get_CurActionCommand();
	CTransform* pMonsterTransform = dynamic_cast<CTransform*>(tAction_Command.pGameObject->Find_Component(g_strTransformTag));
	if (pMonsterTransform == nullptr)
		MSG_BOX("Bug Point 3: CLiDailinAttack");

	MOVEMENT_COMMAND tMovement_Command{};
	tMovement_Command.eCommandType = MOVEMENT_COMMAND_TYPE::MOVE;
	XMStoreFloat3(&tMovement_Command.vTargetPos, pMonsterTransform->Get_State(STATE::POSITION));
	
	pPlayer->Process_MovementCommand(tMovement_Command);

	pPlayer->Set_MovementAniBlock(false);
	m_bIsChasing = true;
}

_bool CLiDailinAttack::CanAttack(CLiDailin* pPlayer)
{
	const ACTION_COMMAND& tAction_Command = pPlayer->Get_CurActionCommand();
	m_fAttackRange = tAction_Command.Data_UInt.fAttackRange;

	CAbstractMonster* pMonster = dynamic_cast<CAbstractMonster*>(tAction_Command.pGameObject);
	if (pMonster == nullptr)
		MSG_BOX("Bug Point 1: CLiDailinAttack");

	CTransform* pMonsterTransform = dynamic_cast<CTransform*>(pMonster->Find_Component(g_strTransformTag));
	if (pMonsterTransform == nullptr)
		MSG_BOX("Bug Point 2: CLiDailinAttack");

	return pPlayer->IsInRange(pMonsterTransform->Get_State(STATE::POSITION), m_fAttackRange);
}

CLiDailinAttack* CLiDailinAttack::Create()
{
	return new CLiDailinAttack;
}

void CLiDailinAttack::Free()
{
	__super::Free();
}
