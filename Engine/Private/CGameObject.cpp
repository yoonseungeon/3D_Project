#include "CGameObject.h"

#include "CGameInstance.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

CGameObject::CGameObject(const CGameObject& Prototype)
    : m_pDevice{ Prototype.m_pDevice }
    , m_pContext{ Prototype.m_pContext }
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CGameObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
    CTransform::TRANSFORM_DESC* pParent = nullptr;
    if (pArg != nullptr)
    {
        GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
        m_iFlag = pDesc->iFlag;
        pParent = static_cast<CTransform::TRANSFORM_DESC*>(pDesc);
    }

    /* 객체 당 부여되어야할 트랜스폼 컴포넌트를 생성한다. */
    m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
    if (m_pTransformCom == nullptr)
        return E_FAIL;

    /* 객체에게 부여된 초기 월드 상태를 트래스폼에게 동기화시킨다. */
    if (FAILED(m_pTransformCom->Initialize(pParent)))
        return E_FAIL;

    return S_OK;
}

void CGameObject::Priority_Update(_float fTimeDelta)
{

}

void CGameObject::Update(_float fTimeDelta)
{

}

void CGameObject::Late_Update(_float fTimeDelta)
{

}

HRESULT CGameObject::Render()
{
    return S_OK;
}

void CGameObject::Free()
{
    Safe_Release(m_pTransformCom);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    __super::Free();
}
