#include "CUI_Day.h"

#include "CGameInstance.h"

#include "CInGame_Manager.h"

CUI_Day::CUI_Day(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CUI_Day::CUI_Day(const CUI_Day& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CUI_Day::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Day::Initialize(void* pArg)
{
    CUI_DAY_DESC* pDesc = static_cast<CUI_DAY_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_vColor = { 0.1294118f, 0.1607843f, 0.2f };
    m_fImageAlpha = 0.8f;

    m_fCenterX = (pDesc->fPosRatioX + 0.5f) * static_cast<_float>(g_iWinSizeX);
    m_fCenterY = -(pDesc->fPosRatioY - 0.5f) * static_cast<_float>(g_iWinSizeY);

    return S_OK;
}

void CUI_Day::Priority_Update(_float fTimeDelta)
{
}

void CUI_Day::Parallel_Update(_float fTimeDelta)
{
}

void CUI_Day::Update(_float fTimeDelta)
{
}

void CUI_Day::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_Day::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(BLENDSTATE::COLOR_ALPHABLEND)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;


    if (FAILED(Render_Text()))
        return E_FAIL;

    return S_OK;
}

void CUI_Day::Set_Day(_uint iDay)
{
    _uint iTempDay = (iDay + 1) / 2;

    m_wstrDay = to_wstring(iTempDay) + L"ÀÏ Â÷";
}

HRESULT CUI_Day::Ready_Components()
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
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Img_HUD_SideFrame"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Day::Bind_ShaderResources()
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

HRESULT CUI_Day::Render_Text()
{
    const float fAdjustFontSize = fDefaultFontSize * 0.4f;

    const _float2 vLength = m_pGameInstance->Cal_FontLength(L"Font_Pretendard_Middle", m_wstrDay);

    const _float fTextStartX = m_fCenterX - vLength.x * fAdjustFontSize * 0.5f;
    const _float fTextStartY = m_fCenterY - vLength.y * fAdjustFontSize * 0.5f;

    m_pGameInstance->Draw_Text(TEXT("Font_Pretendard_Middle"), m_wstrDay.data(), _float2(fTextStartX, fTextStartY),
        XMVectorSet(1.f, 1.f, 1.f, 1.f), _float2(fAdjustFontSize, fAdjustFontSize) );

    return S_OK;
}

CUI_Day* CUI_Day::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Day* pInstance = new CUI_Day(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_DayIcon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Day::Clone(void* pArg)
{
    CUI_Day* pInstance = new CUI_Day(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_DayIcon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Day::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}