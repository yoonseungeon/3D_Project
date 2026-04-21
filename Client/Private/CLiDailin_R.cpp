#include "CLiDailin_R.h"

#include "CPlayer.h"
#include "CBody_Player.h"
#include "CWeapon.h"
#include "CGameInstance.h"

CLiDailin_R::CLiDailin_R()
{
}

void CLiDailin_R::Enter(CPlayer* pPlayer)
{
	// Cool
	m_fChanneling = 0.1f;

	// Cool
	COOL_INFO* pRCoolInfo = pPlayer->Get_CoolInfo(ICON_TYPE::R);
	pRCoolInfo->bCoolWait = true;
	pRCoolInfo->fAccCoolDown = pRCoolInfo->fCurCoolDown;

	// Ani
	pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(LiDailin_Ani::Ani_R1), false);
	pPlayer->Get_Weapon()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(Nunchaku_Ani::IDLE_WP), false);
	pPlayer->Set_AniBlock(true);

	// Ani Speed

	// ÀÌµ¿
	pPlayer->Set_MoveBlock(true);
	pPlayer->Set_WaitMovementState(L"Idle");
}

void CLiDailin_R::Update(CPlayer* pPlayer, _float fTimeDelta)
{
	if (m_fChanneling > 0.f) {
		m_fChanneling -= fTimeDelta;
		if (m_fChanneling <= 0.f) {
			m_fChanneling = 0.f;

		}
	}

	if (CGameInstance::GetInstance()->Key_Down(DIK_T)) {
		bIsCol = true;
	}

	if (bIsCol == false)
	{
		const CMyModel* pModel = pPlayer->Get_BodyPlayer()->Get_ModelCom();
		_float fAniRatio = pModel->Get_CurAniPlayRatio();
		if (fAniRatio >= 0.2f && fAniRatio <= 0.6f)
		{
			static_cast<CMove*>(pPlayer->Find_Component(TEXT("Com_Move")))->Go_Straight(fTimeDelta, 22.f, true);
		}
	}
	else if(bIsCol == true && bIsAniR2Changed == false)
	{
		pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(LiDailin_Ani::Ani_R2), false);

		bIsAniR2Changed = true;
	}

	if (bIsAniR2Changed == true)
	{
		const CMyModel* pModel = pPlayer->Get_BodyPlayer()->Get_ModelCom();
		_float fR2 = pModel->Get_CurAniPlayRatio();

		if (fR2 >= 0.7f) {
			pPlayer->Set_CanMoveCancle(true);
		}
	}

	if (pPlayer->Get_BodyPlayer()->Get_ModelCom()->IsAnimationFinished() == true) {
		pPlayer->Set_ActionEnd();
	}
}

void CLiDailin_R::Exit(CPlayer* pPlayer)
{
	pPlayer->Set_AniBlock(false);
	pPlayer->Set_MoveBlock(false);

	bIsCol = false;
	bIsAniR2Changed = false;

	COOL_INFO* pECoolInfo = pPlayer->Get_CoolInfo(ICON_TYPE::R);
	pECoolInfo->bCoolWait = false;

	pPlayer->Set_CurAni(LiDailin_Ani::Ani_None);

	pPlayer->Set_CanMoveCancle(false);

}

void CLiDailin_R::HandleActionCommand(CPlayer* pPlayer, ACTION_COMMAND& eAction_Command)
{
	if (bIsAniR2Changed == false) {
		return;
	}
	else if(bIsAniR2Changed == true)
	{
		const CMyModel* pModel = pPlayer->Get_BodyPlayer()->Get_ModelCom();
		_float fR2 = pModel->Get_CurAniPlayRatio();

		if (fR2 < 0.7f) {
			return;
		}
	}

	switch (eAction_Command.eCommandType) {
		case ACTION_COMMAND_TYPE::ATTACK:
		{
			pPlayer->Set_CurActionCommand(eAction_Command);
			pPlayer->Set_WaitActionState(L"CLiDailinAttack");

			break;
		}
		case ACTION_COMMAND_TYPE::ATTACK_Q:
		{
			if (pPlayer->CanUseSkill(ICON_TYPE::Q) == false) {
				return;
			}

			pPlayer->Set_CurActionCommand(eAction_Command);
			pPlayer->Set_WaitActionState(L"CLiDailin_Q");

			break;
		}
		case ACTION_COMMAND_TYPE::ATTACK_W:
		{
			if (pPlayer->CanUseSkill(ICON_TYPE::W) == false) {
				return;
			}

			pPlayer->Set_CurActionCommand(eAction_Command);
			pPlayer->Set_WaitActionState(L"CLiDailin_W");
			break;
		}
		case ACTION_COMMAND_TYPE::ATTACK_E:
		{
			if (pPlayer->CanUseSkill(ICON_TYPE::E) == false) {
				return;
			}

			pPlayer->Set_CurActionCommand(eAction_Command);
			pPlayer->Set_WaitActionState(L"CLiDailin_E");
			break;
		}
		case ACTION_COMMAND_TYPE::ATTACK_R:
		{

			if (pPlayer->CanUseSkill(ICON_TYPE::R) == false) {
				return;
			}

			pPlayer->Set_CurActionCommand(eAction_Command);
			pPlayer->Set_WaitActionState(L"CLiDailin_R");

			break;
		}
	}
}

CLiDailin_R* CLiDailin_R::Create()
{
	return new CLiDailin_R;
}

void CLiDailin_R::Free()
{
	__super::Free();
}
