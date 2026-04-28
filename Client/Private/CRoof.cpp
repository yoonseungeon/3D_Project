#include "CRoof.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"

CRoof::CRoof(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CRoof::CRoof(const CRoof& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CRoof::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRoof::Initialize(void* pArg)
{
    ROOF_DESC* pDesc = static_cast<ROOF_DESC*>(pArg);

    m_wstrModelTag = pDesc->wstrModelTag;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pImGameManager = CInGame_Manager::GetInstance();
    Safe_AddRef(m_pImGameManager);

    return S_OK;
}

void CRoof::Priority_Update(_float fTimeDelta)
{
    vPlayerPos = m_pImGameManager->Get_PlayerPos();
}

void CRoof::Parallel_Update(_float fTimeDelta)
{
    const MODEL_LOCAL_MIN_MAX& tLocalPos =  m_pModelCom->Get_LocalXYZ();

    const _float fRoofMargin = { 1.f };

    // 지붕 로컬 == 지붕 월드
    if (vPlayerPos.x >= tLocalPos.vMin.x - fRoofMargin
        && vPlayerPos.x <= tLocalPos.vMax.x + fRoofMargin
        && vPlayerPos.z >= tLocalPos.vMin.z - fRoofMargin
        && vPlayerPos.z <= tLocalPos.vMax.z + fRoofMargin
       )
    {
        m_bIsInactive = true;
    }
    else
    {
        m_bIsInactive = false;
    }
}

void CRoof::Update(_float fTimeDelta)
{
}

void CRoof::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true)
    {
        return;
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::NONBLEND, this);
}

HRESULT CRoof::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    size_t iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), MyTextureType_DIFFUSE, 0)))
            continue;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        //i 번째 메쉬 버퍼 연결 및 draw
        if (FAILED(m_pModelCom->Render(static_cast<_uint>(i))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CRoof::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), m_wstrModelTag,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CRoof::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CRoof* CRoof::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRoof* pInstance = new CRoof(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CRoof");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRoof::Clone(void* pArg)
{
    CRoof* pInstance = new CRoof(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CRoof");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRoof::Free()
{
    Safe_Release(m_pImGameManager);

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
