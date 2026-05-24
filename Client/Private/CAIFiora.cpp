#include "CAIFiora.h"

#include "CGameInstance.h"

#include "CInGame_Manager.h"

#include "CAbstractPlayer.h"
#include "CInventory.h"

#include "CRapier.h"
#include "CFiora_Q_1.h"

#include "CInGameHPBar.h"

CAIFiora::CAIFiora(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAbstractMonster{ pDevice, pContext }
{
}

CAIFiora::CAIFiora(const CAIFiora& Prototype)
    : CAbstractMonster{ Prototype }
{
}

HRESULT CAIFiora::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAIFiora::Initialize(void* pArg)
{
    AIFIORA_DESC* pDesc = static_cast<AIFIORA_DESC*>(pArg);

     pDesc->tTransformDesc.vStartPos = { 6.52f, 0.f, 8.37f };

     _vector vStartPos = XMLoadFloat3(&pDesc->tTransformDesc.vStartPos);
     _vector vMoveRange = XMVectorSet(1.f, 0.f, 0.f, 0.f);
     XMStoreFloat3(&m_vMovePos[0], vStartPos + vMoveRange);
     XMStoreFloat3(&m_vMovePos[1], vStartPos - vMoveRange);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Initialize_Skill()))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    Enter_Action(AIFIORA_ACTION::WAIT);

    if (FAILED(Initialize_Stat()))
        return E_FAIL;

    m_pInGame_Manager = CInGame_Manager::GetInstance();
    Safe_AddRef(m_pInGame_Manager);

    m_fAttackRange = 1.5f;

    //m_pInvetory->Add_Item(34);
    //m_pInvetory->Add_Item(52);
    //m_pInvetory->Add_Item(26);

    m_pInGame_Manager->Add_EnemyCount();

    return S_OK;
}

void CAIFiora::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CAIFiora::Parallel_Update(_float fTimeDelta)
{
    __super::Parallel_Update(fTimeDelta);

    for (auto& pColliderCom : m_Colliders)
        pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    m_pMoveCom->Update_Move_To_Pos(fTimeDelta);
    m_pNavigationCom->Compute_OnNavigation();

    CoolTimer(fTimeDelta);
}

void CAIFiora::Update(_float fTimeDelta)
{
    Update_Action(fTimeDelta);
    Execute_Action(fTimeDelta);

    __super::Update(fTimeDelta);
}

void CAIFiora::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CAIFiora::Render()
{
    return S_OK;
}

void CAIFiora::OnCollision_Enter(const COLLISION_INFO& tCollision)
{
    DAMAGE_INFO tDamageInfo{};
    tDamageInfo.pUnit = this;

    _bool bIsPlayer = tCollision.pColCollider->Get_Layer() == ETOUI(Collision_Layer::PLAYER);

    // --
    //if (bIsPlayer == true)
    //    m_iCondition |= IS_COL_PLAYER;

    if (tCollision.pMyCollider == m_Colliders[AIFIORA_COLLIDER::AIFIORA_Q] && bIsPlayer)
    {
        CUnit* pUnit = static_cast<CUnit*>(tCollision.pColObject);
        tDamageInfo.iDamage = 10;
        pUnit->Damaged(tDamageInfo);
    }

    if (tCollision.pMyCollider == m_Colliders[AIFIORA_COLLIDER::AIFIORA_W1] && bIsPlayer ||
        tCollision.pMyCollider == m_Colliders[AIFIORA_COLLIDER::AIFIORA_W2] && bIsPlayer ||
        tCollision.pMyCollider == m_Colliders[AIFIORA_COLLIDER::AIFIORA_W3] && bIsPlayer)
    {
        auto iter = m_AttackedWEnemy.find(tCollision.pColObject);
        if (iter == m_AttackedWEnemy.end()) {
            m_AttackedWEnemy.insert(tCollision.pColObject);
            CUnit* pUnit = static_cast<CUnit*>(tCollision.pColObject);
            tDamageInfo.iDamage = 200;
            pUnit->Damaged(tDamageInfo);
        }
    }

    if (tCollision.pMyCollider == m_Colliders[AIFIORA_COLLIDER::AIFIORA_E] && bIsPlayer)
    {
        m_iCondition |= AIFIORA_CONDITION::SKILL_E_COL;
        CUnit* pUnit = static_cast<CUnit*>(tCollision.pColObject);
        tDamageInfo.iDamage = 10;
        pUnit->Damaged(tDamageInfo);
    }

    if (tCollision.pMyCollider == m_Colliders[AIFIORA_COLLIDER::AIFIORA_R] && bIsPlayer)
    {
        m_iCondition |= AIFIORA_CONDITION::SKILL_R_COL;
        CUnit* pUnit = static_cast<CUnit*>(tCollision.pColObject);
        tDamageInfo.iDamage = 10;
        pUnit->Damaged(tDamageInfo);
    }
}

