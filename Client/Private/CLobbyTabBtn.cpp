#include "CLobbyTabBtn.h"

#include "CGameInstance.h"

CLobbyTabBtn::CLobbyTabBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Btn{ pDevice, pContext }
{

}

CLobbyTabBtn::CLobbyTabBtn(const CLobbyTabBtn& Prototype)
    : CUI_Btn{ Prototype }
{

}

HRESULT CLobbyTabBtn::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLobbyTabBtn::Initialize(void* pArg)
{
    CLOBBY_TAB_BTN_DESC* pDesc = static_cast<CLOBBY_TAB_BTN_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_funcLobbyCallBack = pDesc->funcLobbyCallBack;
    m_eNextStage = pDesc->eNextStage;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CLobbyTabBtn::Priority_Update(_float fTimeDelta)
{
}

void CLobbyTabBtn::Parallel_Update(_float fTimeDelta)
{
    // m_bIsInactived의 쓰기는 Level Update에서 일어남.(Late Update 후 LevelUpdate 됨.)
    if (m_bIsInactive == true) {
        return;
    }

    __super::Update_BtnState();

    Execute_Btn(fTimeDelta);
}

void CLobbyTabBtn::Update(_float fTimeDelta)
{
    if (m_bIsClicked) {
        BtnClick();
        m_bIsClicked = false;
    }
}

void CLobbyTabBtn::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CLobbyTabBtn::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(m_eBlendState)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;


    const _float fRatio = 0.7f;
    const _float fTextStartX = static_cast<_float>(m_rcBtnRange.left) + m_fHalfSizeX * 0.15f;
    const _float fTextStartY = static_cast<_float>(m_rcBtnRange.top) + m_fHalfSizeY * 0.28f;

    m_pGameInstance->Draw_Text(TEXT("Font_Pretendard_SemiBold"),
        TEXT("게임 시작"),
        _float2(fTextStartX, fTextStartY), XMVectorSet(m_fFontColor, m_fFontColor, m_fFontColor, 1.f),
        _float2(fDefaultFontSize * m_fFontRatio, fDefaultFontSize * m_fFontRatio)
    );

    return S_OK;
}

HRESULT CLobbyTabBtn::Ready_Components()
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

HRESULT CLobbyTabBtn::Bind_ShaderResources()
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

    m_pShaderCom->Bind_RawValue("g_UVFillX", &m_fFillX, sizeof(m_fFillX));
    m_pShaderCom->Bind_RawValue("g_UVFillCenterY", &m_fFillCenterY, sizeof(m_fFillCenterY));

    return S_OK;
}

void CLobbyTabBtn::BtnClick()
{
    m_funcLobbyCallBack(m_eNextStage);
}

void CLobbyTabBtn::Execute_Btn(_float fTimeDelta)
{
    constexpr _float fSpeed = 5.f;
    constexpr _float fFontSpeed = 10.f;
    constexpr _float fMaxFillX = 1.f;
    constexpr _float fMaxFillCenterY = 0.5f;

    switch (m_eCurBtnState) {
    case BTN_STATE::NORMAL:
    {
        m_fFillX -= fTimeDelta * fSpeed;
        m_fFillCenterY -= fTimeDelta * fSpeed;
        m_fFontRatio = 0.7f;

        if (m_fFillX < 0.f) {
            m_fFillX = 0.f;
        }

        if (m_fFillCenterY < 0.f) {
            m_fFillCenterY = 0.f;
        }

        m_fFontColor = 1.f;
        break;
    }

    case BTN_STATE::HOVER:
    {
        const _float fMacFontRatio = 0.9f;

        m_fFillX += fTimeDelta * fSpeed;
        m_fFillCenterY += fTimeDelta * fSpeed;
        m_fFontColor -= fTimeDelta * fFontSpeed;
        m_fFontRatio += fTimeDelta * fSpeed;

        if (m_fFillX > fMaxFillX) {
            m_fFillX = fMaxFillX;
        }

        if (m_fFillCenterY > fMaxFillCenterY) {
            m_fFillCenterY = fMaxFillCenterY;
        }

        if (m_fFontColor < 0.f) {
            m_fFontColor = 0.f;
        }

        if (m_fFontRatio > fMacFontRatio) {
            m_fFontRatio = fMacFontRatio;
        }

        break;
    }

    case BTN_STATE::PRESSED:
    {
        m_fFillX = 0.f;
        m_fFillCenterY = 0.f;
        m_fFontColor = 1.f;
        m_fFontRatio = 0.7f;
        break;
    }

    case BTN_STATE::CLICKED:
    {
        m_fFillX = 0.f;
        m_fFillCenterY = 0.f;
        m_fFontColor = 1.f;
        m_fFontRatio = 0.7f;
        m_bIsClicked = true;
        break;
    }
    }
}

CLobbyTabBtn* CLobbyTabBtn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLobbyTabBtn* pInstance = new CLobbyTabBtn(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CLobbyTabBtn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLobbyTabBtn::Clone(void* pArg)
{
    CLobbyTabBtn* pInstance = new CLobbyTabBtn(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CLobbyTabBtn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLobbyTabBtn::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
