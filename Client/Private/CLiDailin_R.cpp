#include "CLiDailin_R.h"

#include "CPlayer.h"
#include "CBody_Player.h"
#include "CGameInstance.h"

CLiDailin_R::CLiDailin_R()
{
}

void CLiDailin_R::Enter(CPlayer* pPlayer)
{
	// Ani


	// Ani Speed


	// ÀÌµ¿
	pPlayer->Stop_Move_To_Pos();
}

void CLiDailin_R::Update(CPlayer* pPlayer, _float fTimeDelta)
{

}

void CLiDailin_R::Exit(CPlayer* pPlayer)
{
}

void CLiDailin_R::HandleCommand(CPlayer* pPlayer, COMMAND& eCommand)
{
	switch (eCommand.eCommandType) {
	case COMMAND_TYPE::MOVE:
	{

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
