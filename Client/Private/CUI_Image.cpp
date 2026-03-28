#include "CUI_Image.h"

#include "CGameInstance.h"

CUI_Image::CUI_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CUI_Image::CUI_Image(const CUI_Image& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CUI_Image::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Image::Initialize(void* pArg)
{
    CUI_IMAGE_DESC* pDesc = static_cast<CUI_IMAGE_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CUI_Image::Priority_Update(_float fTimeDelta)
{
}

void CUI_Image::Parallel_Update(_float fTimeDelta)
{
}

void CUI_Image::Update(_float fTimeDelta)
{
}

void CUI_Image::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_Image::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(ETOUI(m_eBlendState))))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Image::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* For.Com_Texture*/
    if (FAILED(__super::Add_Component(ETOUI(m_eTexPrototypeLV), m_wstrTexturePrototypeTag,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Image::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    m_pShaderCom->Bind_RawValue("g_FlipX", &m_iFlipX, sizeof(m_iFlipX));
    m_pShaderCom->Bind_RawValue("g_FlipY", &m_iFlipY, sizeof(m_iFlipY));
    m_pShaderCom->Bind_RawValue("g_Alpha", &m_fImageAlpha, sizeof(m_fImageAlpha));
    
    return S_OK;
}

CUI_Image* CUI_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Image* pInstance = new CUI_Image(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_Image");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Image::Clone(void* pArg)
{
    CUI_Image* pInstance = new CUI_Image(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_Image");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Image::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
