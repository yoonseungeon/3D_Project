#include "CPlayer.h"

#include "CGameInstance.h"

#include "CBody_Player.h"
#include "CWeapon.h"
#include "CBottle.h"

#include "CInGame_Manager.h"

#include "CLiDailinIdle.h"
#include "CLiDailinMove.h"
#include "CLiDailin_Q.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
    : CContainerObject{ Prototype }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
    if (pArg == nullptr) {
        GAMEOBJECT_DESC Desc{};

        Desc.tTransformDesc.fSpeedPerSec = 10.f;
        Desc.tTransformDesc.fRotationPerSec = XMConvertToRadians(180.f);

        if (FAILED(__super::Initialize(&Desc)))
            return E_FAIL;
    }
    else {
        GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

        pDesc->tTransformDesc.fSpeedPerSec = 10.f;
        pDesc->tTransformDesc.fRotationPerSec = XMConvertToRadians(180.f);

        if (FAILED(__super::Initialize(pDesc)))
            return E_FAIL;
    }

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(4.f, 0.f, -2.8f, 1.f));

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;
    
    CState* pLiDailinIdle = CLiDailinIdle::Create();
    m_States.emplace(L"Idle", pLiDailinIdle);
    m_States.emplace(L"Move", CLiDailinMove::Create());
    m_States.emplace(L"CLiDailin_Q", CLiDailin_Q::Create());

    m_pCurrentState = pLiDailinIdle;
    m_pCurrentState->Enter(this);

    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    Key_Input();
    Apply_WaitState();

    m_pCurrentState->Update(this, fTimeDelta);
    Apply_WaitState();

    // PartObject들은 GameObject_Manager에 안 들어간다.
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Priority_Update(fTimeDelta);
    }
}

void CPlayer::Parallel_Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Parallel_Update(fTimeDelta);
    }
}

void CPlayer::Update(_float fTimeDelta)
{    
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Update(fTimeDelta);
    }
}

void CPlayer::Late_Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Late_Update(fTimeDelta);
    }
}

HRESULT CPlayer::Render()
{
    return S_OK;
}

void CPlayer::Set_WaitState(wstring wstrState)
{
    auto iter = m_States.find(wstrState);
    if (iter == m_States.end()) {
        MSG_BOX("CPlayer.cpp: No State");
        m_pWaitState = nullptr;
    }

    m_pWaitState = iter->second;
}

void CPlayer::Apply_WaitState()
{
    if (m_pWaitState == nullptr) {
        return;
    }

    m_pCurrentState->Exit(this);

    m_pCurrentState = m_pWaitState;
    m_pWaitState = nullptr;

    m_pCurrentState->Enter(this);
}

void CPlayer::Set_Animation(wstring wstrPartObjTag, _uint eAniIndex, _bool bLoop)
{
    if (wstrPartObjTag == L"Body")
    {
        m_pBody->Set_Animation(eAniIndex, bLoop);

    }
    else if (wstrPartObjTag == L"Weapon")
    {
        auto iter = m_PartObjects.find(L"Weapon");

        if (iter != m_PartObjects.end())
        {
            static_cast<CWeapon*>(iter->second)->Set_Animation(eAniIndex, bLoop);
        }
    }
}

_bool CPlayer::IsAnimationFinished(wstring wstrPartObjTag)
{
    if (wstrPartObjTag == L"Body")
    {
        return m_pBody->IsAnimationFinished();

    }
    else if (wstrPartObjTag == L"Weapon")
    {
        auto iter = m_PartObjects.find(L"Weapon");

        if (iter != m_PartObjects.end())
        {
            return static_cast<CWeapon*>(iter->second)->IsAnimationFinished();
        }
    }

    return false;
}

