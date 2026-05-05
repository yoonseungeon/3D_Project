#include "CChicken.h"

#include "CGameInstance.h"

#include "CInGame_Manager.h"

#include "CBody_Chicken.h"

//test
#include "CAbstractPlayer.h"
#include "CUnit.h"

CChicken::CChicken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAbstractMonster{ pDevice, pContext }
{
}

CChicken::CChicken(const CChicken& Prototype)
    : CAbstractMonster{ Prototype }
{
}

HRESULT CChicken::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CChicken::Initialize(void* pArg)
{
    CHICKEN_DESC* pDesc = static_cast<CHICKEN_DESC*>(pArg);

    pDesc->tTransformDesc.vStartPos = { 5.3f, 3.f, 1.8f };

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    _matrix matRotY = XMMatrixRotationY(XMConvertToRadians(180.f));
    _matrix matRotX = XMMatrixRotationX(XMConvertToRadians(-90.f));
    XMStoreFloat4x4(&m_matDefaultPreTransform, matRotY);
    XMStoreFloat4x4(&m_matBugPreTransform, matRotX * matRotY);

    Enter_Action(CHICKEN_ACTION::APPEAR);

    m_fAttackRange = 1.f;

    return S_OK;
}

void CChicken::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

    if (m_pGameInstance->Key_Down(DIK_P))
    {
        DAMAGE_INFO tDamageInfo;
        tDamageInfo.iDamage = 0;
        tDamageInfo.pUnit = m_pInGame_Manager->Get_Player();
        Damaged(tDamageInfo);
    }
}

void CChicken::Parallel_Update(_float fTimeDelta)
{
    __super::Parallel_Update(fTimeDelta);

    for (auto& pColliderCom : m_Colliders)
        pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    m_pMoveCom->Update_Move_To_Pos(fTimeDelta);
    m_pNavigationCom->Compute_OnNavigation();
}

void CChicken::Update(_float fTimeDelta)
{
    Update_Action(fTimeDelta);
    Execute_Action(fTimeDelta);

    __super::Update(fTimeDelta);
}

void CChicken::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CChicken::Render()
{
	return S_OK;
}

void CChicken::Enter_Animation(CBody_Chicken::CHICKEN_ANI eNewAnimation)
{
    m_eCurAni = eNewAnimation;
    switch (eNewAnimation) {
        case CBody_Chicken::WAIT:
        {
            m_pBodyChicken->Get_ModelCom()->Reset_PreTransformMatrix(m_matDefaultPreTransform);
            m_pBodyChicken->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
            break;
        }
        case CBody_Chicken::RUN:
        {
            m_pBodyChicken->Get_ModelCom()->Reset_PreTransformMatrix(m_matBugPreTransform);
            m_pBodyChicken->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
            break;
        }
        case CBody_Chicken::ENDBATTLE:
        {
            m_pBodyChicken->Get_ModelCom()->Reset_PreTransformMatrix(m_matBugPreTransform);
            m_pBodyChicken->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
            break;
        }
        case CBody_Chicken::DYING:
        {
            m_pBodyChicken->Get_ModelCom()->Reset_PreTransformMatrix(m_matBugPreTransform);
            m_pBodyChicken->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
            break;
        }
        case CBody_Chicken::DEATH:
        {
            m_pBodyChicken->Get_ModelCom()->Reset_PreTransformMatrix(m_matBugPreTransform);
            m_pBodyChicken->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
            break;
        }
        case CBody_Chicken::DANCE:
        {
            m_pBodyChicken->Get_ModelCom()->Reset_PreTransformMatrix(m_matDefaultPreTransform);
            m_pBodyChicken->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
            break;
        }
        case CBody_Chicken::BEWARE_START:
        {
            m_pBodyChicken->Get_ModelCom()->Reset_PreTransformMatrix(m_matDefaultPreTransform);
            m_pBodyChicken->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
            break;
        }
        case CBody_Chicken::BEWARE_LOOP:
        {
            m_pBodyChicken->Get_ModelCom()->Reset_PreTransformMatrix(m_matDefaultPreTransform);
            m_pBodyChicken->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, true);
            break;
        }
        case CBody_Chicken::BEWARE_END:
        {
            m_pBodyChicken->Get_ModelCom()->Reset_PreTransformMatrix(m_matDefaultPreTransform);
            m_pBodyChicken->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
            break;
        }
        case CBody_Chicken::ATK2:
        {
            m_pBodyChicken->Get_ModelCom()->Reset_PreTransformMatrix(m_matBugPreTransform);
            m_pBodyChicken->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
            break;
        }
        case CBody_Chicken::ATK1:
        {
            m_pBodyChicken->Get_ModelCom()->Reset_PreTransformMatrix(m_matBugPreTransform);
            m_pBodyChicken->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
            break;
        }
        case CBody_Chicken::APPEAR:
        {
            m_pBodyChicken->Get_ModelCom()->Reset_PreTransformMatrix(m_matBugPreTransform);
            m_pBodyChicken->Get_ModelCom()->Set_AnimationIndex(eNewAnimation, false);
            break;
        }
    }
}

