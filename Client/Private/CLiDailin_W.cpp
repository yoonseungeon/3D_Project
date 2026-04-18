#include "CLiDailin_W.h"

#include "CPlayer.h"
#include "CBody_Player.h"
#include "CGameInstance.h"

const char* CLiDailin_W::m_pBoneNames[] = {
    "Bip001 Spine",
    "Bip001 Spine1",
    "Bip001 Spine2",
    "Bip001 L Clavicle",
    "Bip001 L UpperArm",
    "Bip001 L Forearm",
    "Bip001 L Hand",
    "Bip001 L Finger0",
    "Bip001 L Finger01",
    "Bip001 L Finger1",
    "Bip001 L Finger11",
    "Bip001 L Finger2",
    "Bip001 L Finger21",
    "Elbow_L",
    "Sleeve_L_NonSpring_01",
    "Sleeve_L_Spring_01",
    "Shoulder_L_Bone",
    "Bip001 Neck",
    "Bip001 Head",
    "Eye_dummy",
    "Hair_Root",
    "Hair_F_Spring_01",
    "Hair_L_Root",
    "Hair_BL_NonSpring",
    "Hair_BL_Spring_01",
    "Hair_BL_Spring_02",
    "Hair_BL_Spring_03",
    "Hair_BL_Spring_04",
    "Hair_BL_Spring_05",
    "Hair_BL_Spring_06",
    "Ribbon_L_Magic_00",
    "Ribbon_L_Magic_01",
    "Ribbon_L_Magic_02",
    "Ribbon_L_Magic_03",
    "Rope_LD_Spring_01",
    "Rope_LU_Spring_01",
    "Hair_L_Spring_01",
    "Hair_L_Spring_02",
    "Hair_R_Root",
    "Hair_BR_NonSpring",
    "Hair_BR_Spring_01",
    "Hair_BR_Spring_02",
    "Hair_BR_Spring_03",
    "Hair_BR_Spring_04",
    "Hair_BR_Spring_05",
    "Hair_BR_Spring_06",
    "Ribbon_R_Magic_00",
    "Ribbon_R_Magic_01",
    "Ribbon_R_Magic_02",
    "Ribbon_R_Magic_03",
    "Rope_RD_Spring_01",
    "Rope_RU_Spring_01",
    "Hair_R_Spring_01",
    "Hair_R_Spring_02",
    "Bip001 R Clavicle",
    "Bip001 R UpperArm",
    "Bip001 R Forearm",
    "Bip001 R Hand",
    "Bip001 R Finger0",
    "Bip001 R Finger01",
    "Bip001 R Finger1",
    "Bip001 R Finger11",
    "Bip001 R Finger2",
    "Bip001 R Finger21",
    "Elbow_R",
    "Sleeve_R_NonSpring_01",
    "Sleeve_R_Spring_01",
    "Shoulder_R_Bone",
    "Chest_Acce_Spring_01",
    "Skirt_Root",
    "Skirt_B_NonSpring_01",
    "Skirt_B_NonSpring_02",
    "Skirt_B_Spring_01",
    "Skirt_BL_NonSpring_01",
    "Skirt_BL_NonSpring_02",
    "Skirt_BL_Spring_01",
    "Skirt_BR_NonSpring_01",
    "Skirt_BR_NonSpring_02",
    "Skirt_BR_Spring_01",
    "Skirt_F_NonSpring_01",
    "Skirt_F_NonSpring_02",
    "Skirt_F_Spring_01",
    "Skirt_FR_NonSpring_01",
    "Skirt_FR_NonSpring_02",
    "Skirt_FR_Spring_01",
    "Skirt_R_NonSpring_01",
    "Skirt_R_NonSpring_02",
    "Skirt_R_Spring_01",
    "Weapon_Special_1"
};
CLiDailin_W::CLiDailin_W()
{
}

HRESULT CLiDailin_W::Initialize()
{
	

	return S_OK;
}

void CLiDailin_W::Enter(CPlayer* pPlayer)
{
	// Cool
	//COOL_INFO* pECoolInfo = pPlayer->Get_CoolInfo(L"E");
	//pECoolInfo->bCoolWait = true;
	//pECoolInfo->fAccCoolDown = pECoolInfo->fCurCoolDown;

	// Ani

	pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_OverlayAnimationIndex(LiDailin_Ani::Ani_W1, m_pBoneNames, sizeof(m_pBoneNames)/ sizeof(const _char*));
	//pPlayer->Set_Animation(L"Body", static_cast<_uint>(LiDailin_Ani::Ani_W2), false);
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
