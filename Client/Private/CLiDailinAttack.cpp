#include "CLiDailinAttack.h"

#include "CGameInstance.h"

#include "CLiDailin.h"
#include "CBody_Player.h"
#include "CWeapon.h"

#include "CAbstractMonster.h"

CLiDailinAttack::CLiDailinAttack()
{
}

void CLiDailinAttack::Enter(CLiDailin* pPlayer)
{	
	pPlayer->Set_CanMoveCancle(true);

	if (IsInAttackRange(pPlayer))
		Attack(pPlayer);
	else 
		Chase(pPlayer);

	// 현재 공격 중인 몬스터와 다른 타겟인지 비교용
	const ACTION_COMMAND& tAction_Command = pPlayer->Get_CurActionCommand();
	m_pTargetObject = tAction_Command.pGameObject;
}

void CLiDailinAttack::Update(CLiDailin* pPlayer, _float fTimeDelta)
{
	// 추적 중일 때만 검사
	if (m_bIsChasing == true)
	{
		if (IsInAttackRange(pPlayer))
		{
			Attack(pPlayer);
		}
		else
		{
			Chase(pPlayer);
			return;
		}
	}

	// 몬스터 포인터 구하기
	const ACTION_COMMAND& tAction_Command = pPlayer->Get_CurActionCommand();
	CAbstractMonster* pMonster = dynamic_cast<CAbstractMonster*>(tAction_Command.pGameObject);
	if (pMonster == nullptr)
		MSG_BOX("Bug Point 4: CLiDailinAttack");

	// 몬스터 바라보게 회전
	CTransform* pPlayerTransformCom = pPlayer->Get_TransformCom();
	_vector vDir = pMonster->Get_TransformCom()->Get_State(STATE::POSITION) - pPlayerTransformCom->Get_State(STATE::POSITION);
	vDir = XMVector3Normalize(vDir);
	pPlayerTransformCom->TurnDirDefaultY(vDir, fTimeDelta, 1080.f);

	ApplyDamage_First(pPlayer);
	ApplyDamage_Second(pPlayer);

	// 내 모델 컴포넌트
	const CMyModel* pModel = pPlayer->Get_BodyPlayer()->Get_ModelCom();

	// 무적 상태가 아니면서 다음 기본 공격 할 때가 되면 다시 공격
	if (pMonster->IsUnitDead() == false && pModel->Get_CurAniPlayRatio() >= 0.75f)
		pPlayer->Set_WaitActionState(L"CLiDailinAttack");

	// 애니메이션 끝나면 종료
	if (pPlayer->Get_BodyPlayer()->Get_ModelCom()->IsAnimationFinished() == true)
		pPlayer->Set_ActionEnd();
}

void CLiDailinAttack::Exit(CLiDailin* pPlayer)
{
	m_bIsAttackProcessed_First = false;
	m_bIsAttackProcessed_Second = false;

	pPlayer->Set_CanMoveCancle(false);

	pPlayer->Set_CurAni(LiDailin_Ani::Ani_None);
	pPlayer->Set_MovementAniBlock(false);

	m_pTargetObject = nullptr;
}

