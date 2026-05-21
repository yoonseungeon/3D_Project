#include "CLiDailin_R.h"

#include "CLiDailin.h"
#include "CBody_Player.h"
#include "CWeapon.h"
#include "CGameInstance.h"

#include "CAbstractMonster.h"

CLiDailin_R::CLiDailin_R()
{
}

HRESULT CLiDailin_R::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	m_iMaxLevel = 3;
	fADRatio = 0.75f;
	return S_OK;
}

void CLiDailin_R::Enter(CLiDailin* pPlayer)
{
	// Cool
	m_fChanneling = 0.1f;

	// Cool
	COOL_INFO* pRCoolInfo = pPlayer->Get_CoolInfo(SKILL_SLOT::R);
	pRCoolInfo->bCoolWait = true;
	pRCoolInfo->fAccCoolDown = pRCoolInfo->fCurCoolDown;

	// Ani
	pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(ETOUI(LiDailin_Ani::Ani_R1), false);
	pPlayer->Get_Weapon()->Get_ModelCom()->Set_AnimationIndex(ETOUI(Nunchaku_Ani::IDLE_WP), false);
	pPlayer->Set_MovementAniBlock(true);

	// Ani Speed
	pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AniSpeed(ETOUI(LiDailin_Ani::Ani_R2), 1.25f);

	// 이동
	pPlayer->Set_MoveBlock(true);
	pPlayer->Set_WaitMovementState(L"Idle");

	// 방향
	pPlayer->Look_MouseDir();

	//MP
	m_iConsumeIntoxication = pPlayer->Get_CurStat().iMP;

	const _int iConsumeIntoxicationToEnhanceBasicATK = 40;
	if (pPlayer->Get_CurStat().iMP >= iConsumeIntoxicationToEnhanceBasicATK)
	{
		pPlayer->Set_EnhancedBasicATK(true);
		m_bEnhanced = true;
	}
	pPlayer->AddMP(-m_iConsumeIntoxication);

	pPlayer->Get_Collider(CLiDailin::LIDAILIN_COLLIDER::LIDAILIN_R)->Set_Active(true);

	CGameInstance::GetInstance()->PlaySound_Once(ETOUI(SOUND_KEY::LIDAILIN_R_DASH));
}

void CLiDailin_R::Update(CLiDailin* pPlayer, _float fTimeDelta)
{
	if (m_fChanneling > 0.f) {
		m_fChanneling -= fTimeDelta;
		if (m_fChanneling <= 0.f) {
			m_fChanneling = 0.f;

		}
	}

	if (bIsCol == false)
	{
		const CMyModel* pModel = pPlayer->Get_BodyPlayer()->Get_ModelCom();
		_float fAniRatio = pModel->Get_CurAniPlayRatio();
		if (fAniRatio >= 0.2f && fAniRatio <= 0.6f)
		{
			static_cast<CMove*>(pPlayer->Find_Component(TEXT("Com_Move")))->Go_Straight(fTimeDelta, 22.f, true);
		}
	}
	else if(bIsCol == true && bIsAniRNextChanged == false)
	{
		pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(static_cast<_uint>(LiDailin_Ani::Ani_R2), false);

		bIsAniRNextChanged = true;
	}

	if (bIsAniRNextChanged == true)
	{
		const CMyModel* pModel = pPlayer->Get_BodyPlayer()->Get_ModelCom();
		_float fR2 = pModel->Get_CurAniPlayRatio();

		if (fR2 >= 0.7f) {
			pPlayer->Set_CanMoveCancle(true);
		}

		if (fR2 >= 0.086f)
			m_eRATKP |= GIVE_R2;
		if (fR2 >= 0.266f)
			m_eRATKP |= GIVE_R3;
		if (fR2 >= 0.46f)
			m_eRATKP |= GIVE_R4;
	}

	if (pPlayer->Get_BodyPlayer()->Get_ModelCom()->IsAnimationFinished() == true) {
		pPlayer->Set_ActionEnd();
	}
}

