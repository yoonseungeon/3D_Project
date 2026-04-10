#include "CLobbySelectBtn.h"

#include "CGameInstance.h"

CLobbySelectBtn::CLobbySelectBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Btn{ pDevice, pContext }
{

}

CLobbySelectBtn::CLobbySelectBtn(const CLobbySelectBtn& Prototype)
    : CUI_Btn{ Prototype }
{

}

HRESULT CLobbySelectBtn::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLobbySelectBtn::Initialize(void* pArg)
{
    CLOBBY_SELECT_BTN_DESC* pDesc = static_cast<CLOBBY_SELECT_BTN_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_wstrText = pDesc->wstrText;
    m_fOffsetX = pDesc->fOffsetX;
    return S_OK;
}

void CLobbySelectBtn::Priority_Update(_float fTimeDelta)
{
}

void CLobbySelectBtn::Parallel_Update(_float fTimeDelta)
{
    // m_bIsInactived의 쓰기는 Level Update에서 일어남.(Late Update 후 LevelUpdate 됨.)
    if (m_bIsInactive == true) {
        return;
    }

    __super::Update_BtnState();

    Execute_Btn(fTimeDelta);
}

void CLobbySelectBtn::Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    if (m_bIsClicked) {
        BtnClick();
        m_bIsClicked = false;
    }
}

void CLobbySelectBtn::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CLobbySelectBtn::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(m_eBlendState)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    const _float fTextStartX = static_cast<_float>(m_rcBtnRange.left) + m_fOffsetX;
    const _float fTextStartY = static_cast<_float>(m_rcBtnRange.top) + m_fHalfSizeY * 0.6f;
    const _float fAdjust = 0.65f;
    m_pGameInstance->Draw_Text(TEXT("Font_Pretendard_Middle"),
        m_wstrText.data(),
        _float2(fTextStartX, fTextStartY), XMVectorSet(1.f, 1.f, 1.f, 1.f),
        _float2(fDefaultFontSize * fAdjust, fDefaultFontSize * fAdjust)
    );

    return S_OK;
}

HRESULT CLobbySelectBtn::Ready_Components()
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

HRESULT CLobbySelectBtn::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_eCurTexState)))
        return E_FAIL;

    m_pShaderCom->Bind_RawValue("g_FlipX", &m_iFlipX, sizeof(m_iFlipX));
    m_pShaderCom->Bind_RawValue("g_FlipY", &m_iFlipY, sizeof(m_iFlipY));
    m_pShaderCom->Bind_RawValue("g_Alpha", &m_fImageAlpha, sizeof(m_fImageAlpha));

    return S_OK;
}

void CLobbySelectBtn::BtnClick()
{
    m_funcCallBack();
    m_bIsInactive = true;
}

void CLobbySelectBtn::Execute_Btn(_float fTimeDelta)
{
    m_fImageAlpha = 1.f;

    switch (m_eCurBtnState) {
    case BTN_STATE::NORMAL:
    {
        m_eCurTexState = TEX_STATE::NORMAL;
        break;
    }

    case BTN_STATE::HOVER:
    {
        m_eCurTexState = TEX_STATE::HOVER;
        break;
    }

    case BTN_STATE::PRESSED:
    {
        m_eCurTexState = TEX_STATE::NORMAL;
        break;
    }

    case BTN_STATE::CLICKED:
    {
        m_eCurTexState = TEX_STATE::NORMAL;
        m_bIsClicked = true;
        break;
    }
    }
}

CLobbySelectBtn* CLobbySelectBtn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLobbySelectBtn* pInstance = new CLobbySelectBtn(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CLobbySelectBtn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLobbySelectBtn::Clone(void* pArg)
{
    CLobbySelectBtn* pInstance = new CLobbySelectBtn(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CLobbySelectBtn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLobbySelectBtn::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