void CLiDailinAttack::HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command)
{
	switch (eAction_Command.eCommandType) {
		case ACTION_COMMAND_TYPE::ATTACK:
		{
			if (eAction_Command.pGameObject == m_pTargetObject)
				return;
			
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

HRESULT CLiDailinAttack::Initialize(_float fAttackRange)
{
	m_fAttackRange = fAttackRange;

	return S_OK;
}

void CLiDailinAttack::Attack(CLiDailin* pPlayer)
{
	_int iRandom = rand() % 2;

	// Ani
	if (pPlayer->Get_EnhancedBasicATK() == true)
	{
		if (iRandom == 0) {
			m_iCurBodyAni = ETOUI(LiDailin_Ani::Ani_ATK_1P);
			m_iCurWeaponAni = ETOUI(Nunchaku_Ani::ATK_1P_WP);
		}
		else
		{
			m_iCurBodyAni = ETOUI(LiDailin_Ani::Ani_ATK_2P);
			m_iCurWeaponAni = ETOUI(Nunchaku_Ani::ATK_2P_WP);
		}
		pPlayer->Set_EnhancedBasicATK(false);
	}
	else
	{
		m_bIsAttackProcessed_Second = true;
		if (iRandom == 0) {
			m_iCurBodyAni = ETOUI(LiDailin_Ani::Ani_ATK_1);
			m_iCurWeaponAni = ETOUI(Nunchaku_Ani::ATK_1_WP);
		}
		else
		{
			m_iCurBodyAni = ETOUI(LiDailin_Ani::Ani_ATK_2);
			m_iCurWeaponAni = ETOUI(Nunchaku_Ani::ATK_2_WP);
		}
	}

	pPlayer->Get_BodyPlayer()->Get_ModelCom()->Set_AnimationIndex(m_iCurBodyAni, false);
	pPlayer->Set_CurAni(static_cast<LiDailin_Ani>(m_iCurBodyAni));
	pPlayer->Get_Weapon()->Get_ModelCom()->Set_AnimationIndex(m_iCurWeaponAni, false);

	pPlayer->Set_MovementAniBlock(true);
	pPlayer->Set_WaitMovementState(L"Idle");
	m_bIsChasing = false;

	CGameInstance::GetInstance()->PlaySound_Once(ETOUI(SOUND_KEY::LIDAILIN_NORMAL_ATK_WIND_1));
}

void CLiDailinAttack::Chase(CLiDailin* pPlayer)
{
	const ACTION_COMMAND& tAction_Command = pPlayer->Get_CurActionCommand();
	CTransform* pMonsterTransform = dynamic_cast<CTransform*>(tAction_Command.pGameObject->Find_Component(g_strTransformTag));
	if (pMonsterTransform == nullptr)
		MSG_BOX("Bug Point 3: CLiDailinAttack");

	MOVEMENT_COMMAND tMovement_Command{};
	tMovement_Command.eCommandType = MOVEMENT_COMMAND_TYPE::MOVE;
	XMStoreFloat3(&tMovement_Command.vTargetPos, pMonsterTransform->Get_State(STATE::POSITION));
	
	pPlayer->Process_MovementCommand(tMovement_Command);

	pPlayer->Set_MovementAniBlock(false);
	m_bIsChasing = true;
}

_bool CLiDailinAttack::IsInAttackRange(CLiDailin* pPlayer)
{
	const ACTION_COMMAND& tAction_Command = pPlayer->Get_CurActionCommand();

	CAbstractMonster* pMonster = dynamic_cast<CAbstractMonster*>(tAction_Command.pGameObject);
	if (pMonster == nullptr)
		MSG_BOX("Bug Point 1: CLiDailinAttack");

	CTransform* pMonsterTransform = dynamic_cast<CTransform*>(pMonster->Find_Component(g_strTransformTag));
	if (pMonsterTransform == nullptr)
		MSG_BOX("Bug Point 2: CLiDailinAttack");

	return pPlayer->IsInRange(pMonsterTransform->Get_State(STATE::POSITION), m_fAttackRange);
}

void CLiDailinAttack::ApplyDamage_First(CLiDailin* pPlayer)
{
	if (m_bIsAttackProcessed_First == true)
		return;	

	CMyModel* pModel = pPlayer->Get_BodyPlayer()->Get_ModelCom();
	_float fRatio = pModel->Get_AniPlayRatio(m_iCurBodyAni);

	_float fAttackRatio{};

	if (m_iCurBodyAni == ETOUI(LiDailin_Ani::Ani_ATK_1))		fAttackRatio = 0.096f;
	else if (m_iCurBodyAni == ETOUI(LiDailin_Ani::Ani_ATK_2))	fAttackRatio = 0.115f;
	else if (m_iCurBodyAni == ETOUI(LiDailin_Ani::Ani_ATK_1P))	fAttackRatio = 0.115f;
	else if (m_iCurBodyAni == ETOUI(LiDailin_Ani::Ani_ATK_2P))	fAttackRatio = 0.115f;

	if (fRatio >= fAttackRatio)
	{
		const ACTION_COMMAND& tAction_Command = pPlayer->Get_CurActionCommand();
		CAbstractMonster* pMonster = dynamic_cast<CAbstractMonster*>(tAction_Command.pGameObject);
		if (pMonster == nullptr)
			MSG_BOX("Bug Point 5: CLiDailinAttack");

		DAMAGE_INFO tDamageInfo = { pPlayer , 100 };
		pMonster->Damaged(tDamageInfo);

		if (m_iCurBodyAni == ETOUI(LiDailin_Ani::Ani_ATK_1) || m_iCurBodyAni == ETOUI(LiDailin_Ani::Ani_ATK_2))		
			CGameInstance::GetInstance()->PlaySound_Once(ETOUI(SOUND_KEY::LIDAILIN_NORMAL_ATK));
		else if (m_iCurBodyAni == ETOUI(LiDailin_Ani::Ani_ATK_1P) || m_iCurBodyAni == ETOUI(LiDailin_Ani::Ani_ATK_2P))
			CGameInstance::GetInstance()->PlaySound_Once(ETOUI(SOUND_KEY::LIDAILIN_NORMAL_ATK_P));

		m_bIsAttackProcessed_First = true;
	}
}

void CLiDailinAttack::ApplyDamage_Second(CLiDailin* pPlayer)
{
	if (m_bIsAttackProcessed_Second == true)
		return;

	CMyModel* pModel = pPlayer->Get_BodyPlayer()->Get_ModelCom();
	_float fRatio = pModel->Get_AniPlayRatio(m_iCurBodyAni);

	_float fAttackRatio{};

	if (m_iCurBodyAni == ETOUI(LiDailin_Ani::Ani_ATK_1P))		fAttackRatio = 0.346f;
	else if (m_iCurBodyAni == ETOUI(LiDailin_Ani::Ani_ATK_2P))	fAttackRatio = 0.336f;

	if (fRatio >= fAttackRatio)
	{
		const ACTION_COMMAND& tAction_Command = pPlayer->Get_CurActionCommand();
		CAbstractMonster* pMonster = dynamic_cast<CAbstractMonster*>(tAction_Command.pGameObject);
		if (pMonster == nullptr)
			MSG_BOX("Bug Point 6: CLiDailinAttack");

		DAMAGE_INFO tDamageInfo = { pPlayer, 100 };
		pMonster->Damaged(tDamageInfo);
		CGameInstance::GetInstance()->PlaySound_Once(ETOUI(SOUND_KEY::LIDAILIN_NORMAL_ATK_P));
		m_bIsAttackProcessed_Second = true;
	}
}

CLiDailinAttack* CLiDailinAttack::Create(_float fAttackRange)
{
	CLiDailinAttack* pInstance = new CLiDailinAttack();

	if (FAILED(pInstance->Initialize(fAttackRange)))
	{
		MSG_BOX("Failed to Created: CLiDailinAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLiDailinAttack::Free()
{
	__super::Free();
}