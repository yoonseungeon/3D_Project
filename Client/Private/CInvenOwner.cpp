#include "CInvenOwner.h"

#include "CGameInstance.h"

#include "CPartObject.h"
#include "CInventory.h"

CInvenOwner::CInvenOwner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject{ pDevice, pContext }
{
}

CInvenOwner::CInvenOwner(const CInvenOwner& Prototype)
    : CContainerObject{ Prototype }
{
}

HRESULT CInvenOwner::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInvenOwner::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pInvetory = CInventory::Create();

    return S_OK;
}

void CInvenOwner::Priority_Update(_float fTimeDelta)
{
    // PartObject들은 GameObject_Manager에 안 들어간다.
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Priority_Update(fTimeDelta);
    }
}

void CInvenOwner::Parallel_Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Parallel_Update(fTimeDelta);
    }
}

void CInvenOwner::Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Update(fTimeDelta);
    }
}

void CInvenOwner::Late_Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Late_Update(fTimeDelta);
    }
}

HRESULT CInvenOwner::Render()
{
    return S_OK;
}

void CInvenOwner::Free()
{
    Safe_Release(m_pInvetory);

    __super::Free();
}