#include "CBoar.h"

#include "CGameInstance.h"

#include "CInGame_Manager.h"

#include "CAbstractPlayer.h"
#include "CInventory.h"

CBoar::CBoar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAbstractMonster{ pDevice, pContext }
{
}

CBoar::CBoar(const CBoar& Prototype)
    : CAbstractMonster{ Prototype }
{
}

HRESULT CBoar::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBoar::Initialize(void* pArg)
{
    BOAR_DESC* pDesc = static_cast<BOAR_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    Enter_Action(BOAR_ACTION::APPEAR);

    if (FAILED(Initialize_Stat()))
        return E_FAIL;

    m_fAttackRange = 1.5f;

    m_pInvetory->Add_Item(34);
    m_pInvetory->Add_Item(52);
    m_pInvetory->Add_Item(26);

    return S_OK;
}

void CBoar::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CBoar::Parallel_Update(_float fTimeDelta)
{
    __super::Parallel_Update(fTimeDelta);

    for (auto& pColliderCom : m_Colliders)
        pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    m_pMoveCom->Update_Move_To_Pos(fTimeDelta);
    m_pNavigationCom->Compute_OnNavigation();
}

void CBoar::Update(_float fTimeDelta)
{
    Update_Action(fTimeDelta);
    Execute_Action(fTimeDelta);

    __super::Update(fTimeDelta);
}

void CBoar::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CBoar::Render()
{
    return S_OK;
}

