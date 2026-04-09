#include "CPlayer.h"

#include "CGameInstance.h"

#include "CBody_Player.h"
#include "CWeapon.h"

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

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    Enter_State(ACTION_STATE::IDLE_P);

    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    Player_Input(fTimeDelta);

    StateRequestProcessing(fTimeDelta);

    // PartObject들은 GameObject_Manager에 안 들어간다.
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Priority_Update(fTimeDelta);
    }
}

void CPlayer::Parallel_Update(_float fTimeDelta)
{
    if (m_pMoveCom->Update_Move_To_Pos(fTimeDelta))
    {
        m_iRequestFlag |= REQUEST_FLAG::RQ_IDLE;
    }

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

HRESULT CPlayer::Ready_Components()
{
    CMove::MOVE_DESC Desc{};
    Desc.pTransform = m_pTransformCom;
    Desc.fSpeed = 5.f;

    /* Com_Move */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_Move"),
        TEXT("Com_Move"), reinterpret_cast<CComponent**>(&m_pMoveCom), &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
    CBody_Player::BODY_PLAYER_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    BodyDesc.pCurMoveState = &m_iCurState;

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
        TEXT("Body"), &BodyDesc)))
        return E_FAIL;

    CWeapon::WEAPON_DESC WeaponDesc{};
    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponDesc.pCurMoveState = &m_iCurState;

    WeaponDesc.pSocketBoneMatrix = dynamic_cast<CBody_Player*>(m_PartObjects[TEXT("Body")])->Get_BoneMatrixPtr("Equip_R");

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Weapon"),
        TEXT("Weapon"), &WeaponDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{
    return S_OK;
}

void CPlayer::StateRequestProcessing(_float fTimeDelta)
{
    // 요청이 있을 때 한 번만 실행

    if (m_iRequestFlag & RQ_REST) {
        Enter_State(ACTION_STATE::REST_P);
        m_pMoveCom->Stop_Move_To_Pos();
    }
    else if (m_iRequestFlag & RQ_RUN && m_iControlFlag & BLOCK_RUN) {
        m_pMoveCom->Move_To_Pos(m_vTargetPos);
        Enter_State(ACTION_STATE::RUN_P);
    }
    else if (m_iRequestFlag & RQ_IDLE) {
        m_pMoveCom->Stop_Move_To_Pos();
        Enter_State(ACTION_STATE::IDLE_P);
    }

    m_iRequestFlag = 0;
}

void CPlayer::Enter_State(ACTION_STATE eNewState)
{
    // 상태가 바뀌었을 때 한 번만 실행
    m_iCurState = eNewState;

    if (m_iCurState != m_iPreState) {

        switch (m_iCurState) {
            case ACTION_STATE::IDLE_P:
            {
                break;
            }

            case ACTION_STATE::RUN_P:
            {
                break;
            }

            case ACTION_STATE::REST_P:
            {
                break;
            }
        }

        m_iPreState = m_iCurState;
    }
}

void CPlayer::Player_Input(_float fTimeDelta)
{
    if (m_pGameInstance->Mouse_Down(DIMB::RBUTTON)) {

        static _float3 vPos = { 0.f, 0.f, 0.f };
        vPos.z += 10.f;
        if (vPos.z >= 11.f) { vPos.z = 0.f; }
        m_vTargetPos = vPos;

        m_iRequestFlag |= REQUEST_FLAG::RQ_RUN;
    }

    if (m_pGameInstance->Key_Down(DIK_S)) {
        m_iRequestFlag |= REQUEST_FLAG::RQ_IDLE;
    }

    if (m_pGameInstance->Key_Down(DIK_X)) {
        m_iRequestFlag |= REQUEST_FLAG::RQ_REST;
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
    Safe_Release(m_pMoveCom);

    __super::Free();
}
