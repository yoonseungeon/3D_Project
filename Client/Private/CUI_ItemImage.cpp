#include "CMapSelectBtn.h"

#include "CGameInstance.h"
#include "CUI_ItemImage.h"

CUI_ItemImage::CUI_ItemImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{
}

CUI_ItemImage::CUI_ItemImage(const CUI_ItemImage& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CUI_ItemImage::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_ItemImage::Initialize(void* pArg)
{
    CUI_ITEMIMAGE_DESC* pDesc = static_cast<CUI_ITEMIMAGE_DESC*>(pArg);

    m_fScaleRatioSlotY = pDesc->fScaleRatioY;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CUI_ItemImage::Priority_Update(_float fTimeDelta)
{
}

void CUI_ItemImage::Parallel_Update(_float fTimeDelta)
{
}

void CUI_ItemImage::Update(_float fTimeDelta)
{

}

void CUI_ItemImage::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true)
    {
        return;
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_ItemImage::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(m_eBlendState)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_ItemImage::Ready_Components()
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
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Item"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* For.Com_Image*/
    if (FAILED(__super::Add_Component(ETOUI(m_eTexPrototypeLV), TEXT("Prototype_Image_Item"),
        TEXT("Com_Image"), reinterpret_cast<CComponent**>(&m_pImageCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_ItemImage::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIdx)))
        return E_FAIL;

    m_pShaderCom->Bind_RawValue("g_FlipX", &m_iFlipX, sizeof(m_iFlipX));
    m_pShaderCom->Bind_RawValue("g_FlipY", &m_iFlipY, sizeof(m_iFlipY));
    m_pShaderCom->Bind_RawValue("g_Alpha", &m_fImageAlpha, sizeof(m_fImageAlpha));

    return S_OK;
}

void CUI_ItemImage::Set_CurItem(int iItemIdx, _uint iItemCnt)
{
    m_iTexIdx = iItemIdx;
    m_iCnt = iItemCnt;

    auto tViewportDesc = m_pGameInstance->Get_ViewportDesc();

    const _float fScaleRatioY = tViewportDesc.y * m_fScaleRatioSlotY * 0.7f;
    const _float fScaleRatioX = fScaleRatioY / m_pImageCom->Get_Height(iItemIdx) * m_pImageCom->Get_Width(iItemIdx);

    m_pTransformCom->Set_Scale(fScaleRatioX, fScaleRatioY, 1.f);
}

CUI_ItemImage* CUI_ItemImage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_ItemImage* pInstance = new CUI_ItemImage(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_ItemImage");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_ItemImage::Clone(void* pArg)
{
    CUI_ItemImage* pInstance = new CUI_ItemImage(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_ItemImage");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_ItemImage::Free()
{
    Safe_Release(m_pImageCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
