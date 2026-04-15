#include "CLiDailin_Q.h"

#include "CPlayer.h"
#include "CBody_Player.h"
#include "CGameInstance.h"

CLiDailin_Q::CLiDailin_Q()
{
}

void CLiDailin_Q::Enter(CPlayer* pPlayer)
{
	STACK_COOL_INFO& tQCoolInfo = pPlayer->Get_QCoolInfo();
	tQCoolInfo.fAccSubCoolDown = tQCoolInfo.fCurSubCoolDown;

	// Ani
	_uint iBodyPhase{};
	_uint iWeaponPhase{};

	switch (tQCoolInfo.fStack) {
		case 0:
		{
			iBodyPhase = static_cast<_uint>(LiDailin_Ani::Ani_Q1);
			iWeaponPhase = static_cast<_uint>(Nunchaku_Ani::Q1_WP);
			break;
		}
		case 1:
		{
			iBodyPhase = static_cast<_uint>(LiDailin_Ani::Ani_Q2);
			iWeaponPhase = static_cast<_uint>(Nunchaku_Ani::Q2_WP);
			break;
		}
		case 2:
		{
			iBodyPhase = static_cast<_uint>(LiDailin_Ani::Ani_Q3);
			iWeaponPhase = static_cast<_uint>(Nunchaku_Ani::Q3_WP);
			break;
		}
	}

	pPlayer->Set_Animation(L"Body", iBodyPhase, false);
	pPlayer->Set_Animation(L"Weapon", iWeaponPhase, false);

	tQCoolInfo.fStack = (++tQCoolInfo.fStack) % 3;

	if (tQCoolInfo.fStack == 0) {
		tQCoolInfo.fAccCoolDown = tQCoolInfo.fCurCoolDown;
		tQCoolInfo.fAccSubCoolDown = 0.f;
	}

	// Ani Speed
	pPlayer->Set_AniSpeed(L"Body", iBodyPhase, 2.f);
	pPlayer->Set_AniSpeed(L"Weapon", iWeaponPhase, 2.f);

	// 이동
	pPlayer->Stop_Move_To_Pos();
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
		static_cast<CMove*>(pPlayer->Find_Component(TEXT("Com_Move")))->Go_Straight(fTimeDelta, 5.f, true);
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

void CLiDailin_Q::Exit(CPlayer* pPlayer)
{
}

void CLiDailin_Q::HandleCommand(CPlayer* pPlayer, COMMAND& eCommand)
{
	switch (eCommand.eCommandType) {
		case COMMAND_TYPE::MOVE:
		{
			pPlayer->Set_CurCommand(eCommand);
			break;
		}
		
		// 나머지 무시
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
