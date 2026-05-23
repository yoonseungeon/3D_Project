#include "CLumia_NavMesh.h"

#include "CGameInstance.h"

CLumia_NavMesh::CLumia_NavMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CLumia_NavMesh::CLumia_NavMesh(const CLumia_NavMesh& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CLumia_NavMesh::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLumia_NavMesh::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CLumia_NavMesh::Priority_Update(_float fTimeDelta)
{
}

void CLumia_NavMesh::Update(_float fTimeDelta)
{
}

void CLumia_NavMesh::Late_Update(_float fTimeDelta)
{
}

HRESULT CLumia_NavMesh::Render()
{
#ifdef _DEBUG
    //m_pNavigationCom->Render();
#endif

    return S_OK;
}

HRESULT CLumia_NavMesh::Ready_Components()
{
    /* For.Com_Navigation */
    CNavigation::NAVIGATION_DESC NaviDesc{};

    NaviDesc.bIsGround = true;
    NaviDesc.pParentMarix = m_pTransformCom->Get_WorldMatrixPtr();

    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLumia_NavMesh::Bind_ShaderResources()
{
    return S_OK;
}

CLumia_NavMesh* CLumia_NavMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLumia_NavMesh* pInstance = new CLumia_NavMesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CLumia_NavMesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLumia_NavMesh::Clone(void* pArg)
{
    CLumia_NavMesh* pInstance = new CLumia_NavMesh(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CLumia_NavMesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLumia_NavMesh::Free()
{
    Safe_Release(m_pNavigationCom);

    __super::Free();
}
