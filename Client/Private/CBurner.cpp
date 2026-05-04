#include "CBurner.h"

#include "CGameInstance.h"

CBurner::CBurner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CBurner::CBurner(const CBurner& Prototype)
    : CPartObject{ Prototype }
{
}

HRESULT CBurner::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBurner::Initialize(void* pArg)
{
    BURNER_DESC* pDesc = static_cast<CBurner::BURNER_DESC*>(pArg);

    m_pSocketBoneMatrix = pDesc->pSocketBoneMatrix;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CBurner::Priority_Update(_float fTimeDelta)
{
}

void CBurner::Parallel_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true)
    {
        return;
    }

    m_pModelCom->Play_Animation(fTimeDelta);
}

void CBurner::Update(_float fTimeDelta)
{
}

void CBurner::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true)
    {
        return;
    }

    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketBoneMatrix);

    // SocketBone 행렬에서 위치는 맞는데, 스케일이 깨져서 스케일 1, 1, 1로 만들어 줌.
    //for (size_t i = 0; i < 3; ++i)
    //    SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

    // 자신 월드 * 소켓 * 컨테이너 부모 이 순서로 곱함
    // 자신 월드 * 소켓 * Body * 컨테이너 부모가 더 정확하지만 Body는 움직이지 않아서 항등 행렬임.
    Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * SocketMatrix);

    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CBurner::Render()
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

HRESULT CBurner::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Burner"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBurner::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CBurner* CBurner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBurner* pInstance = new CBurner(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CBurner");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBurner::Clone(void* pArg)
{
    CBurner* pInstance = new CBurner(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CBurner");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBurner::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
