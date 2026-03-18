#include "CBackGround.h"

#include "CGameInstance.h"

CBackGround::CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CBackGround::CBackGround(const CBackGround& Prototype)
    : CGameObject{ Prototype }
{

}

HRESULT CBackGround::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{
    BACKGROUND_DESC* pDesc = static_cast<BACKGROUND_DESC*>(pArg);


    GAMEOBJECT_DESC* pParent = static_cast<GAMEOBJECT_DESC*>(pDesc);
    if (FAILED(__super::Initialize(pParent)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CBackGround::Priority_Update(_float fTimeDelta)
{
    int a = 10;
}

void CBackGround::Update(_float fTimeDelta)
{
    int a = 10;
}

void CBackGround::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::PRIORITY, this);
}

HRESULT CBackGround::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBackGround::Ready_Components()
{
    m_pShaderCom = dynamic_cast<CShader*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex")));
    if (m_pShaderCom == nullptr)
        return E_FAIL;

    m_pVIBufferCom = dynamic_cast<CVIBuffer_Rect*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect")));
    if (m_pVIBufferCom == nullptr)
        return E_FAIL;

    m_pTextureCom = dynamic_cast<CTexture*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ETOUI(LEVEL::LOGO), TEXT("Prototype_Component_Texture_BackGround")));
    if (m_pTextureCom == nullptr)
        return E_FAIL;

    return S_OK;
}

HRESULT CBackGround::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float4x4 IdentityMatrix{};
    XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &IdentityMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &IdentityMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
        return E_FAIL;

    return S_OK;
}

CBackGround* CBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBackGround* pInstance = new CBackGround(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CBackGround");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBackGround::Clone(void* pArg)
{
    CBackGround* pInstance = new CBackGround(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CBackGround");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBackGround::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
