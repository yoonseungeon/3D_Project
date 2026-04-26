#include "CUI_InvenItemBg.h"

#include "CGameInstance.h"

CUI_InvenItemBg::CUI_InvenItemBg(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CUI_InvenItemBg::CUI_InvenItemBg(const CUI_InvenItemBg& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CUI_InvenItemBg::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_InvenItemBg::Initialize(void* pArg)
{
    CUI_INVENITEMBG_DESC* pDesc = static_cast<CUI_INVENITEMBG_DESC*>(pArg);

    m_fScaleRatioX = pDesc->fScaleRatioX;
    m_fScaleRatioY = pDesc->fScaleRatioY;
    m_fPosRatioX = pDesc->fPosRatioX;
    m_fPosRatioY = pDesc->fPosRatioY;

    static const _float fStartAdjust{ 0.5f };

    // ½½·Ô Å©±â ºñ·Ê·Î ³»¸²
    static const _float fAdjustX = pDesc->fScaleRatioX * static_cast<_float>(g_iWinSizeX) * 0.28f;
    static const _float fAdjustY = pDesc->fScaleRatioY * static_cast<_float>(g_iWinSizeY) * 0.1f;

    m_fImagePosX = (pDesc->fPosRatioX + fStartAdjust) * static_cast<_float>(g_iWinSizeX) + fAdjustX;
    m_fImagePosY = -(pDesc->fPosRatioY - fStartAdjust) * static_cast<_float>(g_iWinSizeY);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CUI_InvenItemBg::Priority_Update(_float fTimeDelta)
{
}

void CUI_InvenItemBg::Parallel_Update(_float fTimeDelta)
{
}

void CUI_InvenItemBg::Update(_float fTimeDelta)
{
}

void CUI_InvenItemBg::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_InvenItemBg::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(ETOUI(m_eBlendState))))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    static const _float fAdjustFontSize = 0.5f;

    if (m_iItemCnt > 1) {
        m_pGameInstance->Draw_Text(TEXT("Font_Pretendard_Middle"),
            m_wstrItemCnt.data(),
            _float2(m_fImagePosX, m_fImagePosY), XMVectorSet(1.f, 1.f, 1.f, 1.f),
            _float2(fDefaultFontSize * fAdjustFontSize, fDefaultFontSize * fAdjustFontSize)
        );
    }

    return S_OK;
}

void CUI_InvenItemBg::Set_ItemCnt(_uint iItemCnt)
{
    m_iItemCnt = iItemCnt;
    m_wstrItemCnt = to_wstring(m_iItemCnt);
}

HRESULT CUI_InvenItemBg::Ready_Components()
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

HRESULT CUI_InvenItemBg::Bind_ShaderResources()
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
    m_pShaderCom->Bind_RawValue("g_Color", &m_vColor, sizeof(m_vColor));

    return S_OK;
}

CUI_InvenItemBg* CUI_InvenItemBg::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_InvenItemBg* pInstance = new CUI_InvenItemBg(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_InvenItemBg");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_InvenItemBg::Clone(void* pArg)
{
    CUI_InvenItemBg* pInstance = new CUI_InvenItemBg(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_InvenItemBg");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_InvenItemBg::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