void CPlayer::Set_AniInterpolationTime(wstring wstrPartObjTag, _float InterpolationTime)
{
    if (wstrPartObjTag == L"Body")
    {
        m_pBody->Set_AniInterpolationTime(InterpolationTime);

    }
    else if (wstrPartObjTag == L"Weapon")
    {
        auto iter = m_PartObjects.find(L"Weapon");

        if (iter != m_PartObjects.end())
        {
            static_cast<CWeapon*>(iter->second)->Set_AniInterpolationTime(InterpolationTime);
        }
    }
}

void CPlayer::Set_AniSpeed(wstring wstrPartObjTag, _uint iIndex, _float fAniSpeed)
{
    if (wstrPartObjTag == L"Body")
    {
        m_pBody->Set_AniSpeed(iIndex, fAniSpeed);

    }
    else if (wstrPartObjTag == L"Weapon")
    {
        auto iter = m_PartObjects.find(L"Weapon");

        if (iter != m_PartObjects.end())
        {
            static_cast<CWeapon*>(iter->second)->Set_AniSpeed(iIndex, fAniSpeed);
        }
    }
}

_bool CPlayer::Update_Move_To_Pos(_float fTimeDelta)
{
    return m_pMoveCom->Update_Move_To_Pos(fTimeDelta);
}

void CPlayer::Move_To_Pos(_float3 vPos, _bool bOperateNavi)
{
    m_pMoveCom->Move_To_Pos(vPos, bOperateNavi);
}

void CPlayer::Stop_Move_To_Pos()
{
    m_pMoveCom->Stop_Move_To_Pos();
}

HRESULT CPlayer::Ready_Components()
{
    /* For.Com_Navigation */
    CNavigation::NAVIGATION_DESC NaviDesc;

    NaviDesc.iCurrentCellIndex = 0;
    XMStoreFloat3(&NaviDesc.vObjectWorldPos, m_pTransformCom->Get_State(STATE::POSITION));

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;


    CMove::MOVE_DESC Desc{};
    Desc.pTransform = m_pTransformCom;
    Desc.pNavigationCom = m_pNavigationCom;

    Desc.fSpeed = 5.f;

    /* Com_Move */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_Move"),
        TEXT("Com_Move"), reinterpret_cast<CComponent**>(&m_pMoveCom), &Desc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
    // Body
    CBody_Player::BODY_PLAYER_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
        TEXT("Body"), &BodyDesc)))
        return E_FAIL;

    m_pBody = dynamic_cast<CBody_Player*>(m_PartObjects[TEXT("Body")]);
    Safe_AddRef(m_pBody);

    // Weapon
    CWeapon::WEAPON_DESC WeaponDesc{};
    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponDesc.pSocketBoneMatrix = m_pBody->Get_BoneMatrixPtr("Equip_R");

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Weapon"),
        TEXT("Weapon"), &WeaponDesc)))
        return E_FAIL;

    //// Bottle
    //CBottle::BOTTLE_DESC BottleDesc{};
    //BottleDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    //BottleDesc.pCurMoveState = &m_iCurState;

    //BottleDesc.pSocketBoneMatrix = m_pBody->Get_BoneMatrixPtr("Weapon_Special_1");

    //if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bottle"),
    //    TEXT("Bottle"), &BottleDesc)))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{
    return S_OK;
}

void CPlayer::Key_Input()
{
    if (m_pGameInstance->Key_Down(DIK_Q)) {
        COMMAND tCommand{};
        tCommand.eCommandType = COMMAND_TYPE::ATTACK_Q;

        m_pCurrentState->HandleCommand(this, tCommand);
    }
    else if (m_pGameInstance->Mouse_Down(DIMB::RBUTTON)) {
        COMMAND tCommand{};
        tCommand.eCommandType = COMMAND_TYPE::MOVE;
        tCommand.vTargetPos = CInGame_Manager::GetInstance()->MapPIcking();

        m_pCurrentState->HandleCommand(this, tCommand);
    }
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer* pInstance = new CPlayer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer::Free()
{
    for (auto& pair : m_States) {
        Safe_Release(pair.second);
    }
    m_States.clear();

    Safe_Release(m_pBody);

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pMoveCom);

    __super::Free();
}
