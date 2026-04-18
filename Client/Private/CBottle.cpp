#include "CBottle.h"

#include "CGameInstance.h"
#include "CPlayer.h"

CBottle::CBottle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CBottle::CBottle(const CBottle& Prototype)
    : CPartObject{ Prototype }
{
}

HRESULT CBottle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBottle::Initialize(void* pArg)
{
    BOTTLE_DESC* pDesc = static_cast<CBottle::BOTTLE_DESC*>(pArg);

    m_pSocketBoneMatrix = pDesc->pSocketBoneMatrix;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CBottle::Priority_Update(_float fTimeDelta)
{
}

void CBottle::Parallel_Update(_float fTimeDelta)
{
}

void CBottle::Update(_float fTimeDelta)
{
}

void CBottle::Late_Update(_float fTimeDelta)
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
}

HRESULT CBottle::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        //i 번째 메쉬 버퍼 연결 및 draw
        if (FAILED(m_pModelCom->Render(static_cast<_uint>(i))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CBottle::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Bottle"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBottle::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CBottle* CBottle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBottle* pInstance = new CBottle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CBottle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBottle::Clone(void* pArg)
{
    CBottle* pInstance = new CBottle(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CBottle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBottle::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
