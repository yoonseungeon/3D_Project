#include "CDog.h"

#include "CGameInstance.h"

#include "CInGame_Manager.h"

#include "CAbstractPlayer.h"
#include "CInventory.h"

#include "CInGameHPBar.h"

CDog::CDog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAbstractMonster{ pDevice, pContext }
{
}

CDog::CDog(const CDog& Prototype)
    : CAbstractMonster{ Prototype }
{
}

HRESULT CDog::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDog::Initialize(void* pArg)
{
    DOG_DESC* pDesc = static_cast<DOG_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    Enter_Action(DOG_ACTION::APPEAR);

    if (FAILED(Initialize_Stat()))
        return E_FAIL;

    m_fAttackRange = 1.5f;

    m_pInvetory->Add_Item(34);
    m_pInvetory->Add_Item(52);
    m_pInvetory->Add_Item(6);

    return S_OK;
}

void CDog::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CDog::Parallel_Update(_float fTimeDelta)
{
    __super::Parallel_Update(fTimeDelta);

    for (auto& pColliderCom : m_Colliders)
        pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    m_pMoveCom->Update_Move_To_Pos(fTimeDelta);
    m_pNavigationCom->Compute_OnNavigation();
}

void CDog::Update(_float fTimeDelta)
{
    Update_Action(fTimeDelta);
    Execute_Action(fTimeDelta);

    __super::Update(fTimeDelta);
}

void CDog::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CDog::Render()
{
    return S_OK;
}

