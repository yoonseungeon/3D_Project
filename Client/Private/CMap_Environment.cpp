#include "CMap_Environment.h"

#include "CGameInstance.h"

CMap_Environment::CMap_Environment(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMap_Environment::CMap_Environment(const CMap_Environment& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CMap_Environment::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMap_Environment::Initialize(void* pArg)
{
    MAP_ENVIRONMENT_DESC* pDesc = static_cast<MAP_ENVIRONMENT_DESC*>(pArg);

    m_wstrPrototype_Component_Tag = pDesc->wstrPrototype_Component_Tag;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CMap_Environment::Priority_Update(_float fTimeDelta)
{
}

void CMap_Environment::Update(_float fTimeDelta)
{
}

void CMap_Environment::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CMap_Environment::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (m_pModelCom->Get_MaterialCount(i, MyTextureType_DIFFUSE) == 0)
        {
            continue;
        }
        else
        {
            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE, 0)))
                return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        //i 번째 메쉬 버퍼 연결 및 draw
        if (FAILED(m_pModelCom->Render(static_cast<_uint>(i))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CMap_Environment::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), m_wstrPrototype_Component_Tag,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMap_Environment::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CMap_Environment* CMap_Environment::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMap_Environment* pInstance = new CMap_Environment(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CMap_Environment");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMap_Environment::Clone(void* pArg)
{
    CMap_Environment* pInstance = new CMap_Environment(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CMap_Environment");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMap_Environment::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
