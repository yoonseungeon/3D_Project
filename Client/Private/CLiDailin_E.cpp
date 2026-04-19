#include "CLiDailin_E.h"

#include "CPlayer.h"
#include "CBody_Player.h"
#include "CWeapon.h"
#include "CGameInstance.h"

CLiDailin_E::CLiDailin_E()
{
}

void CLiDailin_E::Enter(CPlayer* pPlayer)
{
	m_fChanneling = 0.2f;
	m_bCancleLock = true;

	// Cool
	COOL_INFO* pECoolInfo = pPlayer->Get_CoolInfo(L"E");
	pECoolInfo->bCoolWait = true;
	pECoolInfo->fAccCoolDown = pECoolInfo->fCurCoolDown;

	// Ani
    pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(LiDailin_Ani::Ani_E), false);
    pPlayer->Get_Weapon()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(Nunchaku_Ani::IDLE_WP), false);
    pPlayer->Set_AniBlock(true);

	// Ani Speed

	// ÀÌµ¿
    pPlayer->Set_MoveBlock(true);
    pPlayer->Set_WaitMovementState(L"Idle");
}

void CLiDailin_E::Update(CPlayer* pPlayer, _float fTimeDelta)
{
	if (m_fChanneling > 0.f) {
		m_fChanneling -= fTimeDelta;
		if (m_fChanneling <= 0.f) {
			m_fChanneling = 0.f;
			m_bCancleLock = false;

			COOL_INFO* pECoolInfo = pPlayer->Get_CoolInfo(L"E");
			pECoolInfo->bCoolWait = false;
		}
	}

    if (pPlayer->Get_BodyPlayer()->Get_ModelCom()->IsAnimationFinished() == true) {
        pPlayer->Set_ActionEnd();
    }
}

void CLiDailin_E::Exit(CPlayer* pPlayer)
{
    pPlayer->Set_CurAni(LiDailin_Ani::Ani_None);
    pPlayer->Set_AniBlock(false);
    pPlayer->Set_MoveBlock(false);
}

void CLiDailin_E::HandleActionCommand(CPlayer* pPlayer, ACTION_COMMAND& eAction_Command)
{
	if (m_bCancleLock == true)
	{
		return;
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

CLiDailin_E* CLiDailin_E::Create()
{
	return new CLiDailin_E;
}

void CLiDailin_E::Free()
{
	__super::Free();
}
