#include "CLiDailin_W.h"

#include "CPlayer.h"
#include "CBody_Player.h"
#include "CGameInstance.h"

CLiDailin_W::CLiDailin_W()
{
}

HRESULT CLiDailin_W::Initialize()
{
	m_BoneNames =
	{
		// Spine / Head
		"Bip001 Spine", "Bip001 Spine1", "Bip001 Spine2", "Bip001 Neck", "Bip001 Head",

		// Left Arm
		"Bip001 L Clavicle", "Bip001 L UpperArm", "Bip001 L Forearm", "Bip001 L Hand",

		// Left Fingers
		"Bip001 L Finger0", "Bip001 L Finger01", "Bip001 L Finger02",

		"Bip001 L Finger1", "Bip001 L Finger11", "Bip001 L Finger12",

		"Bip001 L Finger2", "Bip001 L Finger21", "Bip001 L Finger22",

		"Bip001 L Finger3", "Bip001 L Finger31", "Bip001 L Finger32",

		"Bip001 L Finger4", "Bip001 L Finger41", "Bip001 L Finger42",

		// Right Arm
		"Bip001 R Clavicle", "Bip001 R UpperArm", "Bip001 R Forearm", "Bip001 R Hand",

		// Right Fingers
		"Bip001 R Finger0", "Bip001 R Finger01", "Bip001 R Finger02",

		"Bip001 R Finger1", "Bip001 R Finger11", "Bip001 R Finger12",

		"Bip001 R Finger2", "Bip001 R Finger21", "Bip001 R Finger22",

		"Bip001 R Finger3", "Bip001 R Finger31", "Bip001 R Finger32",

		"Bip001 R Finger4", "Bip001 R Finger41", "Bip001 R Finger42",

		// Weapon Special 1
		"Weapon_Special_1"
	};

	return S_OK;
}

void CLiDailin_W::Enter(CPlayer* pPlayer)
{
	// Cool
	//COOL_INFO* pECoolInfo = pPlayer->Get_CoolInfo(L"E");
	//pECoolInfo->bCoolWait = true;
	//pECoolInfo->fAccCoolDown = pECoolInfo->fCurCoolDown;

	// Ani

	pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_OverlayAnimationIndex(LiDailin_Ani::Ani_W1, m_BoneNames);
	pPlayer->Set_Animation(L"Body", static_cast<_uint>(LiDailin_Ani::Ani_W2), false);
	//pPlayer->Set_Animation(L"Weapon", static_cast<_uint>(Nunchaku_Ani::IDLE_WP), false);

	// Ani Speed


	// ÀÌµ¿
	//pPlayer->Stop_Move_To_Pos();
}

void CLiDailin_W::Update(CPlayer* pPlayer, _float fTimeDelta)
{
	_bool bIsMoving = pPlayer->Update_Move_To_Pos(fTimeDelta);

	if (pPlayer->Get_BodyPlayer()->Get_ModelCom()->IsAniOverlay() == false) {
		if (bIsMoving == false) {
			pPlayer->Set_WaitState(L"Move");
		}
		else
		{
			pPlayer->Set_WaitState(L"Idle");
		}
	}
}

void CLiDailin_W::Exit(CPlayer* pPlayer)
{
}

void CLiDailin_W::HandleCommand(CPlayer* pPlayer, COMMAND& eCommand)
{
	switch (eCommand.eCommandType) {
	case COMMAND_TYPE::MOVE:
	{
		break;
	}
	case COMMAND_TYPE::ATTACK:
	{
		break;
	}
	case COMMAND_TYPE::ATTACK_Q:
	{
		break;
	}
	case COMMAND_TYPE::ATTACK_W:
	{
		break;
	}
	case COMMAND_TYPE::ATTACK_E:
	{
		break;
	}
	case COMMAND_TYPE::ATTACK_R:
	{
		break;
	}
	}
}

CLiDailin_W* CLiDailin_W::Create()
{
	CLiDailin_W* pInstance = new CLiDailin_W();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLiDailin_W");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLiDailin_W::Free()
{
	__super::Free();
}
