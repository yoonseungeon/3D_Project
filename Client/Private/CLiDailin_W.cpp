#include "CLiDailin_W.h"

#include "CLiDailin.h"
#include "CBody_Player.h"
#include "CWeapon.h"
#include "CGameInstance.h"

const char* CLiDailin_W::m_pBoneNames[] =
{
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
    m_fMaxDrink = 0.8f;
    m_iMaxDrinkCount = 5;
}

HRESULT CLiDailin_W::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    m_iMaxLevel = 5;

	return S_OK;
}

void CLiDailin_W::Enter(CLiDailin* pPlayer)
{
    m_iRemainDrinkCount = m_iMaxDrinkCount;

	// Cool
	COOL_INFO* pWCoolInfo = pPlayer->Get_CoolInfo(SKILL_SLOT::W);
    pWCoolInfo->bCoolWait = true;
    pWCoolInfo->fAccCoolDown = pWCoolInfo->fCurCoolDown;

	// Ani
	pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_OverlayAnimationIndex(static_cast<_uint>(LiDailin_Ani::Ani_W1), m_pBoneNames, sizeof(m_pBoneNames)/ sizeof(const _char*));
    pPlayer->Get_Weapon()->Set_IsInactive(true);

	// Ani Speed
}

void CLiDailin_W::Update(CLiDailin* pPlayer, _float fTimeDelta)
{
    if(m_iRemainDrinkCount > 0)
    {
        m_fAccDrink += fTimeDelta;
        if(m_fAccDrink >= m_fMaxDrink / m_iMaxDrinkCount)
        {
            pPlayer->AddMP(9);
            --m_iRemainDrinkCount;

            m_fAccDrink = 0.f;
        }
    }

    if (pPlayer->Get_BodyPlayer()->Get_ModelCom()->IsAniOverlay() == false)
    {
        pPlayer->Set_ActionEnd();
    }


    const CMyModel* pModel = pPlayer->Get_BodyPlayer()->Get_ModelCom();
    _float fOverlayRatio = pModel->Get_AniPlayRatio(static_cast<_uint>(LiDailin_Ani::Ani_W1));

    if (fOverlayRatio >= 0.97f)
    {
        pPlayer->Get_Weapon()->Set_IsInactive(false);
    }
}

void CLiDailin_W::Exit(CLiDailin* pPlayer)
{
    pPlayer->Set_CurAni(LiDailin_Ani::Ani_None);

    pPlayer->Get_Weapon()->Set_IsInactive(false);

    //Cool
    COOL_INFO* pWCoolInfo = pPlayer->Get_CoolInfo(SKILL_SLOT::W);
    pWCoolInfo->bCoolWait = false;

}

void CLiDailin_W::HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command)
{
    if (pPlayer->Get_BodyPlayer()->Get_ModelCom()->IsAniOverlay() == true)
    {
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
            if (pPlayer->CanUseSkill(SKILL_SLOT::Q) == false) {
                return;
            }

            pPlayer->Set_CurActionCommand(eAction_Command);
            pPlayer->Set_WaitActionState(L"CLiDailin_Q");

            break;
        }
        case ACTION_COMMAND_TYPE::ATTACK_W:
        {
            if (pPlayer->CanUseSkill(SKILL_SLOT::W) == false) {
                return;
            }

            pPlayer->Set_CurActionCommand(eAction_Command);
            pPlayer->Set_WaitActionState(L"CLiDailin_W");
            break;
        }
        case ACTION_COMMAND_TYPE::ATTACK_E:
        {
            if (pPlayer->CanUseSkill(SKILL_SLOT::E) == false) {
                return;
            }

            pPlayer->Set_CurActionCommand(eAction_Command);
            pPlayer->Set_WaitActionState(L"CLiDailin_E");
            break;
        }
        case ACTION_COMMAND_TYPE::ATTACK_R:
        {

            if (pPlayer->CanUseSkill(SKILL_SLOT::R) == false) {
                return;
            }

            pPlayer->Set_CurActionCommand(eAction_Command);
            pPlayer->Set_WaitActionState(L"CLiDailin_R");

            break;
        }
    }
}

CLiDailin_W* CLiDailin_W::Create()
{
	CLiDailin_W* pInstance = new CLiDailin_W();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created: CLiDailin_W");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLiDailin_W::Free()
{
	__super::Free();
}