void CBoar::Enter_Animation(CBody_Boar::BOAR_ANI eNewAnimation)
{
    m_eCurAni = eNewAnimation;
    switch (eNewAnimation) {
    case CBody_Boar::WAIT:
    {
        m_pBodyBoar->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
        break;
    }
    case CBody_Boar::RUN:
    {
        m_pBodyBoar->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
        break;
    }
    case CBody_Boar::ENDBATTLE:
    {
        m_pBodyBoar->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Boar::DYING:
    {
        m_pBodyBoar->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
        break;
    }
    case CBody_Boar::DEATH:
    {
        m_pBodyBoar->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Boar::DANCE:
    {
        m_pBodyBoar->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Boar::BEWARE_START:
    {
        m_pBodyBoar->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Boar::BEWARE_LOOP:
    {
        m_pBodyBoar->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
        break;
    }
    case CBody_Boar::BEWARE_END:
    {
        m_pBodyBoar->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Boar::ATK2:
    {
        m_pBodyBoar->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Boar::ATK1:
    {
        m_pBodyBoar->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    case CBody_Boar::APPEAR:
    {
        m_pBodyBoar->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
        break;
    }
    }
}

void CBoar::Update_Action(_float fTimeDelta)
{
    if (m_eCurState == BOAR_ACTION::DYING)
        return;

    if (m_eCurState == BOAR_ACTION::DEATH)
    {
        if (m_pBodyBoar->Get_ModelCom()->IsAnimationFinished() == true)
        {
            Enter_Action(BOAR_ACTION::DYING);
            return;
        }
    }
    else if (m_iMonsterCondition & MONSTER_CONDITION::CON_HPZERO)
    {
        Enter_Action(BOAR_ACTION::DEATH);
    }

    if (m_pTargetPlayer != nullptr && !(m_iMonsterCondition & CON_ATTACK))
    {
        Run_OR_ATTACK();
        m_iMonsterCondition |= CON_ATTACK;
    }

    switch (m_eCurState)
    {
    case WAIT:
        if (PlayerIsInRange(m_fBewareRange) == true)
            Enter_Action(BOAR_ACTION::BEWARE_START);
        break;

    case RUN:
        if (IsNearSpawnPoint(m_fBewareRange * 1.5f) == false)
        {
            Enter_Action(BOAR_ACTION::ENDBATTLE);
            return;
        }

        if (PlayerIsInRange(m_fAttackRange) == true)
            Enter_Action(BOAR_ACTION::ATK);

        break;

    case ENDBATTLE:
        if (m_pBodyBoar->Get_ModelCom()->IsAnimationFinished() == true)
            Enter_Action(BOAR_ACTION::RETURN);
        break;

    case DANCE:
        break;

    case BEWARE_START:
        if (m_pBodyBoar->Get_ModelCom()->IsAnimationFinished() == true)
            Enter_Action(BOAR_ACTION::BEWARE_LOOP);
        break;

    case BEWARE_LOOP:
        if (PlayerIsInRange(m_fBewareRange) == false)
            Enter_Action(BOAR_ACTION::BEWARE_END);
        break;

    case BEWARE_END:
        if (m_pBodyBoar->Get_ModelCom()->IsAnimationFinished() == true)
            Enter_Action(BOAR_ACTION::WAIT);
        break;

    case ATK:
        if (m_pBodyBoar->Get_ModelCom()->IsAnimationFinished() == true)
        {
            if (PlayerIsInRange(m_fAttackRange) == true)
            {
                m_bIsAttackProcessed = false;
                m_pMoveCom->Stop_Move_To_Pos();
                if (m_iAttackIndex++ % 2 == 0)
                    Enter_Animation(CBody_Boar::BOAR_ANI::ATK1);
                else
                    Enter_Animation(CBody_Boar::BOAR_ANI::ATK2);
            }
            else if (IsNearSpawnPoint(m_fBewareRange * 1.5f) == true)
            {
                Enter_Action(BOAR_ACTION::RUN);
            }
            else
            {
                Enter_Action(BOAR_ACTION::ENDBATTLE);
            }
        }
        break;

    case APPEAR:
        if (m_pBodyBoar->Get_ModelCom()->IsAnimationFinished() == true)
            Enter_Action(BOAR_ACTION::WAIT);
        break;

    case RETURN:
        if (m_pMoveCom->IsMove() == false)
            Enter_Action(BOAR_ACTION::WAIT);
        break;
    }
}

void CBoar::Enter_Action(BOAR_ACTION eNewAction)
{
    m_eCurState = eNewAction;

    if (m_eCurState != m_ePreState)
    {
        switch (m_eCurState)
        {
        case WAIT:
            Enter_Animation(CBody_Boar::BOAR_ANI::WAIT);
            m_pMoveCom->Stop_Move_To_Pos();
            break;

        case RUN:
            Enter_Animation(CBody_Boar::BOAR_ANI::RUN);
            break;

        case ENDBATTLE:
            m_pMoveCom->Stop_Move_To_Pos();
            Enter_Animation(CBody_Boar::BOAR_ANI::ENDBATTLE);
            break;

        case DYING:
            Enter_Animation(CBody_Boar::BOAR_ANI::DYING);
            break;

        case DEATH:
            Enter_Animation(CBody_Boar::BOAR_ANI::DEATH);
            m_iMonsterCondition |= MONSTER_CONDITION::CON_DEAD;
            break;

        case DANCE:
            Enter_Animation(CBody_Boar::BOAR_ANI::DANCE);
            break;

        case BEWARE_START:
            Enter_Animation(CBody_Boar::BOAR_ANI::BEWARE_START);
            break;

        case BEWARE_LOOP:
            Enter_Animation(CBody_Boar::BOAR_ANI::BEWARE_LOOP);
            break;

        case BEWARE_END:
            Enter_Animation(CBody_Boar::BOAR_ANI::BEWARE_END);
            break;

        case ATK:
            m_bIsAttackProcessed = false;
            m_pMoveCom->Stop_Move_To_Pos();
            if (m_iAttackIndex++ % 2 == 0)
                Enter_Animation(CBody_Boar::BOAR_ANI::ATK1);
            else
                Enter_Animation(CBody_Boar::BOAR_ANI::ATK2);
            break;

        case APPEAR:
            Enter_Animation(CBody_Boar::BOAR_ANI::APPEAR);
            break;

        case RETURN:
            m_iMonsterCondition &= ~CON_ATTACK;
            Enter_Animation(CBody_Boar::BOAR_ANI::RUN);
            m_pMoveCom->Move_To_Pos(m_vStartPos, true);
            m_iAttackIndex = 0;

            Safe_Release(m_pTargetPlayer);
            m_pTargetPlayer = nullptr;

            break;
        }

        m_ePreState = m_eCurState;
    }
}

void CBoar::Execute_Action(_float fTimeDelta)
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
            if (m_eCurAni == CBody_Boar::BOAR_ANI::ATK1)
                fAttackTime = 0.2f;
            else
                fAttackTime = 0.4f;

            if (m_pBodyBoar->Get_ModelCom()->Get_AniPlayRatio(m_eCurAni) >= fAttackTime)
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

HRESULT CBoar::Ready_Components()
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
    AABBDesc.vSize = _float3(1.5f, 1.0f, 1.5f);
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

HRESULT CBoar::Ready_PartObjects()
{
    // Body
    CBody_Boar::BODY_BOAR_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Boar"),
        TEXT("Body"), &BodyDesc)))
        return E_FAIL;

    m_pBodyBoar = dynamic_cast<CBody_Boar*>(m_PartObjects[TEXT("Body")]);
    Safe_AddRef(m_pBodyBoar);

    return S_OK;
}

HRESULT CBoar::Bind_ShaderResources()
{
    return S_OK;
}

void CBoar::Run_OR_ATTACK()
{
    if (PlayerIsInRange(m_fAttackRange) == true)
        Enter_Action(BOAR_ACTION::ATK);
    else
        Enter_Action(BOAR_ACTION::RUN);
}

HRESULT CBoar::Initialize_Stat()
{
    SetStat(m_tBaseStat, 2, 0, 265, 100, 0.f, 0.f, 62, 0, 0, 0, 18, 0.8f, 0, 0, 3.76f);
    SetStat(m_tCurStat, 2, 999, 265, 100, 0.f, 0.f, 62, 0, 0, 0, 18, 0.8f, 0, 0, 3.76f);

    SetFinalStat();

    m_eMPType = MAINGAUGE_TYPE::MP;

    return S_OK;
}

CBoar* CBoar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBoar* pInstance = new CBoar(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CBoar");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBoar::Clone(void* pArg)
{
    CBoar* pInstance = new CBoar(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CBoar");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoar::Free()
{
    Safe_Release(m_pBodyBoar);

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pMoveCom);

    __super::Free();
}
