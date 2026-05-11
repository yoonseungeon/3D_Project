#include "CFryingPan.h"

#include "CGameInstance.h"

CFryingPan::CFryingPan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CFryingPan::CFryingPan(const CFryingPan& Prototype)
    : CPartObject{ Prototype }
{
}

HRESULT CFryingPan::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFryingPan::Initialize(void* pArg)
{
    FRYINGPAN_DESC* pDesc = static_cast<CFryingPan::FRYINGPAN_DESC*>(pArg);

    m_pSocketBoneMatrix = pDesc->pSocketBoneMatrix;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CFryingPan::Priority_Update(_float fTimeDelta)
{
}

void CFryingPan::Parallel_Update(_float fTimeDelta)
{

}

void CFryingPan::Update(_float fTimeDelta)
{
    if (m_bIsInactive == true)
    {
        return;
    }

    m_pModelCom->Play_Animation(fTimeDelta);
}

void CFryingPan::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true)
    {
        return;
    }

    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketBoneMatrix);

    //for (size_t i = 0; i < 3; ++i)
    //    SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

    Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * SocketMatrix);

    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(RENDERID::SHADOW, this);
}

HRESULT CFryingPan::Render()
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

        if (FAILED(m_pShaderCom->Begin(ETOUI(ANIMMESH_SHADER::DEFAULT))))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CFryingPan::Render_Shadow()
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

HRESULT CFryingPan::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_FryingPan"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CFryingPan::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CFryingPan* CFryingPan::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFryingPan* pInstance = new CFryingPan(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CFryingPan");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFryingPan::Clone(void* pArg)
{
    CFryingPan* pInstance = new CFryingPan(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CFryingPan");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFryingPan::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