void CAIFiora::OnCollision_Stay(const COLLISION_INFO& tCollision)
{
    DAMAGE_INFO tDamageInfo{};
    tDamageInfo.pUnit = this;

    _bool bIsPlayer = tCollision.pColCollider->Get_Layer() == ETOUI(Collision_Layer::PLAYER);

    //if (tCollision.pMyCollider == m_Colliders[AIFIORA_COLLIDER::AIFIORA_E] && bIsPlayer)
    //{
    //    m_iCondition |= AIFIORA_CONDITION::SKILL_E_COL;
    //    CUnit* pUnit = static_cast<CUnit*>(tCollision.pColObject);
    //    tDamageInfo.iDamage = 10;
    //    pUnit->Damaged(tDamageInfo);
    //}

    //if (tCollision.pMyCollider == m_Colliders[AIFIORA_COLLIDER::AIFIORA_R] && bIsPlayer)
    //{
    //    m_iCondition |= AIFIORA_CONDITION::SKILL_R_COL;
    //    CUnit* pUnit = static_cast<CUnit*>(tCollision.pColObject);
    //    tDamageInfo.iDamage = 10;
    //    pUnit->Damaged(tDamageInfo);
    //}
}

void CAIFiora::OnCollision_Exit(const COLLISION_INFO& tCollision)
{
    // --
    //_bool bIsPlayer = tCollision.pColCollider->Get_Layer() == ETOUI(Collision_Layer::PLAYER);

    //if (bIsPlayer == true)
    //    m_iCondition &= ~IS_COL_PLAYER;
}

void CAIFiora::Enter_Animation(CBody_Fiora::FIORA_ANI eNewAnimation)
{
    m_eCurAni = eNewAnimation;

    switch (eNewAnimation)
    {
        case CBody_Fiora::FIORA_ANI::DEATH:
        case CBody_Fiora::FIORA_ANI::DOWN_DEAD:
        case CBody_Fiora::FIORA_ANI::ATK1:
        case CBody_Fiora::FIORA_ANI::ATK2:
        case CBody_Fiora::FIORA_ANI::SKILL1:
        case CBody_Fiora::FIORA_ANI::SKILL2:
        case CBody_Fiora::FIORA_ANI::SKILL2_NEW:
        case CBody_Fiora::FIORA_ANI::SKILL3_ATK:
        case CBody_Fiora::FIORA_ANI::SKILL3_BACK:
        case CBody_Fiora::FIORA_ANI::SKILL3_FORWARD:
        case CBody_Fiora::FIORA_ANI::SKILL4_1_NEW:
        case CBody_Fiora::FIORA_ANI::SKILL4_2_NEW:
        case CBody_Fiora::FIORA_ANI::SKILL4_ATK1:
        case CBody_Fiora::FIORA_ANI::SKILL4_ATK2:
        {
            m_pBodyFiora->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
            break;
        }

        case CBody_Fiora::FIORA_ANI::RUN:
        case CBody_Fiora::FIORA_ANI::WAIT:
        {
            m_pBodyFiora->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
            break;
        }
    }
}