void CLiDailin_R::Exit(CLiDailin* pPlayer)
{
	m_iConsumeIntoxication = 0;
	m_eRATKP = RESET;
	m_pTempCompareTarget = nullptr;

	pPlayer->Get_Collider(CLiDailin::LIDAILIN_COLLIDER::LIDAILIN_R)->Set_Active(false);

	m_bEnhanced = false;

	pPlayer->Set_MovementAniBlock(false);
	pPlayer->Set_MoveBlock(false);

	bIsCol = false;
	bIsAniRNextChanged = false;

	COOL_INFO* pECoolInfo = pPlayer->Get_CoolInfo(SKILL_SLOT::R);
	pECoolInfo->bCoolWait = false;

	pPlayer->Set_CurAni(LiDailin_Ani::Ani_None);

	pPlayer->Set_CanMoveCancle(false);
}

void CLiDailin_R::HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command)
{
	if (bIsAniRNextChanged == false) {
		return;
	}
	else if(bIsAniRNextChanged == true)
	{
		const CMyModel* pModel = pPlayer->Get_BodyPlayer()->Get_ModelCom();
		_float fR2 = pModel->Get_CurAniPlayRatio();

		if (fR2 < 0.7f) {
			return;
		}
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

void CLiDailin_R::OnCollision_Enter(const COLLISION_INFO& tCollision)
{
	if (bIsCol == false && tCollision.pColCollider->Get_Layer() == ETOUI(Collision_Layer::ENEMY))
	{
		if(static_cast<CUnit*>(tCollision.pColObject)->IsUnitDead() != true)
		{
			bIsCol = true;
			m_pTempCompareTarget = tCollision.pColObject;

			Give_Damage(tCollision , true);
		}
	}
}

void CLiDailin_R::OnCollision_Stay(const COLLISION_INFO& tCollision)
{
	if (bIsAniRNextChanged == true && m_pTempCompareTarget == tCollision.pColObject)
	{
		if ((m_eRATKP & GIVE_R2) && !(m_eRATKP & END_R2))
		{
			m_eRATKP |= END_R2;
			Give_Damage(tCollision);
		}
		else if ((m_eRATKP & GIVE_R3) && !(m_eRATKP & END_R3))
		{
			m_eRATKP |= END_R3;
			Give_Damage(tCollision);
		}
		else if ((m_eRATKP & GIVE_R4) && !(m_eRATKP & END_R4))
		{
			m_eRATKP |= END_R4;
			Give_Damage(tCollision);
		}
	}
}

void CLiDailin_R::OnCollision_Exit(const COLLISION_INFO& tCollision)
{
}

void CLiDailin_R::Give_Damage(const COLLISION_INFO& tCollision, _bool bStun)
{
	CUnit* pPlayer = static_cast<CUnit*>(tCollision.pMyCollider->Get_Owner());
	_int iATKPower = pPlayer->Get_CurStat().iATKPower;
	DAMAGE_INFO tDamageInfo{};

	tDamageInfo.iDamage =
			static_cast<_int>
		(
			fADRatio * static_cast<_float>(iATKPower) +
			static_cast<_float>(m_iConsumeIntoxication) * 0.5f
		) * 5;
	tDamageInfo.pUnit = pPlayer;

	if (bStun == true)
	{
		tDamageInfo.bStun = true;
		tDamageInfo.fStunTime = 1.2f;
	}

	static_cast<CUnit*>(tCollision.pColObject)->Damaged(tDamageInfo);
	CGameInstance::GetInstance()->PlaySound_Once(ETOUI(SOUND_KEY::LIDAILIN_R_HIT));
}

CLiDailin_R* CLiDailin_R::Create()
{
	CLiDailin_R* pInstance = new CLiDailin_R();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created: CLiDailin_R");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLiDailin_R::Free()
{
	__super::Free();
}