#include "CCamera_Free.h"
#include "CGameInstance.h"

#include "CInGame_Manager.h"

#include "CAbstractPlayer.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& Prototype)
    : CCamera{ Prototype }
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
    CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_fMouseSensor = pDesc->fMouseSensor;

    m_vDefaultPos = { 4.5f, 9.f, -4.5f };

    return S_OK;
}

void CCamera_Free::Priority_Update(_float fTimeDelta)
{
    //if (m_pGameInstance->Key_Down(DIK_T) || m_pGameInstance->Key_Down(DIK_TAB))
    //{
    //    m_bLock = !m_bLock;
    //}

    //if (m_bLock == true) {
    //    return;
    //}

    //_float fSpeed = 0.05f;

    //if (m_pGameInstance->Key_Pressing(DIK_LSHIFT))
    //{
    //    fSpeed *= 10.f;
    //}

    //if (m_pGameInstance->Key_Pressing(DIK_W))
    //{
    //    m_pTransformCom->Go_Straight(fTimeDelta * fSpeed);
    //}
    //if (m_pGameInstance->Key_Pressing(DIK_S))
    //{
    //    m_pTransformCom->Go_Backward(fTimeDelta * fSpeed);
    //}
    //if (m_pGameInstance->Key_Pressing(DIK_A))
    //{
    //    m_pTransformCom->Go_Left(fTimeDelta * fSpeed);
    //}
    //if (m_pGameInstance->Key_Pressing(DIK_D))
    //{
    //    m_pTransformCom->Go_Right(fTimeDelta * fSpeed);
    //}

    //if (m_pGameInstance->Key_Pressing(DIK_Q))
    //{
    //    m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -fSpeed * fTimeDelta, XMConvertToRadians(180.f));
    //}

    //if (m_pGameInstance->Key_Pressing(DIK_E))
    //{
    //    m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fSpeed * fTimeDelta, XMConvertToRadians(180.f));
    //}
    //if (m_pGameInstance->Key_Pressing(DIK_2))
    //{
    //    m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), -fSpeed * fTimeDelta, XMConvertToRadians(180.f));
    //}

    //if (m_pGameInstance->Key_Pressing(DIK_3))
    //{
    //    m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fSpeed * fTimeDelta, XMConvertToRadians(180.f));
    //}

    CTransform* pTransform = nullptr;

    CAbstractPlayer* pPlayer = CInGame_Manager::GetInstance()->Get_Player();
    if (pPlayer != nullptr)
        pTransform = pPlayer->Get_TransformCom();

    if (pTransform != nullptr)
    {
        _float3 vCameraPos = m_vDefaultPos;
        _vector vPlayerPos = pTransform->Get_State(STATE::POSITION);
        m_pTransformCom->Set_Pos(XMVectorSetW(XMLoadFloat3(&m_vDefaultPos) + XMVectorSetY(vPlayerPos, 0.f), 1.f));
    }
    else
    {
        m_pTransformCom->Set_Pos(XMVectorSetW(XMLoadFloat3(&m_vDefaultPos), 1.f));
    }

    __super::Priority_Update(fTimeDelta);
}

void CCamera_Free::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CCamera_Free::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CCamera_Free::Render()
{
    return S_OK;
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CCamera_Free");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
    CCamera_Free* pInstance = new CCamera_Free(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CCamera_Free");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Free::Free()
{
    __super::Free();
}
