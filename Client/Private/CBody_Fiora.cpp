#include "CBody_Fiora.h"

#include "CGameInstance.h"

CBody_Fiora::CBody_Fiora(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CBody_Fiora::CBody_Fiora(const CBody_Fiora& Prototype)
    : CPartObject{ Prototype }
{
}

HRESULT CBody_Fiora::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_Fiora::Initialize(void* pArg)
{
    BODY_FIORA_DESC* pDesc = static_cast<BODY_FIORA_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fOutLineLength = 0.013f;
    m_vOutLineColor = { 0.f, 0.f, 0.f, 1.f };

    return S_OK;
}

void CBody_Fiora::Priority_Update(_float fTimeDelta)
{

}

void CBody_Fiora::Parallel_Update(_float fTimeDelta)
{
    m_pModelCom->Play_Animation(fTimeDelta);
}

void CBody_Fiora::Update(_float fTimeDelta)
{
}

void CBody_Fiora::Late_Update(_float fTimeDelta)
{
    __super::Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(RENDERID::SHADOW, this);
    m_pGameInstance->Add_RenderGroup(RENDERID::OUTLINE, this);
}

HRESULT CBody_Fiora::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        // 버그 mesh
        if (i == 1)
            continue;

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

HRESULT CBody_Fiora::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    // 광원 뷰스페이스, 투영 행렬 던져줌
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

HRESULT CBody_Fiora::Render_OutLine()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(Bind_OutLineShaderResources()))
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

const _float4x4* CBody_Fiora::Get_BoneMatrixPtr(const _char* pBoneName) const
{
    return m_pModelCom->Get_BoneMatrixPtr(pBoneName);
}

HRESULT CBody_Fiora::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Fiora"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Fiora::Bind_ShaderResources()
{
    // 부모 행렬이 곱해진 행렬 던짐
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Fiora::Bind_OutLineShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_OutLineLength", &m_fOutLineLength, sizeof(m_fOutLineLength))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_OutLineColor", &m_vOutLineColor, sizeof(m_vOutLineColor))))
        return E_FAIL;

    return S_OK;
}

CBody_Fiora* CBody_Fiora::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Fiora* pInstance = new CBody_Fiora(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CBody_Bat");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_Fiora::Clone(void* pArg)
{
    CBody_Fiora* pInstance = new CBody_Fiora(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CBody_Bat");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_Fiora::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}