void CChicken::Update_Action(_float fTimeDelta)
{
    if (m_eCurState  == CHICKEN_ACTION::DYING)
        return;

    if (m_eCurState == CHICKEN_ACTION::DEATH)
    {
        if (m_pBodyChicken->Get_ModelCom()->IsAnimationFinished() == true)
        {
            Enter_Action(CHICKEN_ACTION::DYING);
            return;
        }
    }

    if (m_pTargetPlayer != nullptr && !(iMonsterCondition & CON_ATTACK))
    {
        Run_OR_ATTACK();
        iMonsterCondition |= CON_ATTACK;
    }

    switch (m_eCurState)
    {
    case WAIT:
       if (PlayerIsInRange(m_fBewareRange) == true)
           Enter_Action(CHICKEN_ACTION::BEWARE_START);
        break;

    case RUN:
        if (IsInRange(m_fBewareRange * 2.f) == false)
        {
            Enter_Action(CHICKEN_ACTION::RETURN);
            return;
        }

        if (PlayerIsInRange(m_fAttackRange) == true)
            Enter_Action(CHICKEN_ACTION::ATK);

        break;

    case ENDBATTLE:
        if (m_pBodyChicken->Get_ModelCom()->IsAnimationFinished() == true)
            
        break;

    case DANCE:
        break;

    case BEWARE_START:
        if (m_pBodyChicken->Get_ModelCom()->IsAnimationFinished() == true)
            Enter_Action(CHICKEN_ACTION::BEWARE_LOOP);
        break;

    case BEWARE_LOOP:
        if (PlayerIsInRange(m_fBewareRange) == false)
            Enter_Action(CHICKEN_ACTION::BEWARE_END);
        break;

    case BEWARE_END:
        if (m_pBodyChicken->Get_ModelCom()->IsAnimationFinished() == true)
            Enter_Action(CHICKEN_ACTION::WAIT);
        break;

    case ATK:
        break;

    case APPEAR:
        if (m_pBodyChicken->Get_ModelCom()->IsAnimationFinished() == true)
            Enter_Action(CHICKEN_ACTION::WAIT);
        break;

    case RETURN:
        if (IsInRange(0.1f) == true)
            Enter_Action(CHICKEN_ACTION::WAIT);
        break;
    }
}

