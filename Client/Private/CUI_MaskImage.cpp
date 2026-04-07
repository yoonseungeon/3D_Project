#include "CUI_MaskImage.h"

#include "CGameInstance.h"

CUI_MaskImage::CUI_MaskImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Image{ pDevice, pContext }
{
}

CUI_MaskImage::CUI_MaskImage(const CUI_MaskImage& Prototype)
    : CUI_Image{ Prototype }
{
}

HRESULT CUI_MaskImage::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_MaskImage::Initialize(void* pArg)
{
    CUI_MASKIMAGE_DESC* pDesc = static_cast<CUI_MASKIMAGE_DESC*>(pArg);

    m_wstrMaskPrototypeTag = pDesc->wstrMaskPrototypeTag;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CUI_MaskImage::Priority_Update(_float fTimeDelta)
{
}

void CUI_MaskImage::Parallel_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

}

void CUI_MaskImage::Update(_float fTimeDelta)
{

}

void CUI_MaskImage::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_MaskImage::Render()
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

HRESULT CUI_MaskImage::Ready_Components()
{
    /* For.Com_Mask*/
    if (FAILED(__super::Add_Component(ETOUI(m_eTexPrototypeLV), m_wstrMaskPrototypeTag,
        TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pMaskCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_MaskImage::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(CUIObject::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(CUIObject::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIdx)))
        return E_FAIL;

    if (FAILED(m_pMaskCom->Bind_ShaderResource(m_pShaderCom, "g_Mask", 0)))
        return E_FAIL;

    m_pShaderCom->Bind_RawValue("g_FlipX", &m_iFlipX, sizeof(m_iFlipX));
    m_pShaderCom->Bind_RawValue("g_FlipY", &m_iFlipY, sizeof(m_iFlipY));
    m_pShaderCom->Bind_RawValue("g_Alpha", &m_fImageAlpha, sizeof(m_fImageAlpha));

    return S_OK;
}

CUI_MaskImage* CUI_MaskImage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_MaskImage* pInstance = new CUI_MaskImage(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_MaskImage");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_MaskImage::Clone(void* pArg)
{
    CUI_MaskImage* pInstance = new CUI_MaskImage(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_MaskImage");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_MaskImage::Free()
{
    Safe_Release(m_pMaskCom);

    __super::Free();
}
