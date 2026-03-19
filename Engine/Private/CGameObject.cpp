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
        pParent = &(pDesc->tTransformDesc);
    }

    /* 객체 당 부여되어야할 트랜스폼 컴포넌트를 생성한다. */
    m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
    if (m_pTransformCom == nullptr)
        return E_FAIL;

    /* 객체에게 부여된 초기 월드 상태를 트랜스폼에게 동기화시킨다. */
    if (FAILED(m_pTransformCom->Initialize(pParent)))
        return E_FAIL;

    m_Components.emplace(g_strTransformTag, m_pTransformCom);
    Safe_AddRef(m_pTransformCom);

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

HRESULT CGameObject::Add_Component(_uint iPrototypeLevelIndex, const _wstring& wstrPrototypeTag, const _wstring& wstrComponentTag, CComponent** ppOut, void* pArg)
{
    CComponent* pComponent = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, iPrototypeLevelIndex, wstrPrototypeTag, pArg));
    if (pComponent == nullptr)
    {
        MSG_BOX("Failed to Created: Component - GameObject.cpp");
        return E_FAIL;
    }

    auto iter = m_Components.emplace(wstrComponentTag, pComponent);

    if (iter.second == false)
    {
        Safe_Release(pComponent);
        MSG_BOX("Failed to Inserted: Component name is duplicate - GameObject.cpp");
        return E_FAIL;
    }

    *ppOut = pComponent;

    Safe_AddRef(pComponent);

    return S_OK;
}

CComponent* CGameObject::Find_Component(const _wstring& wstrComponentTag)
{
    auto iter = m_Components.find(wstrComponentTag);
    if (iter == m_Components.end())
        return nullptr;

    return iter->second;
}

void CGameObject::Free()
{
    for (auto& Pair : m_Components)
        Safe_Release(Pair.second);
    m_Components.clear();

    Safe_Release(m_pTransformCom);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    __super::Free();
}
