#include "CLiDailinMove.h"

#include "CLiDailin.h"
#include "CBody_Player.h"
#include "CWeapon.h"
#include "CGameInstance.h"

CLiDailinMove::CLiDailinMove()
{
	m_iFootStepStartSoundKey = ETOUI(SOUND_KEY::FOOTSTEPASPHALT_W1);
	m_iFootStepSoundKeyCount = ETOUI(SOUND_KEY::FOOTSTEPASPHALT_W4) - m_iFootStepStartSoundKey;

	m_fFootStepRatios[0] = 0.45f;
	m_fFootStepRatios[1] = 0.9f;
}

void CLiDailinMove::Enter(CLiDailin* pPlayer)
{
	// Ani
	if (pPlayer->Get_MovementAniBlock() == false)
	{
		pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(LiDailin_Ani::Ani_Run), true);
		pPlayer->Set_CurAni(LiDailin_Ani::Ani_Run);
		pPlayer->Get_Weapon()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(Nunchaku_Ani::RUN_WP), true);
	}

	// 이동
	//if (pPlayer->Get_MoveBlock() == false)
	//{
		_float3 vTargetPos = pPlayer->Get_CurMovementCommand().vTargetPos;
		pPlayer->Get_MoveCom()->Move_To_Pos(vTargetPos, true);
	//}
}

void CLiDailinMove::Update(CLiDailin* pPlayer, _float fTimeDelta)
{
	if (pPlayer->Get_MoveBlock() == true)
	{
		return;
	}

	const LiDailin_Ani& eCurAni = pPlayer->Get_CurAni();
	if (pPlayer->Get_MovementAniBlock() == false && eCurAni == LiDailin_Ani::Ani_None)
	{
		pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(LiDailin_Ani::Ani_Run), true);
		pPlayer->Set_CurAni(LiDailin_Ani::Ani_Run);
		pPlayer->Get_Weapon()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(Nunchaku_Ani::RUN_WP), true);
	}

	// 도착하면
	if (pPlayer->Get_MoveCom()->Update_Move_To_Pos(fTimeDelta))
	{
		pPlayer->Set_WaitMovementState(L"Idle");
	}

	CMyModel* pModel = pPlayer->Get_BodyPlayer()->Get_ModelCom();
	_uint iCurAniIndex = pModel->Get_CurAniIndex();



	if (iCurAniIndex == ETOUI(LiDailin_Ani::Ani_Run))
	{
		_float fRatio = pModel->Get_AniPlayRatio(iCurAniIndex);

		if (m_fFootStepRatios[0] < fRatio && m_bPlaySound[0] == true)
		{
			m_bPlaySound[0] = m_bPlaySound[1] = false;
		}

		if (m_fFootStepRatios[0] >= fRatio && m_bPlaySound[0] == false)
		{
			m_bPlaySound[0] = true;
			Play_FootStepSound();
		}
		else if (m_fFootStepRatios[1] >= fRatio && m_bPlaySound[1] == false)
		{
			m_bPlaySound[1] = true;
			Play_FootStepSound();
		}
	}
}

void CLiDailinMove::Exit(CLiDailin* pPlayer)
{
	if (pPlayer->Get_MoveBlock() == true) {
		return;
	}
}

void CLiDailinMove::HandleMovementCommand(CLiDailin* pPlayer, MOVEMENT_COMMAND& eMovement_Command)
{
	switch (eMovement_Command.eCommandType) {
		case MOVEMENT_COMMAND_TYPE::MOVE:
		{
			if (pPlayer->Get_MoveBlock() == true && pPlayer->Get_CanMoveCancle() == false) {
				return;
			}

			pPlayer->Set_CurMovementCommand(eMovement_Command);
			pPlayer->Set_WaitMovementState(L"Move");
			break;
		}
	}
}

void CLiDailinMove::Play_FootStepSound()
{
	m_iFootStepIndex = (m_iFootStepIndex + 1) % m_iFootStepSoundKeyCount;
	CGameInstance::GetInstance()->PlaySound_Once(m_iFootStepStartSoundKey + m_iFootStepIndex);
}

CLiDailinMove* CLiDailinMove::Create()
{
	return new CLiDailinMove;
}

void CLiDailinMove::Free()
{
	__super::Free();
}
