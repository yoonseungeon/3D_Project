#include "CMap_Grass.h"

#include "CGameInstance.h"

CMap_Grass::CMap_Grass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMap_Grass::CMap_Grass(const CMap_Grass& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CMap_Grass::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMap_Grass::Initialize(void* pArg)
{
    MAP_GRASS_DESC* pDesc = static_cast<MAP_GRASS_DESC*>(pArg);

    m_wstrPrototype_Component_Tag = pDesc->wstrPrototype_Component_Tag;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CMap_Grass::Priority_Update(_float fTimeDelta)
{
}

void CMap_Grass::Update(_float fTimeDelta)
{
}

void CMap_Grass::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CMap_Grass::Render()
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

HRESULT CMap_Grass::Ready_Components()
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

HRESULT CMap_Grass::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CMap_Grass* CMap_Grass::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMap_Grass* pInstance = new CMap_Grass(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CMap_Grass");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMap_Grass::Clone(void* pArg)
{
    CMap_Grass* pInstance = new CMap_Grass(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CMap_Grass");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMap_Grass::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
