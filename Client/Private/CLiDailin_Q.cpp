#include "CLiDailin_Q.h"

#include "CPlayer.h"
#include "CBody_Player.h"
#include "CWeapon.h"
#include "CGameInstance.h"

CLiDailin_Q::CLiDailin_Q()
{
}

void CLiDailin_Q::Enter(CPlayer* pPlayer)
{
	// Cool
	STACK_COOL_INFO& tQCoolInfo = pPlayer->Get_QCoolInfo();
	tQCoolInfo.fAccSubCoolDown = tQCoolInfo.fCurSubCoolDown;

	// Ani
	_uint iBodyPhase{};
	_uint iWeaponPhase{};

	switch (tQCoolInfo.fStack) {
		case 0:
		{
			iBodyPhase = static_cast<_uint>(LiDailin_Ani::Ani_Q1);
			pPlayer->Set_CurAni(LiDailin_Ani::Ani_Q1);
			pPlayer->Set_AniBlock(true);
			iWeaponPhase = static_cast<_uint>(Nunchaku_Ani::Q1_WP);
			break;
		}
		case 1:
		{
			iBodyPhase = static_cast<_uint>(LiDailin_Ani::Ani_Q2);
			pPlayer->Set_CurAni(LiDailin_Ani::Ani_Q2);
			pPlayer->Set_AniBlock(true);
			iWeaponPhase = static_cast<_uint>(Nunchaku_Ani::Q2_WP);
			break;
		}
		case 2:
		{
			iBodyPhase = static_cast<_uint>(LiDailin_Ani::Ani_Q3);
			pPlayer->Set_CurAni(LiDailin_Ani::Ani_Q3);
			pPlayer->Set_AniBlock(true);
			iWeaponPhase = static_cast<_uint>(Nunchaku_Ani::Q3_WP);
			break;
		}
	}

	pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(iBodyPhase, false);
	pPlayer->Get_Weapon()->Get_ModelCom()->Set_AnimationIndex(iWeaponPhase, false);

	tQCoolInfo.fStack = (++tQCoolInfo.fStack) % 3;

	if (tQCoolInfo.fStack == 0) {
		tQCoolInfo.fAccCoolDown = tQCoolInfo.fCurCoolDown;
		tQCoolInfo.fAccSubCoolDown = 0.f;
	}

	// Ani Speed
	pPlayer->Set_AniSpeed(L"Body", iBodyPhase, 2.f);
	pPlayer->Set_AniSpeed(L"Weapon", iWeaponPhase, 2.f);

	// 이동
	pPlayer->Set_MoveBlock(true);
	pPlayer->Set_WaitMovementState(L"Idle");
}

void CLiDailin_Q::Update(CPlayer* pPlayer, _float fTimeDelta)
{
	STACK_COOL_INFO& tQCoolInfo = pPlayer->Get_QCoolInfo();
	const CMyModel* pModel = pPlayer->Get_BodyPlayer()->Get_ModelCom();

	// 0인 경우는 3타
	if (tQCoolInfo.fStack == 0 && pModel->Get_CurAniPlayRatio() <= 0.5f)
	{
		static_cast<CMove*>(pPlayer->Find_Component(TEXT("Com_Move")))->Go_Straight(fTimeDelta, 7.f, true);
	}
	else if(tQCoolInfo.fStack != 0){
		static_cast<CMove*>(pPlayer->Find_Component(TEXT("Com_Move")))->Go_Straight(fTimeDelta, 4.f, true);
	}

	if (pPlayer->Get_BodyPlayer()->Get_ModelCom()->IsAnimationFinished() == true) {
		pPlayer->Set_ActionEnd();
	}
}

void CLiDailin_Q::Exit(CPlayer* pPlayer)
{
	pPlayer->Set_CurAni(LiDailin_Ani::Ani_None);
	pPlayer->Set_AniBlock(false);
	pPlayer->Set_MoveBlock(false);
}

void CLiDailin_Q::HandleActionCommand(CPlayer* pPlayer, ACTION_COMMAND& eAction_Command)
{
	if (pPlayer->Get_BodyPlayer()->Get_ModelCom()->IsAnimationFinished() == false) {
		return;
	}

	switch (eAction_Command.eCommandType)
	{
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

CLiDailin_Q* CLiDailin_Q::Create()
{
	return new CLiDailin_Q;
}

void CLiDailin_Q::Free()
{
	__super::Free();
}
