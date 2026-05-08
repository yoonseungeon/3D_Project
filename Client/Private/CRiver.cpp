#include "CRiver.h"

#include "CGameInstance.h"

CRiver::CRiver(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CRiver::CRiver(const CRiver& Prototype)
    : CGameObject{ Prototype }
{

}

HRESULT CRiver::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRiver::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CRiver::Priority_Update(_float fTimeDelta)
{

}

void CRiver::Parallel_Update(_float fTimeDelta)
{
}

void CRiver::Update(_float fTimeDelta)
{

}

void CRiver::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::BLEND, this);
}

HRESULT CRiver::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    const _uint iWaterPass = 2;

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pShaderCom->Begin(iWaterPass)))
            return E_FAIL;

        //i 번째 메쉬 버퍼 연결 및 draw
        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CRiver::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Component_Model_River",
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CRiver::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CRiver* CRiver::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRiver* pInstance = new CRiver(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CRiver");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRiver::Clone(void* pArg)
{
    CRiver* pInstance = new CRiver(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CRiver");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRiver::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}

