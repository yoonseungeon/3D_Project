#include "CBody_Wolf.h"

#include "CGameInstance.h"

CBody_Wolf::CBody_Wolf(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CBody_Wolf::CBody_Wolf(const CBody_Wolf& Prototype)
    : CPartObject{ Prototype }
{
}

HRESULT CBody_Wolf::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_Wolf::Initialize(void* pArg)
{
    BODY_WOLF_DESC* pDesc = static_cast<BODY_WOLF_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CBody_Wolf::Priority_Update(_float fTimeDelta)
{

}

void CBody_Wolf::Parallel_Update(_float fTimeDelta)
{
    m_pModelCom->Play_Animation(fTimeDelta);
}

void CBody_Wolf::Update(_float fTimeDelta)
{
}

void CBody_Wolf::Late_Update(_float fTimeDelta)
{
    __super::Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(RENDERID::SHADOW, this);

}

HRESULT CBody_Wolf::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE, 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CBody_Wolf::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    // ±¤¿ø ºä½ºÆäÀÌ½º, Åõ¿µ Çà·Ä ´øÁ®ÁÜ
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Shadow_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Shadow_Transform(D3DTS::PROJ))))
        return E_FAIL;

    size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(ETOUI(ANIMMESH_SHADER::SHADOW))))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

const _float4x4* CBody_Wolf::Get_BoneMatrixPtr(const _char* pBoneName) const
{
    return m_pModelCom->Get_BoneMatrixPtr(pBoneName);
}

HRESULT CBody_Wolf::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Wolf"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Wolf::Bind_ShaderResources()
{
    //if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
    //    return E_FAIL;

    // ºÎ¸ð Çà·ÄÀÌ °öÇØÁø Çà·Ä ´øÁü
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CBody_Wolf* CBody_Wolf::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Wolf* pInstance = new CBody_Wolf(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CBody_Wolf");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_Wolf::Clone(void* pArg)
{
    CBody_Wolf* pInstance = new CBody_Wolf(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CBody_Wolf");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_Wolf::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}