void CChicken::Enter_Action(CHICKEN_ACTION eNewAction)
{
    m_eCurState = eNewAction;

    if(m_eCurState != m_ePreState)
    {
        switch (m_eCurState)
        {
        case WAIT:           
            Enter_Animation(CBody_Chicken::CHICKEN_ANI::WAIT);
            break;

        case RUN:
            Enter_Animation(CBody_Chicken::CHICKEN_ANI::RUN);
            break;

        case ENDBATTLE:
            Enter_Animation(CBody_Chicken::CHICKEN_ANI::ENDBATTLE);
            break;

        case DYING:
            Enter_Animation(CBody_Chicken::CHICKEN_ANI::DYING);
            break;

        case DEATH:
            Enter_Animation(CBody_Chicken::CHICKEN_ANI::DEATH);
            break;

        case DANCE:
            Enter_Animation(CBody_Chicken::CHICKEN_ANI::DANCE);
            break;

        case BEWARE_START:
            Enter_Animation(CBody_Chicken::CHICKEN_ANI::BEWARE_START);
            break;

        case BEWARE_LOOP:
            Enter_Animation(CBody_Chicken::CHICKEN_ANI::BEWARE_LOOP);
            break;

        case BEWARE_END:
            Enter_Animation(CBody_Chicken::CHICKEN_ANI::BEWARE_END);
            break;

        case ATK:
            m_pMoveCom->Stop_Move_To_Pos();
            if(rand() % 2 == 0)
                Enter_Animation(CBody_Chicken::CHICKEN_ANI::ATK1);
            else
                Enter_Animation(CBody_Chicken::CHICKEN_ANI::ATK2);
            break;

        case APPEAR:
            Enter_Animation(CBody_Chicken::CHICKEN_ANI::APPEAR);
            break;

        case RETURN:
            iMonsterCondition &= ~CON_ATTACK;
            Enter_Animation(CBody_Chicken::CHICKEN_ANI::RUN);
            m_pMoveCom->Move_To_Pos(m_vStartPos, true);

            Safe_Release(m_pTargetPlayer);
            m_pTargetPlayer = nullptr;
            break;
    }

        m_ePreState = m_eCurState;
    }
}

void CChicken::Execute_Action(_float fTimeDelta)
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
        if (m_pBodyChicken->Get_ModelCom()->IsAnimationFinished() == true)
        {
            if (PlayerIsInRange(m_fAttackRange) == true)
            {
                m_pMoveCom->Stop_Move_To_Pos();
                if (rand() % 2 == 0)
                    Enter_Animation(CBody_Chicken::CHICKEN_ANI::ATK1);
                else
                    Enter_Animation(CBody_Chicken::CHICKEN_ANI::ATK2);
            }
            else if(IsInRange(m_fBewareRange * 2.f) == true)
            {
                Enter_Action(CHICKEN_ACTION::RUN);
            }
            else
            {
                Enter_Action(CHICKEN_ACTION::RETURN);
            }
        }
        break;

    case APPEAR:
        break;

    case RETURN:
        break;
    }
}

HRESULT CChicken::Ready_Components()
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
    AABBDesc.vSize = _float3(0.7f, 1.0f, 0.7f);
    AABBDesc.vCenter = _float3(0.f, AABBDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&pColliderCom), &AABBDesc)))
        return E_FAIL;

    m_Colliders.push_back(pColliderCom);

    m_pGameInstance->Add_Collider(pColliderCom);
    pColliderCom->Set_Owner(this);
    pColliderCom->Set_Layer(ETOUI(Collision_Layer::MONSTER));
    pColliderCom->Set_Mask(ETOUI(Collision_Layer::PLAYER));

    return S_OK;
}

HRESULT CChicken::Ready_PartObjects()
{
    // Body
    CBody_Chicken::BODY_CHICKEN_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Chicken"),
        TEXT("Body"), &BodyDesc)))
        return E_FAIL;

    m_pBodyChicken = dynamic_cast<CBody_Chicken*>(m_PartObjects[TEXT("Body")]);
    Safe_AddRef(m_pBodyChicken);

    return S_OK;
}

HRESULT CChicken::Bind_ShaderResources()
{
    return S_OK;
}

void CChicken::Run_OR_ATTACK()
{
    if (PlayerIsInRange(m_fAttackRange) == true)
        Enter_Action(CHICKEN_ACTION::ATK);
    else
        Enter_Action(CHICKEN_ACTION::RUN);
}

CChicken* CChicken::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CChicken* pInstance = new CChicken(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CChicken");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CChicken::Clone(void* pArg)
{
    CChicken* pInstance = new CChicken(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CChicken");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CChicken::Free()
{
    Safe_Release(m_pBodyChicken);

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pMoveCom);

	__super::Free();
}
