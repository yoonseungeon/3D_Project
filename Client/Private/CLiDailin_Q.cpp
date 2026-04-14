#include "CLiDailin_Q.h"

#include "CPlayer.h"

CLiDailin_Q::CLiDailin_Q()
{
}

void CLiDailin_Q::Enter(CPlayer* pPlayer)
{
	// Ani
	_uint iBodyPhase{};
	_uint iWeaponPhase{};
	switch (m_iPhase) {
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

	m_iPhase = (++m_iPhase) % 3;

	// Ani Speed
	pPlayer->Set_AniSpeed(L"Body", iBodyPhase, 2.2f);
	pPlayer->Set_AniSpeed(L"Weapon", iWeaponPhase, 2.2f);

	// 이동
	pPlayer->Stop_Move_To_Pos();
}

void CLiDailin_Q::Update(CPlayer* pPlayer, _float fTimeDelta)
{
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
