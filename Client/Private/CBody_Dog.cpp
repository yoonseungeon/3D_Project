#include "CBody_Dog.h"

#include "CGameInstance.h"

CBody_Dog::CBody_Dog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CBody_Dog::CBody_Dog(const CBody_Dog& Prototype)
    : CPartObject{ Prototype }
{
}

HRESULT CBody_Dog::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_Dog::Initialize(void* pArg)
{
    BODY_DOG_DESC* pDesc = static_cast<BODY_DOG_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CBody_Dog::Priority_Update(_float fTimeDelta)
{

}

void CBody_Dog::Parallel_Update(_float fTimeDelta)
{
    m_pModelCom->Play_Animation(fTimeDelta);
}

void CBody_Dog::Update(_float fTimeDelta)
{
}

void CBody_Dog::Late_Update(_float fTimeDelta)
{
    __super::Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    _float4 vPos{};
    memcpy(&vPos, m_CombinedWorldMatrix.m[3], sizeof(_float4));

    if (m_pGameInstance->isIn_Frustum_WorldSpace(XMLoadFloat4(&vPos), 2.f) == false)
        return;

    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(RENDERID::SHADOW, this);
    m_pGameInstance->Add_RenderGroup(RENDERID::OUTLINE, this);
}

HRESULT CBody_Dog::Render()
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

HRESULT CBody_Dog::Render_Shadow()
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

HRESULT CBody_Dog::Render_OutLine()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(ETOUI(ANIMMESH_SHADER::OUTLINE))))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

const _float4x4* CBody_Dog::Get_BoneMatrixPtr(const _char* pBoneName) const
{
    return m_pModelCom->Get_BoneMatrixPtr(pBoneName);
}

HRESULT CBody_Dog::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Dog"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Dog::Bind_ShaderResources()
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

CBody_Dog* CBody_Dog::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Dog* pInstance = new CBody_Dog(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CBody_Dog");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_Dog::Clone(void* pArg)
{
    CBody_Dog* pInstance = new CBody_Dog(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CBody_Dog");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_Dog::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}