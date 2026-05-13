#include "CLiDailin_E.h"

#include "CLiDailin.h"
#include "CBody_Player.h"
#include "CWeapon.h"
#include "CGameInstance.h"

CLiDailin_E::CLiDailin_E()
{
    m_fMaxChanneling = 0.3f;
}

HRESULT CLiDailin_E::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    m_iMaxLevel = 5;

    return S_OK;
}

void CLiDailin_E::Enter(CLiDailin* pPlayer)
{
	m_fChanneling = m_fMaxChanneling;
	m_bCancleLock = true;

	// Cool
	COOL_INFO* pECoolInfo = pPlayer->Get_CoolInfo(SKILL_SLOT::E);
	pECoolInfo->bCoolWait = true;
	pECoolInfo->fAccCoolDown = pECoolInfo->fCurCoolDown;

	// Ani
    pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(LiDailin_Ani::Ani_E), false);
    pPlayer->Get_Weapon()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(Nunchaku_Ani::IDLE_WP), false);
    pPlayer->Set_MovementAniBlock(true);

	// Ani Speed
    pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AniSpeed(static_cast<_uint>(LiDailin_Ani::Ani_E), 1.5f);

	// 이동
    pPlayer->Set_MoveBlock(true);
    pPlayer->Set_WaitMovementState(L"Idle");

    // 방향
    pPlayer->Look_MouseDir();

    //MP
    const _int iConsumeIntoxication = 40;
    if (pPlayer->Get_CurStat().iMP >= iConsumeIntoxication)
    {
        pPlayer->AddMP(-iConsumeIntoxication);
        pPlayer->Set_EnhancedBasicATK(true);
        m_bEnhanced = true;
    }
}

void CLiDailin_E::Update(CLiDailin* pPlayer, _float fTimeDelta)
{
	if (m_fChanneling > 0.f) {
		m_fChanneling -= fTimeDelta;
		if (m_fChanneling <= 0.f) {
			m_fChanneling = 0.f;
			m_bCancleLock = false;

            pPlayer->Set_CanMoveCancle(true);


			COOL_INFO* pECoolInfo = pPlayer->Get_CoolInfo(SKILL_SLOT::E);
			pECoolInfo->bCoolWait = false;
		}
	}

    const CMyModel* pModel = pPlayer->Get_BodyPlayer()->Get_ModelCom();
    const _float fAniRatio = pModel->Get_CurAniPlayRatio();
    if (fAniRatio >= 0.2f && fAniRatio <= 0.3f)
    {
        pPlayer->Get_Collider(CLiDailin::LIDAILIN_COLLIDER::LIDAILIN_E)->Set_Active(true);
    }
    else
    {
        pPlayer->Get_Collider(CLiDailin::LIDAILIN_COLLIDER::LIDAILIN_E)->Set_Active(false);
    }

    if (pPlayer->Get_BodyPlayer()->Get_ModelCom()->IsAnimationFinished() == true) {
        pPlayer->Set_ActionEnd();
    }
}

void CLiDailin_E::Exit(CLiDailin* pPlayer)
{
    m_AttackedObj.clear();
    m_bEnhanced = false;

    pPlayer->Get_Collider(CLiDailin::LIDAILIN_COLLIDER::LIDAILIN_E)->Set_Active(false);

    pPlayer->Set_MovementAniBlock(false);
    pPlayer->Set_MoveBlock(false);

    pPlayer->Set_CurAni(LiDailin_Ani::Ani_None);

    pPlayer->Set_CanMoveCancle(false);
}

void CLiDailin_E::HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command)
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

void CLiDailin_E::OnCollision_Enter(const COLLISION_INFO& tCollision)
{
    if (tCollision.pColCollider->Get_Layer() == ETOUI(Collision_Layer::MONSTER))
    {
        auto iter = m_AttackedObj.insert(tCollision.pColObject);

        if (iter.second == false)
            return;

        DAMAGE_INFO tDamageInfo = { static_cast<CUnit*>(tCollision.pMyCollider->Get_Owner()), 50 };
        static_cast<CUnit*>(tCollision.pColObject)->Damaged(tDamageInfo);
    }
}

void CLiDailin_E::OnCollision_Stay(const COLLISION_INFO& tCollision)
{
}

void CLiDailin_E::OnCollision_Exit(const COLLISION_INFO& tCollision)
{
}

CLiDailin_E* CLiDailin_E::Create()
{
    CLiDailin_E* pInstance = new CLiDailin_E();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created: CLiDailin_E");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLiDailin_E::Free()
{
	__super::Free();
}