void CAIFiora::Update_Action(_float fTimeDelta)
{
    if (m_eCurState == AIFIORA_ACTION::DEAD)
        return;

    if (m_iMonsterCondition & MONSTER_CONDITION::CON_HPZERO)
    {
        Enter_Action(AIFIORA_ACTION::DEAD);
        return;
    }

    if (m_iMonsterCondition & MONSTER_CONDITION::CON_STUN)
    {
        Enter_Action(AIFIORA_ACTION::STUN);
    }

    switch (m_eCurState)
    {
        case STUN:
        {
            m_fStunTime -= fTimeDelta;
            if (m_fStunTime <= 0.f)
            {
                m_fStunTime = 0.f;
                m_iMonsterCondition &= ~MONSTER_CONDITION::CON_STUN;
                Enter_Action(CHASE);
            }
            break;
        }
        case WAIT:
        {
            const _float fDetectRange = 5.f;
            CUnit* pPlayer = m_pInGame_Manager->Get_Player();
            _vector vPlayerPos = pPlayer->Get_TransformCom()->Get_State(STATE::POSITION);
            if (IsInRange(vPlayerPos, fDetectRange) == true)            
            {
                Enter_Action(CHASE);
                if (m_pTargetPlayer == nullptr) {
                    m_pTargetPlayer = pPlayer;
                    Safe_AddRef(m_pTargetPlayer);
                }
            }
            break;
        }
        case CHASE:
        {
            Update_Chase(fTimeDelta);
            break;
        }

        case Q:
        {
            const _float fCurAniRatio = m_pBodyFiora->Get_ModelCom()->Get_CurAniPlayRatio();
            if (fCurAniRatio >= 0.4f)
            {
                Enter_Action(CHASE);
                pQEffect->Set_IsInactive(true);
            }
            break;
        }

        case W:
        {
            const _bool fAniFinished = m_pBodyFiora->Get_ModelCom()->IsAnimationFinished();
            if (fAniFinished == true)
            {
                m_Colliders[AIFIORA_COLLIDER::AIFIORA_W1]->Set_Active(false);
                m_Colliders[AIFIORA_COLLIDER::AIFIORA_W2]->Set_Active(false);
                m_Colliders[AIFIORA_COLLIDER::AIFIORA_W3]->Set_Active(false);
                m_AttackedWEnemy.clear();
                Enter_Action(CHASE);
            }
            break;
        }

        case E:
        {
            if (m_iCondition & AIFIORA_CONDITION::SKILL_E_COL)
            {
                m_iCondition &= ~AIFIORA_CONDITION::SKILL_E_COL;
                m_Colliders[AIFIORA_COLLIDER::AIFIORA_E]->Set_Active(false);
                Enter_Action(E_ATK);
                return;
            }

            const _float fCurAniRatio = m_pBodyFiora->Get_ModelCom()->Get_CurAniPlayRatio();
            if (fCurAniRatio >= 0.3f)
            {
                m_Colliders[AIFIORA_COLLIDER::AIFIORA_E]->Set_Active(false);
                Enter_Action(CHASE);
            }
            break;
        }

        case E_ATK:
        {
            const _bool fAniFinished = m_pBodyFiora->Get_ModelCom()->IsAnimationFinished();
            if (fAniFinished == true)
            {
                Enter_Action(CHASE);
            }
            break;
        }

        case NORMAL_ATK:
        {
            const _float fCurAniRatio = m_pBodyFiora->Get_ModelCom()->Get_CurAniPlayRatio();
            if (fCurAniRatio >= 0.7f)
            {
                m_iNormalATKCount = 0;

                _float fLength = {};
                _bool bIsTargetMissing = !(Get_TargetDistance(fLength));

                if(CanUseAnythingSkill() == true)
                {
                    if (Choose_UseSkill(fLength))
                        return;
                }

                // 공격 재실행
                if (!bIsTargetMissing && fLength <= m_fAttackRange) {
                    Enter_Animation(CBody_Fiora::FIORA_ANI::ATK1);
                    m_bIsAttackProcessed = false;
                    return;
                }

                Enter_Action(CHASE);
            }
            break;
        }

        case R:
        {

            const _bool fAniFinished = m_pBodyFiora->Get_ModelCom()->IsAnimationFinished();
            if (fAniFinished == true)
            {
                Enter_Action(CHASE);
            }
            break;
        }
    }
}