void CDog::Enter_Animation(CBody_Dog::DOG_ANI eNewAnimation)
{
    m_eCurAni = eNewAnimation;
    switch (eNewAnimation) {
    case CBody_Dog::APPEAR_WAIT:
    {
        m_pBodyDog->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
        break;
    }
    case CBody_Dog::ATK1:
    {
        m_pBodyDog->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Dog::ATK2:
    {
        m_pBodyDog->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Dog::DANCE:
    {
        m_pBodyDog->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Dog::DEATH:
    {
        m_pBodyDog->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Dog::DYING:
    {
        m_pBodyDog->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
        break;
    }
    case CBody_Dog::RUN:
    {
        m_pBodyDog->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
        break;
    }
    case CBody_Dog::SLEEP_START:
    {
        m_pBodyDog->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Dog::WAIT:
    {
        m_pBodyDog->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
        break;
    }
    case CBody_Dog::APPEAR:
    {
        m_pBodyDog->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Dog::BEWARE_END:
    {
        m_pBodyDog->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Dog::BEWARE_LOOP:
    {
        m_pBodyDog->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
        break;
    }
    case CBody_Dog::BEWARE_LOOP_WAIT:
    {
        m_pBodyDog->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Dog::BEWARE_START:
    {
        m_pBodyDog->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Dog::ENDBATTLE:
    {
        m_pBodyDog->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Dog::SLEEP:
    {
        m_pBodyDog->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
        break;
    }
    case CBody_Dog::WAKE:
    {
        m_pBodyDog->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Dog::DYING_1:
    {
        m_pBodyDog->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    }
}

void CDog::Update_Action(_float fTimeDelta)
{
    if (m_eCurState == DOG_ACTION::DYING)
        return;

    if (m_eCurState == DOG_ACTION::DEATH)
    {
        if (m_pBodyDog->Get_ModelCom()->IsAnimationFinished() == true)
        {
            Enter_Action(DOG_ACTION::DYING);
            return;
        }
    }
    else if (m_iMonsterCondition & MONSTER_CONDITION::CON_HPZERO)
    {
        Enter_Action(DOG_ACTION::DEATH);
    }

    if (m_pTargetPlayer != nullptr && !(m_iMonsterCondition & CON_ATTACK))
    {
        Run_OR_ATTACK();
        m_iMonsterCondition |= CON_ATTACK;
    }

    switch (m_eCurState)
    {
    case WAIT:
    {
        // 밤만 진입
        if (PlayerIsInRange(m_fBewareRange) == true)
            Enter_Action(DOG_ACTION::BEWARE_START);

        if (m_pInGame_Manager->IsDay() == true)
            Enter_Action(DOG_ACTION::SLEEP_START);
        break;
    }

    case SLEEP_START:
    {
        if (m_pBodyDog->Get_ModelCom()->IsAnimationFinished() == true)
            Enter_Action(DOG_ACTION::SLEEP);

        if (PlayerIsInRange(m_fBewareRange) == true)
            Enter_Action(DOG_ACTION::SLEEP_END);
        break;
    }

    case SLEEP:
    {
        if (PlayerIsInRange(m_fBewareRange) == true)
            Enter_Action(DOG_ACTION::SLEEP_END);

        if (m_pInGame_Manager->IsDay() == false)
            Enter_Action(DOG_ACTION::WAIT);
        break;
    }

    case SLEEP_END:
    {
        if (m_pBodyDog->Get_ModelCom()->IsAnimationFinished() == true)
            Enter_Action(DOG_ACTION::BEWARE_START);
        break;
    }

    case RUN:
    {
        if (IsNearSpawnPoint(m_fBewareRange * 1.5f) == false)
        {
            Enter_Action(DOG_ACTION::ENDBATTLE);
            return;
        }

        if (PlayerIsInRange(m_fAttackRange) == true)
            Enter_Action(DOG_ACTION::ATK);

        break;
    }

    case ENDBATTLE:
    {
        if (m_pBodyDog->Get_ModelCom()->IsAnimationFinished() == true)
            Enter_Action(DOG_ACTION::RETURN);
        break;
    }

    case DANCE:
    {
        break;
    }

    case BEWARE_START:
    {
        if (m_pBodyDog->Get_ModelCom()->IsAnimationFinished() == true)
            Enter_Action(DOG_ACTION::BEWARE_LOOP);
        break;
    }

    case BEWARE_LOOP:
    {
        if (PlayerIsInRange(m_fBewareRange) == false)
            if (m_pInGame_Manager->IsDay())
            {
                Enter_Action(DOG_ACTION::SLEEP_START);
            }
            else
            {
                Enter_Action(DOG_ACTION::BEWARE_END);
            }
        break;
    }

    case BEWARE_END:
    {
        if (m_pBodyDog->Get_ModelCom()->IsAnimationFinished() == true)
        {
            if (m_pInGame_Manager->IsDay())
            {
                Enter_Action(DOG_ACTION::SLEEP_START);
            }
            else
            {
                Enter_Action(DOG_ACTION::WAIT);
            }
        }

        if (PlayerIsInRange(m_fBewareRange) == true)
            Enter_Action(DOG_ACTION::BEWARE_START);
        break;
    }

    case ATK:
    {
        if (m_pBodyDog->Get_ModelCom()->IsAnimationFinished() == true)
        {
            if (PlayerIsInRange(m_fAttackRange) == true)
            {
                m_bIsAttackProcessed = false;
                m_pMoveCom->Stop_Move_To_Pos();
                if (m_iAttackIndex++ % 2 == 0)
                    Enter_Animation(CBody_Dog::DOG_ANI::ATK1);
                else
                    Enter_Animation(CBody_Dog::DOG_ANI::ATK2);
            }
            else if (IsNearSpawnPoint(m_fBewareRange * 1.5f) == true)
            {
                Enter_Action(DOG_ACTION::RUN);
            }
            else
            {
                Enter_Action(DOG_ACTION::ENDBATTLE);
            }
        }
        break;
    }

    case APPEAR:
    {
        if (m_pBodyDog->Get_ModelCom()->IsAnimationFinished() == true)
            Enter_Action(DOG_ACTION::SLEEP_START);
        //Enter_Action(DOG_ACTION::WAIT);
        break;
    }

    case RETURN:
    {
        if (m_pMoveCom->IsMove() == false)
        {
            if (PlayerIsInRange(m_fBewareRange) == true)
            {
                Enter_Action(DOG_ACTION::BEWARE_LOOP);
            }
            else
            {
                //Enter_Action(DOG_ACTION::WAIT);
                Enter_Action(DOG_ACTION::SLEEP_START);
            }
        }
        break;
    }
    }
}

void CDog::Enter_Action(DOG_ACTION eNewAction)
{
    m_eCurState = eNewAction;

    if (m_eCurState != m_ePreState)
    {
        switch (m_eCurState)
        {
        case WAIT:
            Enter_Animation(CBody_Dog::DOG_ANI::WAIT);
            m_pMoveCom->Stop_Move_To_Pos();
            break;

        case SLEEP_START:
            Enter_Animation(CBody_Dog::DOG_ANI::SLEEP_START);
            m_pMoveCom->Stop_Move_To_Pos();
            break;

        case SLEEP:
            Enter_Animation(CBody_Dog::DOG_ANI::SLEEP);
            break;

        case SLEEP_END:
            Enter_Animation(CBody_Dog::DOG_ANI::WAKE);

            if (m_pInGame_Manager->IsDay() == true)
            {
                m_pGameInstance->PlaySound_Once(ETOUI(SOUND_KEY::WILDDOG_WAKEUP));
                m_bIsCried = true;
            }
            break;

        case RUN:
            Enter_Animation(CBody_Dog::DOG_ANI::RUN);
            break;

        case ENDBATTLE:
            m_pMoveCom->Stop_Move_To_Pos();
            Enter_Animation(CBody_Dog::DOG_ANI::ENDBATTLE);
            break;

        case DYING:
            Enter_Animation(CBody_Dog::DOG_ANI::DYING);
            break;

        case DEATH:
            Enter_Animation(CBody_Dog::DOG_ANI::DEATH);
            m_iMonsterCondition |= MONSTER_CONDITION::CON_DEAD;
            m_pMoveCom->Stop_Move_To_Pos();

            m_pInGameHPBar->Set_IsInactive(true);

            m_pGameInstance->PlaySound_Once(ETOUI(SOUND_KEY::WILDDOG_DIE));
            break;

        case DANCE:
            Enter_Animation(CBody_Dog::DOG_ANI::DANCE);
            break;

        case BEWARE_START:
            if (m_pInGame_Manager->IsDay() == false && m_bIsCried == false)
                m_pGameInstance->PlaySound_Once(ETOUI(SOUND_KEY::WILDDOG_WAKEUP));
            else
                m_bIsCried = false;

            Enter_Animation(CBody_Dog::DOG_ANI::BEWARE_START);
            break;

        case BEWARE_LOOP:
            Enter_Animation(CBody_Dog::DOG_ANI::BEWARE_LOOP);
            break;

        case BEWARE_END:
            Enter_Animation(CBody_Dog::DOG_ANI::BEWARE_END);
            break;

        case ATK:
            m_bIsAttackProcessed = false;
            m_pMoveCom->Stop_Move_To_Pos();
            if (m_iAttackIndex++ % 2 == 0)
                Enter_Animation(CBody_Dog::DOG_ANI::ATK1);
            else
                Enter_Animation(CBody_Dog::DOG_ANI::ATK2);
            break;

        case APPEAR:
            Enter_Animation(CBody_Dog::DOG_ANI::APPEAR);
            break;

        case RETURN:
            m_iMonsterCondition &= ~CON_ATTACK;
            Enter_Animation(CBody_Dog::DOG_ANI::RUN);
            m_pMoveCom->Move_To_Pos(m_vStartPos, true);
            m_iAttackIndex = 0;

            Safe_Release(m_pTargetPlayer);
            m_pTargetPlayer = nullptr;

            break;
        }

        m_ePreState = m_eCurState;
    }
}

void CDog::Execute_Action(_float fTimeDelta)
{
    switch (m_eCurState)
    {
    case WAIT:
        break;

    case RUN:
        if (m_pTargetPlayer != nullptr) {
            _vector vTempTargetPos = m_pTargetPlayer->Get_TransformCom()->Get_State(STATE::POSITION);
            _float3 vTargetPos{};
            XMStoreFloat3(&vTargetPos, vTempTargetPos);
            m_pMoveCom->Move_To_Pos(vTargetPos, true);
        }
        break;

    case ENDBATTLE:
        break;

    case DYING:
        break;

    case DEATH:
        break;

    case DANCE:
        break;

    case BEWARE_START:

        break;

    case BEWARE_LOOP:

        break;

    case BEWARE_END:

        break;

    case ATK:
    {
        if (m_bIsAttackProcessed == false)
        {
            _float fAttackTime{};
            if (m_eCurAni == CBody_Dog::DOG_ANI::ATK1)
                fAttackTime = 0.2f;
            else
                fAttackTime = 0.3f;

            if (m_pBodyDog->Get_ModelCom()->Get_AniPlayRatio(m_eCurAni) >= fAttackTime)
            {
                if (m_pTargetPlayer != nullptr)
                {
                    DAMAGE_INFO tDamageInfo{};
                    tDamageInfo.iDamage = 30;
                    tDamageInfo.pUnit = nullptr;
                    m_pTargetPlayer->Damaged(tDamageInfo);

                    m_pGameInstance->PlaySound_Once(ETOUI(SOUND_KEY::WILDDOG_ATTACK));
                    m_pGameInstance->PlaySound_Once(ETOUI(SOUND_KEY::WILDDOG_HIT));
                }
                m_bIsAttackProcessed = true;
            }
        }

        // 플레이어 바라보게 회전
        CTransform* pPlayerTransformCom = m_pTargetPlayer->Get_TransformCom();
        _vector vDir = pPlayerTransformCom->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
        vDir = XMVector3Normalize(vDir);
        m_pTransformCom->TurnDirDefaultY(vDir, fTimeDelta, 1080.f);
        break;
    }
    case APPEAR:
        break;

    case RETURN:
        break;
    }
}

HRESULT CDog::Ready_Components()
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
    AABBDesc.vSize = _float3(1.f, 1.0f, 1.f);
    AABBDesc.vCenter = _float3(0.f, fColliderCenterY, 0.f);

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&pColliderCom), &AABBDesc)))
        return E_FAIL;

    pColliderCom->Set_Owner(this);
    m_Colliders.push_back(pColliderCom);

    m_pGameInstance->Add_Collider(pColliderCom);
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::MONSTER));
    //pColliderCom->Set_Mask(ETOUI(Collision_Layer::PLAYER));

    m_pGameInstance->Add_PickingCollider(pColliderCom);

    tLocalMinMax = {};
    Cal_LocalMinMaxAABB(tLocalMinMax, AABBDesc.vCenter, AABBDesc.vSize);

    return S_OK;
}

HRESULT CDog::Ready_PartObjects()
{
    // Body
    CBody_Dog::BODY_DOG_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Dog"),
        TEXT("Body"), &BodyDesc)))
        return E_FAIL;

    m_pBodyDog = dynamic_cast<CBody_Dog*>(m_PartObjects[TEXT("Body")]);
    Safe_AddRef(m_pBodyDog);

    CInGameHPBar::INGAMEHPBAR_DESC HPBarDesc{};
    HPBarDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    HPBarDesc.fHeight = 2.5f;

    HPBarDesc.pMaxHp = &m_tFinalStat.iHP;
    HPBarDesc.pCurHp = &m_tCurStat.iHP;

    HPBarDesc.pMaxMp = &m_tFinalStat.iMP;
    HPBarDesc.pCurMp = &m_tCurStat.iMP;

    HPBarDesc.vHPColor = COLOR_TO_FLOAT(212, 22, 56);
    HPBarDesc.vMPColor = COLOR_TO_FLOAT(255, 255, 255);

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_InGameHPBar"),
        TEXT("HPBar"), &HPBarDesc)))
        return E_FAIL;

    m_pInGameHPBar = dynamic_cast<CInGameHPBar*>(m_PartObjects[TEXT("HPBar")]);
    Safe_AddRef(m_pInGameHPBar);

    return S_OK;
}

HRESULT CDog::Bind_ShaderResources()
{
    return S_OK;
}

void CDog::Run_OR_ATTACK()
{
    if (PlayerIsInRange(m_fAttackRange) == true)
        Enter_Action(DOG_ACTION::ATK);
    else
        Enter_Action(DOG_ACTION::RUN);
}

HRESULT CDog::Initialize_Stat()
{
    SetStat(m_tBaseStat, 2, 60, 1010, 100, 0.f, 0.f, 113, 0, 0, 0, 33, 1.1f, 0, 0, 4.67f);
    SetStat(m_tCurStat, 2, 999, 1010, 100, 0.f, 0.f, 113, 0, 0, 0, 33, 1.1f, 0, 0, 4.67f);

    SetFinalStat();

    m_eMPType = MAINGAUGE_TYPE::MP;

    return S_OK;
}

CDog* CDog::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDog* pInstance = new CDog(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CDog");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDog::Clone(void* pArg)
{
    CDog* pInstance = new CDog(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CDog");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDog::Free()
{
    Safe_Release(m_pBodyDog);

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pMoveCom);

    __super::Free();
}
