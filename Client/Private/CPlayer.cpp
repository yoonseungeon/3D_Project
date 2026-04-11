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

    m_pBody = dynamic_cast<CBody_Player*>(m_PartObjects[TEXT("Body")]);

    CWeapon::WEAPON_DESC WeaponDesc{};
    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponDesc.pCurMoveState = &m_iCurState;
    WeaponDesc.pCurATKType = m_pBody->Get_CurATKType();

    Safe_AddRef(m_pBody);
    WeaponDesc.pSocketBoneMatrix = m_pBody->Get_BoneMatrixPtr("Equip_R");

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

    _uint iFinalRequestFlag{};

    if(m_pBody->IsAniLockExit() && (m_iReserveRequestFlag & 0xffffff))
    {
        iFinalRequestFlag = m_iReserveRequestFlag;
        m_iReserveRequestFlag = 0;
    }
    else
    {
        iFinalRequestFlag = m_iRequestFlag;
        m_iRequestFlag = 0;
    }



    if (iFinalRequestFlag & RQ_Q) {
        m_eCurQState = static_cast<SKILL_Q_STATE>((static_cast<_uint>(m_eCurQState)) % static_cast<_uint>(Q_STATE_END));

        Enter_State(static_cast<ACTION_STATE>(static_cast<_uint>(ACTION_STATE::Q1) + static_cast<_uint>(m_eCurQState)));
        m_pMoveCom->Stop_Move_To_Pos();

        m_eCurQState = static_cast<SKILL_Q_STATE>(static_cast<_uint>(m_eCurQState) + 1);
    }
    else if (iFinalRequestFlag & RQ_E) {
        Enter_State(ACTION_STATE::E);
        m_pMoveCom->Stop_Move_To_Pos();
    }
    else if (iFinalRequestFlag & RQ_R) {
        Enter_State(ACTION_STATE::R);
        m_pMoveCom->Stop_Move_To_Pos();
    }
    else if (iFinalRequestFlag & RQ_REST) {
        Enter_State(ACTION_STATE::REST_P);
        m_pMoveCom->Stop_Move_To_Pos();
    }
    else if (iFinalRequestFlag & RQ_CRAFT) {
        Enter_State(ACTION_STATE::CRAFT_P);
        m_pMoveCom->Stop_Move_To_Pos();
    }
    else if (iFinalRequestFlag & RQ_COLLECT) {
        Enter_State(ACTION_STATE::COLLECT_P);
        m_pMoveCom->Stop_Move_To_Pos();
    }
    else if (iFinalRequestFlag & RQ_COOK) {
        Enter_State(ACTION_STATE::COOK_P);
        m_pMoveCom->Stop_Move_To_Pos();
    }
    else if (iFinalRequestFlag & RQ_ATK) {
        Enter_State(ACTION_STATE::ATK_P_P);
        m_pMoveCom->Stop_Move_To_Pos();
    }
    else if (iFinalRequestFlag & RQ_RUN && !(m_iControlFlag & BLOCK_RUN)) {
        m_pMoveCom->Move_To_Pos(m_vTargetPos);
        Enter_State(ACTION_STATE::RUN_P);
    }
    else if (iFinalRequestFlag & RQ_IDLE) {
        m_pMoveCom->Stop_Move_To_Pos();
        Enter_State(ACTION_STATE::IDLE_P);
    }

    iFinalRequestFlag = 0;
}

void CPlayer::Enter_State(ACTION_STATE eNewState)
{
    // 상태가 바뀌었을 때 한 번만 실행
    m_iCurState = eNewState;

    if (m_iCurState != m_iPreState) {

        switch (m_iCurState) {
            case ACTION_STATE::ATK_P:
            {
                break;
            }

            case ACTION_STATE::ATK_P_P:
            {
                break;
            }

            case ACTION_STATE::Q1:
            {
                break;
            }

            case ACTION_STATE::Q2:
            {
                break;
            }

            case ACTION_STATE::Q3:
            {
                break;
            }

            case ACTION_STATE::E:
            {
                break;
            }

            case ACTION_STATE::R:
            {
                break;
            }

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

            case ACTION_STATE::CRAFT_P:
            {
                break;
            }

            case ACTION_STATE::COLLECT_P:
            {
                break;
            }

            case ACTION_STATE::COOK_P:
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

    if (m_pGameInstance->Key_Down(DIK_A)) {
        m_iRequestFlag |= REQUEST_FLAG::RQ_ATK;
    }

    if (m_pGameInstance->Key_Down(DIK_Q)) {
        m_iRequestFlag |= REQUEST_FLAG::RQ_Q;
    }

    if (m_pGameInstance->Key_Down(DIK_E)) {
        m_iRequestFlag |= REQUEST_FLAG::RQ_E;
    }
    if (m_pGameInstance->Key_Down(DIK_R)) {
        m_iRequestFlag |= REQUEST_FLAG::RQ_R;
    }

    if (m_pGameInstance->Key_Down(DIK_S)) {
        m_iRequestFlag |= REQUEST_FLAG::RQ_IDLE;
    }

    if (m_pGameInstance->Key_Down(DIK_X)) {
        m_iRequestFlag |= REQUEST_FLAG::RQ_REST;
    }

    if (m_pGameInstance->Key_Down(DIK_Z)) {
        m_iRequestFlag |= REQUEST_FLAG::RQ_CRAFT;
    }

    if (m_pGameInstance->Key_Down(DIK_C)) {
        m_iRequestFlag |= REQUEST_FLAG::RQ_COLLECT;
    }

    if (m_pGameInstance->Key_Down(DIK_V)) {
        m_iRequestFlag |= REQUEST_FLAG::RQ_COOK;
    }




    if ((m_iRequestFlag & 0xffffff) && m_pBody->IsAniLock()) {
        m_pBody->RequestUnlock();
        m_iRequestFlag = 0;
    }
    m_iReserveRequestFlag |= REQUEST_FLAG::RQ_IDLE;
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
    Safe_Release(m_pBody);
    Safe_Release(m_pMoveCom);

    __super::Free();
}
