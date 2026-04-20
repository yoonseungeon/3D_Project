#include "CContainerObject.h"

#include "CGameInstance.h"
#include "CPartObject.h"

CContainerObject::CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CContainerObject::CContainerObject(const CContainerObject& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CContainerObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CContainerObject::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CContainerObject::Priority_Update(_float fTimeDelta)
{
}

void CContainerObject::Parallel_Update(_float fTimeDelta)
{
}

void CContainerObject::Update(_float fTimeDelta)
{
}

void CContainerObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CContainerObject::Render()
{
    return S_OK;
}

CComponent* CContainerObject::Find_PartObjectComponent(const _wstring& strPartTag, const _wstring& strComponentTag)
{
    auto    iter = m_PartObjects.find(strPartTag);

    if (iter == m_PartObjects.end())
        return nullptr;

    return iter->second->Find_Component(strComponentTag);
}

HRESULT CContainerObject::Add_PartObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strPartTag, void* pArg)
{
    CPartObject* pPartObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
    if (pPartObject == nullptr)
        return E_FAIL;

    m_PartObjects.emplace(strPartTag, pPartObject);

    return S_OK;
}

void CContainerObject::Free()
{
    for (auto& Pair : m_PartObjects)
        Safe_Release(Pair.second);
    m_PartObjects.clear();

    __super::Free();
}