void CAIFiora::Enter_Action(AIFIORA_ACTION eNewAction)
{
    m_eCurState = eNewAction;

    if (m_eCurState != m_ePreState)
    {
        switch (m_eCurState)
        {
            case STUN:
                Enter_Animation(CBody_Fiora::FIORA_ANI::WAIT);
                break;

            case DEAD:
                Enter_Animation(CBody_Fiora::FIORA_ANI::DEATH);
                m_iMonsterCondition |= MONSTER_CONDITION::CON_DEAD;
                m_pMoveCom->Stop_Move_To_Pos();

                m_pInGameHPBar->Set_IsInactive(true);

                m_pInGame_Manager->Sub_EnemyCount();
                break;

            case WAIT:
                Enter_Animation(CBody_Fiora::FIORA_ANI::RUN);
                break;

            case CHASE:
                Enter_Animation(CBody_Fiora::FIORA_ANI::RUN);
                break;

            case Q:
                Enter_Animation(CBody_Fiora::FIORA_ANI::SKILL1);
                m_pMoveCom->Stop_Move_To_Pos();
                LookTargetDir();
                tQCool.fAccCoolDown = tQCool.fCurCoolDown;

                pQEffect->Set_IsInactive(false);
                break;

            case W:
                Enter_Animation(CBody_Fiora::FIORA_ANI::SKILL2_NEW);
                m_pMoveCom->Stop_Move_To_Pos();
                LookTargetDir();
                tWCool.fAccCoolDown = tWCool.fCurCoolDown;
                m_Colliders[AIFIORA_COLLIDER::AIFIORA_W1]->Set_Active(true);
                m_Colliders[AIFIORA_COLLIDER::AIFIORA_W2]->Set_Active(true);
                m_Colliders[AIFIORA_COLLIDER::AIFIORA_W3]->Set_Active(true);
                break;

            case E:
                Enter_Animation(CBody_Fiora::FIORA_ANI::SKILL3_FORWARD);
                m_pMoveCom->Stop_Move_To_Pos();
                LookTargetDir();
                m_Colliders[AIFIORA_COLLIDER::AIFIORA_E]->Set_Active(true);
                tECool.fAccCoolDown = tECool.fCurCoolDown;

                break;

            case E_ATK:
                Enter_Animation(CBody_Fiora::FIORA_ANI::SKILL3_ATK);
                m_pMoveCom->Stop_Move_To_Pos();

                m_iNormalATKCount = 1;
                break;

            case NORMAL_ATK:
                Enter_Animation(CBody_Fiora::FIORA_ANI::ATK1);
                m_pMoveCom->Stop_Move_To_Pos();

                m_bIsAttackProcessed = false;
                break;

            case R:
            {
                Enter_Animation(CBody_Fiora::FIORA_ANI::SKILL4_1_NEW);
                m_pMoveCom->Stop_Move_To_Pos();
                m_Colliders[AIFIORA_COLLIDER::AIFIORA_R]->Set_Active(true);

                const _uint iMaxStack = 3;
                ++tRCool.fStack;
                if (tRCool.fStack >= iMaxStack)
                {
                    tRCool.fStack = 0;
                    tRCool.fAccCoolDown = tRCool.fCurCoolDown;
                    tRCool.fAccSubCoolDown = 0.f;
                }
                else
                {
                    tRCool.fAccSubCoolDown = tRCool.fCurSubCoolDown;
                }

                m_iNormalATKCount = 1;
                break;
            }
        }

        m_ePreState = m_eCurState;
    }
}

