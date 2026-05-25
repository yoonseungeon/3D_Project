#include "CWaterPlan.h"

#include "CGameInstance.h"

CWaterPlan::CWaterPlan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CWaterPlan::CWaterPlan(const CWaterPlan& Prototype)
    : CGameObject{ Prototype }
{

}

HRESULT CWaterPlan::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWaterPlan::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fAlpha = 0.5f;
    m_vColor = { 0.157f, 0.314f, 0.392f };

    return S_OK;
}

void CWaterPlan::Priority_Update(_float fTimeDelta)
{

}

void CWaterPlan::Parallel_Update(_float fTimeDelta)
{
}

void CWaterPlan::Update(_float fTimeDelta)
{

}

void CWaterPlan::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::BLEND, this);
}

HRESULT CWaterPlan::Render()
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

HRESULT CWaterPlan::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Component_Model_WaterPlan",
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CWaterPlan::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(m_fAlpha));
    m_pShaderCom->Bind_RawValue("g_Color", &m_vColor, sizeof(m_vColor));

    return S_OK;
}

CWaterPlan* CWaterPlan::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWaterPlan* pInstance = new CWaterPlan(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CWaterPlan");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWaterPlan::Clone(void* pArg)
{
    CWaterPlan* pInstance = new CWaterPlan(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CWaterPlan");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWaterPlan::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}

