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

    Enter_State(PLAYER_STATE::P_IDLE);

    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    Player_Input(fTimeDelta);

    Update_State(fTimeDelta);
    Execute_State(fTimeDelta);

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

HRESULT CPlayer::Ready_Components()
{
    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
    CBody_Player::BODY_PLAYER_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pCurParent_State = &m_iCurState;

    if (FAILED(__super::Add_PartObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
        TEXT("Body"), &BodyDesc)))
        return E_FAIL;

    CWeapon::WEAPON_DESC WeaponDesc{};
    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponDesc.pCurParent_State = &m_iCurState;

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

void CPlayer::Update_State(_float fTimeDelta)
{
    switch (m_iCurState) {
        case PLAYER_STATE::P_IDLE:
        {

            break;
        }

        case PLAYER_STATE::P_RUN:
        {

            break;
        }
    }
}

void CPlayer::Enter_State(PLAYER_STATE eNewState)
{
    m_iCurState = eNewState;

    if (m_iCurState != m_iPreState) {

        switch (m_iCurState) {
            case PLAYER_STATE::P_IDLE:
            {

                break;
            }

            case PLAYER_STATE::P_RUN:
            {

                break;
            }
        }

        m_iPreState = m_iCurState;
    }
}

void CPlayer::Execute_State(_float fTimeDelta)
{
    switch (m_iCurState) {
        case PLAYER_STATE::P_IDLE:
        {

            break;
        }

        case PLAYER_STATE::P_RUN:
        {

            break;
        }
    }
}

void CPlayer::Player_Input(_float fTimeDelta)
{
    _bool bMove{};

    if (m_pGameInstance->Key_Pressing(DIK_UP)) {
        m_pTransformCom->Go_Straight(fTimeDelta);
        Enter_State(PLAYER_STATE::P_RUN);
        bMove = true;
    }

    if (m_pGameInstance->Key_Pressing(DIK_DOWN)) {
        m_pTransformCom->Go_Backward(fTimeDelta);
        Enter_State(PLAYER_STATE::P_RUN);
        bMove = true;
    }

    if (m_pGameInstance->Key_Pressing(DIK_RIGHT)) {
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
    }

    if (m_pGameInstance->Key_Pressing(DIK_LEFT)) {

        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -fTimeDelta);
    }

    if (bMove == false) {
        Enter_State(PLAYER_STATE::P_IDLE);
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
    __super::Free();
}