void CAIFiora::Execute_Action(_float fTimeDelta)
{
    switch (m_eCurState)
    {
        case DEAD:
            break;

        case WAIT:
            WaitStateMove();
            break;

        case CHASE:
        {
            _float fLength{};
            Get_TargetDistance(fLength);

            if (fLength <= m_fAttackRange)
            {
                m_pMoveCom->Stop_Move_To_Pos();
                break;
            }

            _float3 vTargetPos = {};
            XMStoreFloat3(&vTargetPos, m_pTargetPlayer->Get_TransformCom()->Get_State(STATE::POSITION));
            m_pMoveCom->Move_To_Pos(vTargetPos, true);
            break;
        }

        case Q:
        {
            const _float fCurAniRatio = m_pBodyFiora->Get_ModelCom()->Get_CurAniPlayRatio();
            _bool IsActiveCollider = m_Colliders[AIFIORA_COLLIDER::AIFIORA_Q]->Get_Active();
            _bool bActive = fCurAniRatio >= 0.2f && fCurAniRatio < 0.3f;

            if (IsActiveCollider != bActive)
                m_Colliders[AIFIORA_COLLIDER::AIFIORA_Q]->Set_Active(bActive);

            break;
        }

        case W:
        {
            break;
        }

        case E:
        {
            m_pMoveCom->Go_Straight(fTimeDelta, 10.f, true);
            break;
        }

        case E_ATK:
        {
            break;
        }

        case R:
        {
            if (m_iCondition & AIFIORA_CONDITION::SKILL_R_COL)
            {
                m_iCondition &= ~AIFIORA_CONDITION::SKILL_R_COL;
                m_Colliders[AIFIORA_COLLIDER::AIFIORA_R]->Set_Active(false);
                m_pMoveCom->Stop_Move_To_Pos();
                return;
            }

            const _float fCurAniRatio = m_pBodyFiora->Get_ModelCom()->Get_CurAniPlayRatio();
            if(fCurAniRatio <= 0.005f)
                m_pMoveCom->Go_Straight(fTimeDelta, 10.f, true);

            break;
        }
        case NORMAL_ATK:
        {
            LookTargetDir();

            if (m_bIsAttackProcessed == false)
            {
                const _float fAttackTime = 0.1f;
                if (m_pBodyFiora->Get_ModelCom()->Get_AniPlayRatio(m_eCurAni) >= fAttackTime)
                {
                    if (m_pTargetPlayer != nullptr)
                    {
                        DAMAGE_INFO tDamageInfo{};
                        tDamageInfo.iDamage = 10;
                        tDamageInfo.pUnit = nullptr;
                        m_pTargetPlayer->Damaged(tDamageInfo);
                    }
                    m_bIsAttackProcessed = true;
                }
            }

        }
        break;
    }
}

HRESULT CAIFiora::Ready_Components()
{
    /* For.Com_Navigation */
    CNavigation::NAVIGATION_DESC NaviDesc;

    NaviDesc.bIsGround = false;
    NaviDesc.pTransformCom = m_pTransformCom;
    XMStoreFloat3(&NaviDesc.vObjectWorldPos, m_pTransformCom->Get_State(STATE::POSITION));

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;


    CMove::MOVE_DESC Desc{};
    Desc.pTransform = m_pTransformCom;
    Desc.pNavigationCom = m_pNavigationCom;

    Desc.fSpeed = 4.f;

    /* Com_Move */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_Move"),
        TEXT("Com_Move"), reinterpret_cast<CComponent**>(&m_pMoveCom), &Desc)))
        return E_FAIL;


    CCollider* pColliderCom;

    /* For.Com_Collider_AABB */
    CBounding_AABB::BOUNDING_AABB_DESC  AABBDesc{ };
    AABBDesc.vSize = _float3(0.7f, 1.5f, 0.7f);
    AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&pColliderCom), &AABBDesc)))
        return E_FAIL;

    pColliderCom->Set_Owner(this);
    m_Colliders.push_back(pColliderCom);

    m_pGameInstance->Add_Collider(pColliderCom);
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::ENEMY));
    pColliderCom->Set_Mask(ETOUI(Collision_Layer::PLAYER));

    m_pGameInstance->Add_PickingCollider(pColliderCom);

    tLocalMinMax = {};
    Cal_LocalMinMaxAABB(tLocalMinMax, AABBDesc.vCenter, AABBDesc.vSize);

#pragma region Q
    /* For.Com_Collider_Q */
    CBounding_OBB::BOUNDING_OBB_DESC  OBB_Q_Desc{ };
    OBB_Q_Desc.vSize = _float3(0.4f, 1.0f, m_SkillRange[ETOUI(SKILL_SLOT::Q)]);
    OBB_Q_Desc.vCenter = _float3(0.f, fColliderCenterY, OBB_Q_Desc.vSize.z * 0.5f);

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_Q"), reinterpret_cast<CComponent**>(&pColliderCom), &OBB_Q_Desc)))
        return E_FAIL;

    pColliderCom->Set_Owner(this);
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::ENEMY_SKILL));
    pColliderCom->Set_Mask(ETOUI(Collision_Layer::PLAYER));
    pColliderCom->Set_Active(false);

    m_Colliders.push_back(pColliderCom);
    m_pGameInstance->Add_Collider(pColliderCom);
