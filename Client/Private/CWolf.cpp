#include "CWolf.h"

#include "CGameInstance.h"

#include "CInGame_Manager.h"

#include "CAbstractPlayer.h"
#include "CInventory.h"

CWolf::CWolf(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAbstractMonster{ pDevice, pContext }
{
}

CWolf::CWolf(const CWolf& Prototype)
    : CAbstractMonster{ Prototype }
{
}

HRESULT CWolf::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWolf::Initialize(void* pArg)
{
    WOLF_DESC* pDesc = static_cast<WOLF_DESC*>(pArg);

    pDesc->tTransformDesc.vStartPos = { -0.38f, 3.f, 6.f };

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    Enter_Action(WOLF_ACTION::APPEAR);

    if (FAILED(Initialize_Stat()))
        return E_FAIL;

    m_fAttackRange = 1.5f;

    m_pInvetory->Add_Item(34);
    m_pInvetory->Add_Item(52);
    m_pInvetory->Add_Item(6);

    return S_OK;
}

void CWolf::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CWolf::Parallel_Update(_float fTimeDelta)
{
    __super::Parallel_Update(fTimeDelta);

    for (auto& pColliderCom : m_Colliders)
        pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    m_pMoveCom->Update_Move_To_Pos(fTimeDelta);
    m_pNavigationCom->Compute_OnNavigation();
}

void CWolf::Update(_float fTimeDelta)
{
    Update_Action(fTimeDelta);
    Execute_Action(fTimeDelta);

    __super::Update(fTimeDelta);
}

void CWolf::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CWolf::Render()
{
    return S_OK;
}

void CWolf::Enter_Animation(CBody_Wolf::WOLF_ANI eNewAnimation)
{
    m_eCurAni = eNewAnimation;
    switch (eNewAnimation) {
    case CBody_Wolf::WAKE:
    {
        m_pBodyWolf->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Wolf::WAIT:
    {
        m_pBodyWolf->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
        break;
    }
    case CBody_Wolf::SLEEP_START:
    {
        m_pBodyWolf->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Wolf::SLEEP:
    {
        m_pBodyWolf->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
        break;
    }
    case CBody_Wolf::RUN:
    {
        m_pBodyWolf->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
        break;
    }
    case CBody_Wolf::ENDBATTLE:
    {
        m_pBodyWolf->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Wolf::DYING:
    {
        m_pBodyWolf->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
        break;
    }
    case CBody_Wolf::DEATH:
    {
        m_pBodyWolf->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Wolf::DANCE:
    {
        m_pBodyWolf->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Wolf::BEWARE_START:
    {
        m_pBodyWolf->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Wolf::BEWARE_LOOP:
    {
        m_pBodyWolf->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
        break;
    }
    case CBody_Wolf::BEWARE_END:
    {
        m_pBodyWolf->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Wolf::ATK2:
    {
        m_pBodyWolf->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Wolf::ATK1:
    {
        m_pBodyWolf->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Wolf::APPEAR:
    {
        m_pBodyWolf->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    }
}

void CWolf::Update_Action(_float fTimeDelta)
{
    if (m_eCurState == WOLF_ACTION::DYING)
        return;

    if (m_eCurState == WOLF_ACTION::DEATH)
    {
        if (m_pBodyWolf->Get_ModelCom()->IsAnimationFinished() == true)
        {
            Enter_Action(WOLF_ACTION::DYING);
            return;
        }
    }
    else if (m_iMonsterCondition & MONSTER_CONDITION::CON_HPZERO)
    {
        Enter_Action(WOLF_ACTION::DEATH);
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
            // 진입 x
            if (PlayerIsInRange(m_fBewareRange) == true)
                Enter_Action(WOLF_ACTION::BEWARE_START);
            break;
        }

        case SLEEP_START:
        {
            if (m_pBodyWolf->Get_ModelCom()->IsAnimationFinished() == true)
                Enter_Action(WOLF_ACTION::SLEEP);
            break;
        }

        case SLEEP:
        {
            if (PlayerIsInRange(m_fBewareRange) == true)
                Enter_Action(WOLF_ACTION::SLEEP_END);
            break;
        }

        case SLEEP_END:
        {
            if (m_pBodyWolf->Get_ModelCom()->IsAnimationFinished() == true)
                Enter_Action(WOLF_ACTION::BEWARE_START);
            break;
        }

        case RUN:
        {
            if (IsNearSpawnPoint(m_fBewareRange * 1.5f) == false)
            {
                Enter_Action(WOLF_ACTION::ENDBATTLE);
                return;
            }

            if (PlayerIsInRange(m_fAttackRange) == true)
                Enter_Action(WOLF_ACTION::ATK);

            break;
        }

        case ENDBATTLE:
        {
            if (m_pBodyWolf->Get_ModelCom()->IsAnimationFinished() == true)
                Enter_Action(WOLF_ACTION::RETURN);
            break;
        }

        case DANCE:
        {
            break;
        }

        case BEWARE_START:
        {
            if (m_pBodyWolf->Get_ModelCom()->IsAnimationFinished() == true)
                Enter_Action(WOLF_ACTION::BEWARE_LOOP);
            break;
        }

        case BEWARE_LOOP:
        {
            if (PlayerIsInRange(m_fBewareRange) == false)
                Enter_Action(WOLF_ACTION::SLEEP_START);
                //Enter_Action(WOLF_ACTION::BEWARE_END);
            break;
        }

        case BEWARE_END:
        {
            // 진입 x
            if (m_pBodyWolf->Get_ModelCom()->IsAnimationFinished() == true)
                Enter_Action(WOLF_ACTION::SLEEP_START);
                //Enter_Action(WOLF_ACTION::WAIT);
            break;
        }

        case ATK:
        {
            if (m_pBodyWolf->Get_ModelCom()->IsAnimationFinished() == true)
            {
                if (PlayerIsInRange(m_fAttackRange) == true)
                {
                    m_bIsAttackProcessed = false;
                    m_pMoveCom->Stop_Move_To_Pos();
                    if (m_iAttackIndex++ % 2 == 0)
                        Enter_Animation(CBody_Wolf::WOLF_ANI::ATK1);
                    else
                        Enter_Animation(CBody_Wolf::WOLF_ANI::ATK2);
                }
                else if (IsNearSpawnPoint(m_fBewareRange * 1.5f) == true)
                {
                    Enter_Action(WOLF_ACTION::RUN);
                }
                else
                {
                    Enter_Action(WOLF_ACTION::ENDBATTLE);
                }
            }
            break;
        }

        case APPEAR:
        {
            if (m_pBodyWolf->Get_ModelCom()->IsAnimationFinished() == true)
                Enter_Action(WOLF_ACTION::SLEEP_START);
                //Enter_Action(WOLF_ACTION::WAIT);
            break;
        }

        case RETURN:
        {
            if (m_pMoveCom->IsMove() == false)
            {
                if (PlayerIsInRange(m_fBewareRange) == true)
                {
                    Enter_Action(WOLF_ACTION::BEWARE_LOOP);
                }
                else
                {
                    //Enter_Action(WOLF_ACTION::WAIT);
                    Enter_Action(WOLF_ACTION::SLEEP_START);
                }
            }
            break;
        }
    }
}

void CWolf::Enter_Action(WOLF_ACTION eNewAction)
{
    m_eCurState = eNewAction;

    if (m_eCurState != m_ePreState)
    {
        switch (m_eCurState)
        {
        case WAIT:
            Enter_Animation(CBody_Wolf::WOLF_ANI::WAIT);
            m_pMoveCom->Stop_Move_To_Pos();
            break;

        case SLEEP_START:
            Enter_Animation(CBody_Wolf::WOLF_ANI::SLEEP_START);
            m_pMoveCom->Stop_Move_To_Pos();
            break;

        case SLEEP:
            Enter_Animation(CBody_Wolf::WOLF_ANI::SLEEP);
            break;

        case SLEEP_END:
            Enter_Animation(CBody_Wolf::WOLF_ANI::WAKE);
            break;

        case RUN:
            Enter_Animation(CBody_Wolf::WOLF_ANI::RUN);
            break;

        case ENDBATTLE:
            m_pMoveCom->Stop_Move_To_Pos();
            Enter_Animation(CBody_Wolf::WOLF_ANI::ENDBATTLE);
            break;

        case DYING:
            Enter_Animation(CBody_Wolf::WOLF_ANI::DYING);
            break;

        case DEATH:
            Enter_Animation(CBody_Wolf::WOLF_ANI::DEATH);
            m_iMonsterCondition |= MONSTER_CONDITION::CON_DEAD;
            break;

        case DANCE:
            Enter_Animation(CBody_Wolf::WOLF_ANI::DANCE);
            break;

        case BEWARE_START:
            Enter_Animation(CBody_Wolf::WOLF_ANI::BEWARE_START);
            break;

        case BEWARE_LOOP:
            Enter_Animation(CBody_Wolf::WOLF_ANI::BEWARE_LOOP);
            break;

        case BEWARE_END:
            Enter_Animation(CBody_Wolf::WOLF_ANI::BEWARE_END);
            break;

        case ATK:
            m_bIsAttackProcessed = false;
            m_pMoveCom->Stop_Move_To_Pos();
            if (m_iAttackIndex++ % 2 == 0)
                Enter_Animation(CBody_Wolf::WOLF_ANI::ATK1);
            else
                Enter_Animation(CBody_Wolf::WOLF_ANI::ATK2);
            break;

        case APPEAR:
            Enter_Animation(CBody_Wolf::WOLF_ANI::APPEAR);
            break;

        case RETURN:
            m_iMonsterCondition &= ~CON_ATTACK;
            Enter_Animation(CBody_Wolf::WOLF_ANI::RUN);
            m_pMoveCom->Move_To_Pos(m_vStartPos, true);
            m_iAttackIndex = 0;

            Safe_Release(m_pTargetPlayer);
            m_pTargetPlayer = nullptr;

            break;
        }

        m_ePreState = m_eCurState;
    }
}

void CWolf::Execute_Action(_float fTimeDelta)
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
            if (m_eCurAni == CBody_Wolf::WOLF_ANI::ATK1)
                fAttackTime = 0.2f;
            else
                fAttackTime = 0.3f;

            if (m_pBodyWolf->Get_ModelCom()->Get_AniPlayRatio(m_eCurAni) >= fAttackTime)
            {
                if (m_pTargetPlayer != nullptr)
                {
                    DAMAGE_INFO tDamageInfo{};
                    tDamageInfo.iDamage = 30;
                    tDamageInfo.pUnit = nullptr;
                    m_pTargetPlayer->Damaged(tDamageInfo);
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

HRESULT CWolf::Ready_Components()
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
    AABBDesc.vSize = _float3(1.2f, 1.0f, 1.2f);
    AABBDesc.vCenter = _float3(0.f, fColliderCenterY, 0.f);

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&pColliderCom), &AABBDesc)))
        return E_FAIL;

    m_Colliders.push_back(pColliderCom);

    m_pGameInstance->Add_Collider(pColliderCom);
    pColliderCom->Set_Owner(this);
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::MONSTER));
    pColliderCom->Set_Mask(ETOUI(Collision_Layer::PLAYER));

    tLocalMinMax = {};
    Cal_LocalMinMaxAABB(tLocalMinMax, AABBDesc.vCenter, AABBDesc.vSize);

    return S_OK;
}

HRESULT CWolf::Ready_PartObjects()
{
    // Body
    CBody_Wolf::BODY_WOLF_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Wolf"),
        TEXT("Body"), &BodyDesc)))
        return E_FAIL;

    m_pBodyWolf = dynamic_cast<CBody_Wolf*>(m_PartObjects[TEXT("Body")]);
    Safe_AddRef(m_pBodyWolf);

    return S_OK;
}

HRESULT CWolf::Bind_ShaderResources()
{
    return S_OK;
}

void CWolf::Run_OR_ATTACK()
{
    if (PlayerIsInRange(m_fAttackRange) == true)
        Enter_Action(WOLF_ACTION::ATK);
    else
        Enter_Action(WOLF_ACTION::RUN);
}

HRESULT CWolf::Initialize_Stat()
{
    SetStat(m_tBaseStat, 2, 0, 265, 100, 0.f, 0.f, 62, 0, 0, 0, 18, 0.8f, 0, 0, 3.76f);
    SetStat(m_tCurStat, 2, 999, 265, 100, 0.f, 0.f, 62, 0, 0, 0, 18, 0.8f, 0, 0, 3.76f);

    SetFinalStat();

    m_eMPType = MAINGAUGE_TYPE::MP;

    return S_OK;
}

CWolf* CWolf::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWolf* pInstance = new CWolf(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CWolf");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWolf::Clone(void* pArg)
{
    CWolf* pInstance = new CWolf(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CWolf");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWolf::Free()
{
    Safe_Release(m_pBodyWolf);

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pMoveCom);

    __super::Free();
}
