#include "CLiDailin_Q.h"

#include "CGameInstance.h"

#include "CLiDailin.h"
#include "CBody_Player.h"
#include "CWeapon.h"

CLiDailin_Q::CLiDailin_Q()
{
}

HRESULT CLiDailin_Q::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	m_iMaxLevel = 5;

	return S_OK;
}

void CLiDailin_Q::Enter(CLiDailin* pPlayer)
{
	// Cool
	STACK_COOL_INFO* pQCoolInfo = static_cast<STACK_COOL_INFO*>(pPlayer->Get_CoolInfo(SKILL_SLOT::Q));
	pQCoolInfo->fAccSubCoolDown = pQCoolInfo->fCurSubCoolDown;

	// Ani
	_uint iBodyPhase{};
	_uint iWeaponPhase{};

	switch (pQCoolInfo->fStack) {
		case 0:
		{
			iBodyPhase = static_cast<_uint>(LiDailin_Ani::Ani_Q1);
			pPlayer->Set_CurAni(LiDailin_Ani::Ani_Q1);
			pPlayer->Set_MovementAniBlock(true);
			iWeaponPhase = static_cast<_uint>(Nunchaku_Ani::Q1_WP);

			// Intoxication
			const _int iConsumeIntoxication = 40;
			if (pPlayer->Get_CurStat().iMP >= iConsumeIntoxication)
			{
				pPlayer->AddMP(-iConsumeIntoxication);
				m_bEnhanced = true;
			}
			else
			{
				m_bEnhanced = false;
			}

			break;
		}
		case 1:
		{
			iBodyPhase = static_cast<_uint>(LiDailin_Ani::Ani_Q2);
			pPlayer->Set_CurAni(LiDailin_Ani::Ani_Q2);
			pPlayer->Set_MovementAniBlock(true);
			iWeaponPhase = static_cast<_uint>(Nunchaku_Ani::Q2_WP);
			break;
		}
		case 2:
		{
			iBodyPhase = static_cast<_uint>(LiDailin_Ani::Ani_Q3);
			pPlayer->Set_CurAni(LiDailin_Ani::Ani_Q3);
			pPlayer->Set_MovementAniBlock(true);
			iWeaponPhase = static_cast<_uint>(Nunchaku_Ani::Q3_WP);
			break;
		}
	}

	// 강화 Q 이면 기본 공격 강화
	if (m_bEnhanced == true)
		pPlayer->Set_EnhancedBasicATK(true);

	pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(iBodyPhase, false);
	pPlayer->Get_Weapon()->Get_ModelCom()->Set_AnimationIndex(iWeaponPhase, false);

	pQCoolInfo->fStack = (++(pQCoolInfo->fStack)) % 3;

	if ((pQCoolInfo->fStack) == 0) {
		pQCoolInfo->fAccCoolDown = pQCoolInfo->fCurCoolDown;
		pQCoolInfo->fAccSubCoolDown = 0.f;
	}

	// Ani Speed
	pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AniSpeed(iBodyPhase, 2.f);
	pPlayer->Get_Weapon()->Get_ModelCom()->Set_AniSpeed(iWeaponPhase, 2.f);

	// 이동
	pPlayer->Set_MoveBlock(true);
	pPlayer->Set_WaitMovementState(L"Idle");

	// 방향
	pPlayer->Look_MouseDir();
}

void CLiDailin_Q::Update(CLiDailin* pPlayer, _float fTimeDelta)
{
	const CMyModel* pModel = pPlayer->Get_BodyPlayer()->Get_ModelCom();
	const _float fAniRatio = pModel->Get_CurAniPlayRatio();

	STACK_COOL_INFO* pQCoolInfo = static_cast<STACK_COOL_INFO*>(pPlayer->Get_CoolInfo(SKILL_SLOT::Q));
	// 0인 경우는 3타
	if ((pQCoolInfo->fStack) == 0 && fAniRatio <= 0.283f)
	{
		static_cast<CMove*>(pPlayer->Find_Component(TEXT("Com_Move")))->Go_Straight(fTimeDelta, 10.f, true);
	}
	else if((pQCoolInfo->fStack) != 0){
		static_cast<CMove*>(pPlayer->Find_Component(TEXT("Com_Move")))->Go_Straight(fTimeDelta, 4.f, true);
	}

	if (fAniRatio >= 0.f && fAniRatio <= 0.8f)
	{
		pPlayer->Get_Collider(CLiDailin::LIDAILIN_COLLIDER::LIDAILIN_Q)->Set_Active(true);
	}
	else
	{
		pPlayer->Get_Collider(CLiDailin::LIDAILIN_COLLIDER::LIDAILIN_Q)->Set_Active(false);
	}

	if (pPlayer->Get_BodyPlayer()->Get_ModelCom()->IsAnimationFinished() == true) {
		pPlayer->Set_ActionEnd();
	}
}

void CLiDailin_Q::Exit(CLiDailin* pPlayer)
{
	m_AttackedObj.clear();

	pPlayer->Set_CurAni(LiDailin_Ani::Ani_None);
	pPlayer->Set_MovementAniBlock(false);
	pPlayer->Set_MoveBlock(false);

	pPlayer->Get_Collider(CLiDailin::LIDAILIN_COLLIDER::LIDAILIN_Q)->Set_Active(false);
}

void CLiDailin_Q::HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command)
{
	if (pPlayer->Get_BodyPlayer()->Get_ModelCom()->IsAnimationFinished() == false) {
		return;
	}

	pPlayer->DefaultProcess_ActionCommand(eAction_Command);
}

void CLiDailin_Q::OnCollision_Enter(const COLLISION_INFO& tCollision)
{
	if (tCollision.pColCollider->Get_Layer() == ETOUI(Collision_Layer::MONSTER) ||
		tCollision.pColCollider->Get_Layer() == ETOUI(Collision_Layer::ENEMY))
	{
		auto iter = m_AttackedObj.insert(tCollision.pColObject);

		if (iter.second == false)
			return;

		DAMAGE_INFO tDamageInfo = { static_cast<CUnit*>(tCollision.pMyCollider->Get_Owner()), 50 };
		static_cast<CUnit*>(tCollision.pColObject)->Damaged(tDamageInfo);
	}
}

void CLiDailin_Q::OnCollision_Stay(const COLLISION_INFO& tCollision)
{
}

void CLiDailin_Q::OnCollision_Exit(const COLLISION_INFO& tCollision)
{
}

CLiDailin_Q* CLiDailin_Q::Create()
{
	CLiDailin_Q* pInstance = new CLiDailin_Q();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created: CLiDailin_Q");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLiDailin_Q::Free()
{
	__super::Free();
}