#pragma endregion

#pragma region W
    /* For.Com_Collider_W1 */
    CBounding_Frustum::BOUNDING_FRUSTUM_DESC  FrustumDesc{ };
    FrustumDesc.vOrigin = _float3(0.f, fColliderCenterY, 0.f);
    FrustumDesc.vRadians = _float3(0.f, 0.f, 0.f);
    FrustumDesc.fNear = 0.f;
    FrustumDesc.fFar = m_SkillRange[ETOUI(SKILL_SLOT::W)];

    // tan으로 넣어줘야 함.
    FrustumDesc.fRightSlope = tanf(XMConvertToRadians(30.f));  // 오른쪽 각도
    FrustumDesc.fLeftSlope = -FrustumDesc.fRightSlope;         // 왼쪽 각도

    FrustumDesc.fTopSlope = tanf(XMConvertToRadians(10.f)); // 위 각도
    FrustumDesc.fBottomSlope = -FrustumDesc.fTopSlope;      // 아래 각도

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Frustum"),
        TEXT("Com_Collider_W1"), reinterpret_cast<CComponent**>(&pColliderCom), &FrustumDesc)))
        return E_FAIL;

    pColliderCom->Set_Owner(this);
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::ENEMY_SKILL));
    pColliderCom->Set_Mask(ETOUI(Collision_Layer::PLAYER));
    pColliderCom->Set_Active(false);

    m_Colliders.push_back(pColliderCom);
    m_pGameInstance->Add_Collider(pColliderCom);

    /* For.Com_Collider_W2 */
    FrustumDesc.vRadians = _float3(0.f, XMConvertToRadians(60.f), 0.f);

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Frustum"),
        TEXT("Com_Collider_W2"), reinterpret_cast<CComponent**>(&pColliderCom), &FrustumDesc)))
        return E_FAIL;

    pColliderCom->Set_Owner(this);
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::ENEMY_SKILL));
    pColliderCom->Set_Mask(ETOUI(Collision_Layer::PLAYER));
    pColliderCom->Set_Active(false);

    m_Colliders.push_back(pColliderCom);
    m_pGameInstance->Add_Collider(pColliderCom);

    /* For.Com_Collider_W3 */
    FrustumDesc.vRadians = _float3(0.f, XMConvertToRadians(-60.f), 0.f);

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_Frustum"),
        TEXT("Com_Collider_W3"), reinterpret_cast<CComponent**>(&pColliderCom), &FrustumDesc)))
        return E_FAIL;

    pColliderCom->Set_Owner(this);
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::ENEMY_SKILL));
    pColliderCom->Set_Mask(ETOUI(Collision_Layer::PLAYER));
    pColliderCom->Set_Active(false);

    m_Colliders.push_back(pColliderCom);
    m_pGameInstance->Add_Collider(pColliderCom);
#pragma endregion

#pragma region E
    /* For.Com_Collider_E */
    CBounding_OBB::BOUNDING_OBB_DESC  OBB_E_Desc{ };
    OBB_E_Desc.vSize = _float3(0.7f, 1.0f, 1.f);
    OBB_E_Desc.vCenter = _float3(0.f, fColliderCenterY, OBB_E_Desc.vSize.z * 0.5f);

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_E"), reinterpret_cast<CComponent**>(&pColliderCom), &OBB_E_Desc)))
        return E_FAIL;

    pColliderCom->Set_Owner(this);
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::ENEMY_SKILL));
    pColliderCom->Set_Mask(ETOUI(Collision_Layer::PLAYER));
    pColliderCom->Set_CanMousePicking(false);
    pColliderCom->Set_Active(false);
    
    m_pGameInstance->Add_Collider(pColliderCom);
    m_Colliders.push_back(pColliderCom);
#pragma endregion

