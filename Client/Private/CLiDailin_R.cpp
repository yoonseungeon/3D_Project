#include "CLiDailin_R.h"

#include "CPlayer.h"
#include "CBody_Player.h"
#include "CGameInstance.h"

CLiDailin_R::CLiDailin_R()
{
}

void CLiDailin_R::Enter(CPlayer* pPlayer)
{
	// Cool
	m_fChanneling = 0.1f;

	// Cool
	COOL_INFO* pRCoolInfo = pPlayer->Get_CoolInfo(L"R");
	pRCoolInfo->bCoolWait = true;
	pRCoolInfo->fAccCoolDown = pRCoolInfo->fCurCoolDown;

	// Ani
	pPlayer->Set_Animation(L"Body", static_cast<_uint>(LiDailin_Ani::Ani_R1), false);
	pPlayer->Set_Animation(L"Weapon", static_cast<_uint>(Nunchaku_Ani::IDLE_WP), false);

	// Ani Speed


	// ÀÌµ¿
	pPlayer->Stop_Move_To_Pos();
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
		if (fAniRatio >= 0.2f && fAniRatio <= 0.6f) {
			static_cast<CMove*>(pPlayer->Find_Component(TEXT("Com_Move")))->Go_Straight(fTimeDelta, 22.f, true);
		}
	}
	else if(bIsCol == true && bIsAniR2Changed == false)
	{
		pPlayer->Set_Animation(L"Body", static_cast<_uint>(LiDailin_Ani::Ani_R2), false);
		bIsAniR2Changed = true;
	}


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

void CLiDailin_R::Exit(CPlayer* pPlayer)
{
	bIsCol = false;
	bIsAniR2Changed = false;

	COOL_INFO* pECoolInfo = pPlayer->Get_CoolInfo(L"R");
	pECoolInfo->bCoolWait = false;
}

void CLiDailin_R::HandleCommand(CPlayer* pPlayer, COMMAND& eCommand)
{
	if(bIsAniR2Changed == true)
	{
		const CMyModel* pModel = pPlayer->Get_BodyPlayer()->Get_ModelCom();
		_float fR2 = pModel->Get_CurAniPlayRatio();

		if (fR2 < 0.7f) {
			return;
		}
	}

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

CLiDailin_R* CLiDailin_R::Create()
{
	return new CLiDailin_R;
}

void CLiDailin_R::Free()
{
	__super::Free();
}