#pragma region R
    /* For.Com_Collider_R */
    CBounding_OBB::BOUNDING_OBB_DESC  OBB_R_Desc{ };
    OBB_R_Desc.vSize = _float3(0.7f, 1.0f, m_SkillRange[ETOUI(SKILL_SLOT::R)]);
    OBB_R_Desc.vCenter = _float3(0.f, fColliderCenterY, OBB_R_Desc.vSize.z * 0.5f);

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_R"), reinterpret_cast<CComponent**>(&pColliderCom), &OBB_R_Desc)))
        return E_FAIL;

    pColliderCom->Set_Owner(this);
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::ENEMY_SKILL));
    pColliderCom->Set_Mask(ETOUI(Collision_Layer::PLAYER));
    pColliderCom->Set_CanMousePicking(false);
    pColliderCom->Set_Active(false);

    m_pGameInstance->Add_Collider(pColliderCom);
    m_Colliders.push_back(pColliderCom);
#pragma endregion

    return S_OK;
}

HRESULT CAIFiora::Ready_PartObjects()
{
    // Body
    CBody_Fiora::BODY_FIORA_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Fiora"),
        TEXT("Body"), &BodyDesc)))
        return E_FAIL;

    m_pBodyFiora = dynamic_cast<CBody_Fiora*>(m_PartObjects[TEXT("Body")]);
    Safe_AddRef(m_pBodyFiora);

    // Rapier
    CRapier::RAPIER_DESC RapierDesc{};
    RapierDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    RapierDesc.pSocketBoneMatrix = m_pBodyFiora->Get_BoneMatrixPtr("Equip_R");

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Rapier"),
        TEXT("Rapier"), &RapierDesc)))
        return E_FAIL;

    // Q_Effect_1
    CFiora_Q_1::FIORA_Q_1_DESC QEffectDesc{};
    QEffectDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CFiora_Q_1"),
        TEXT("Q_Effect_1"), &QEffectDesc)))
        return E_FAIL;

    pQEffect = dynamic_cast<CFiora_Q_1*>(m_PartObjects[TEXT("Q_Effect_1")]);
    Safe_AddRef(pQEffect);

    CInGameHPBar::INGAMEHPBAR_DESC HPBarDesc{};
    HPBarDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    HPBarDesc.fHeight = 2.5f;

    HPBarDesc.pMaxHp = &m_tFinalStat.iHP;
    HPBarDesc.pCurHp = &m_tCurStat.iHP;
    HPBarDesc.vHPColor = COLOR_TO_FLOAT(212, 22, 56);

    HPBarDesc.bNoMp = true;

    HPBarDesc.eUnitType = CInGameHPBar::CHAR;

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_InGameHPBar"),
        TEXT("HPBar"), &HPBarDesc)))
        return E_FAIL;

    m_pInGameHPBar = dynamic_cast<CInGameHPBar*>(m_PartObjects[TEXT("HPBar")]);
    Safe_AddRef(m_pInGameHPBar);

    return S_OK;
}

HRESULT CAIFiora::Bind_ShaderResources()
{
    return S_OK;
}

HRESULT CAIFiora::Initialize_Skill()
{
    tQCool.fMaxCoolDown = tQCool.fCurCoolDown = 4.f;
    m_SkillRange.push_back(3.f);

    tWCool.fMaxCoolDown = tWCool.fCurCoolDown = 6.f;
    m_SkillRange.push_back(2.f);

    tECool.fMaxCoolDown = tECool.fCurCoolDown = 9.f;
    m_SkillRange.push_back(4.f);

    tRCool.fMaxCoolDown = tRCool.fCurCoolDown = 60.f;
    tRCool.fMaxSubCoolDown = tRCool.fCurSubCoolDown = 10.f;
    m_SkillRange.push_back(2.f);

    return S_OK;
}

HRESULT CAIFiora::Initialize_Stat()
{
    SetStat(m_tBaseStat, 2, 0, 2650, 100, 0.f, 0.f, 62, 0, 0, 0, 18, 0.8f, 0, 0, 3.76f);
    SetStat(m_tCurStat, 2, 999, 2650, 100, 0.f, 0.f, 62, 0, 0, 0, 18, 0.8f, 0, 0, 3.76f);

    SetFinalStat();

    m_eMPType = MAINGAUGE_TYPE::MP;

    return S_OK;
}

void CAIFiora::WaitStateMove()
{
    m_bIsArrive =  !(m_pMoveCom->IsMove());

    if (m_bIsArrive == true) {
        m_iMovePosIndex %= 2;
        m_pMoveCom->Move_To_Pos(m_vMovePos[m_iMovePosIndex++], true);
    }
}

void CAIFiora::CoolTimer(_float fTimeDelta)
{
    // Q
    tQCool.Update_Cool(fTimeDelta);

    // W
    tWCool.Update_Cool(fTimeDelta);

    // E
    tECool.Update_Cool(fTimeDelta);

    // R
    tRCool.Update_Cool(fTimeDelta);
    tRCool.Update_SubCool(fTimeDelta);
}

_bool CAIFiora::CanUseSkill(const SKILL_SLOT eType)
{
    switch (eType)
    {
        case SKILL_SLOT::Q:
        {
            if (tQCool.fAccCoolDown <= 0.f)
                return true;
            break;
        }
        case SKILL_SLOT::W:
        {
            if (tWCool.fAccCoolDown <= 0.f)
                return true;
            break;
        }
        case SKILL_SLOT::E:
        {
            if (tECool.fAccCoolDown <= 0.f)
                return true;
            break;
        }
        case SKILL_SLOT::R:
        {
            if (tRCool.fAccCoolDown <= 0.f)
                return true;
            break;
        }
    }

    return false;
}

_bool CAIFiora::CanUseAnythingSkill()
{
    return tQCool.fAccCoolDown <= 0.f || tWCool.fAccCoolDown <= 0.f
        || tECool.fAccCoolDown <= 0.f || tRCool.fAccCoolDown <= 0.f;
}

_bool CAIFiora::Update_Chase(_float fTimeDelta)
{
    _float fLength = {};
    Get_TargetDistance(fLength);

    if (m_iNormalATKCount == 0 && Choose_UseSkill(fLength) == true)
        return true;

    if (fLength <= m_fAttackRange) {
        Enter_Action(NORMAL_ATK);
        return true;
    }

    return false;
}

_bool CAIFiora::Choose_UseSkill(_float fLength)
{
    // Q 사용 가능하면 무조건 Q부터
    if (CanUseSkill(SKILL_SLOT::Q) == true)
    {
        const _float fQRange = m_SkillRange[ETOUI(SKILL_SLOT::Q)];
        if (fLength >= fQRange * 0.7f)
            return false;

        Enter_Action(Q);
        return true;
    }

    if (CanUseSkill(SKILL_SLOT::E) == true)
    {
        const _float fERange = m_SkillRange[ETOUI(SKILL_SLOT::E)];
        if (fLength <= fERange)
        {
            Enter_Action(E);
            return true;
        }
    }
    else if (CanUseSkill(SKILL_SLOT::R) == true)
    {
        const _float fRRange = m_SkillRange[ETOUI(SKILL_SLOT::R)];
        if (fLength <= fRRange)
        {
            Enter_Action(R);
            return true;
        }
    }
    else if (CanUseSkill(SKILL_SLOT::W) == true)
    {
        const _float fRRange = m_SkillRange[ETOUI(SKILL_SLOT::W)];
        if (fLength <= fRRange)
        {
            Enter_Action(W);
            return true;
        }
    }

    return false;
}

void CAIFiora::LookTargetDir()
{
    if (m_pTargetPlayer == nullptr)
        return;

    _vector vDir = XMVector3Normalize(m_pTargetPlayer->Get_TransformCom()->Get_State(STATE::POSITION)
        - m_pTransformCom->Get_State(STATE::POSITION));

    m_pTransformCom->LookDir(vDir);
}

_bool CAIFiora::Get_TargetDistance(_float& Length)
{
    if (m_pTargetPlayer == nullptr)
        return false;

    Length = XMVectorGetX(XMVector3Length(m_pTargetPlayer->Get_TransformCom()->Get_State(STATE::POSITION)
        - m_pTransformCom->Get_State(STATE::POSITION)));

    return true;
}

CAIFiora* CAIFiora::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAIFiora* pInstance = new CAIFiora(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CAIFiora");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAIFiora::Clone(void* pArg)
{
    CAIFiora* pInstance = new CAIFiora(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CAIFiora");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAIFiora::Free()
{
    Safe_Release(m_pInGame_Manager);

    Safe_Release(pQEffect);
    Safe_Release(m_pBodyFiora);

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pMoveCom);

    __super::Free();
}