#include "CUI_DayIcon.h"

#include "CGameInstance.h"

#include "CInGame_Manager.h"

#include "CUI_Image.h"

CUI_DayIcon::CUI_DayIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{
}

CUI_DayIcon::CUI_DayIcon(const CUI_DayIcon& Prototype)
    : CUI_Default{ Prototype }
{
}

HRESULT CUI_DayIcon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_DayIcon::Initialize(void* pArg)
{
    CUI_DAYICON_DESC* pDesc = static_cast<CUI_DAYICON_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_vColor = { 0.1294118f, 0.1607843f, 0.2f };
    m_fImageAlpha = 0.8f;

    m_fPosRatioX = pDesc->fPosRatioX;
    m_fPosRatioY = pDesc->fPosRatioY;

    if (FAILED(Ready_Layer_Timer(TEXT("Layer_UI_Timer"))))
        return E_FAIL;

    return S_OK;
}

void CUI_DayIcon::Priority_Update(_float fTimeDelta)
{
}

void CUI_DayIcon::Parallel_Update(_float fTimeDelta)
{
}

void CUI_DayIcon::Update(_float fTimeDelta)
{
}

void CUI_DayIcon::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_DayIcon::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(BLENDSTATE::COLOR_ALPHABLEND)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CUI_DayIcon::Set_Day(_uint iDay)
{
    m_pDayIcon->Set_TexIdx((iDay + 1) % 2);
}

HRESULT CUI_DayIcon::Ready_Components()
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

HRESULT CUI_DayIcon::Bind_ShaderResources()
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

HRESULT CUI_DayIcon::Ready_Layer_Timer(const _wstring& strLayerTag)
{
    CUI_Image::CUI_IMAGE_DESC Desc{};

    Desc.fScaleRatioX = 0.025f;
    Desc.fScaleRatioY = Desc.fScaleRatioX / static_cast<_float>(g_iWinSizeY) * static_cast<_float>(g_iWinSizeX);
    Desc.fPosRatioX = m_fPosRatioX;
    Desc.fPosRatioY = m_fPosRatioY;
    Desc.iUILayer = ETOUI(UILAYER::DECO_LAYER1);

    Desc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Ico_Day";

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&m_pDayIcon))))
        return E_FAIL;

    return S_OK;
}

CUI_DayIcon* CUI_DayIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_DayIcon* pInstance = new CUI_DayIcon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_Day");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_DayIcon::Clone(void* pArg)
{
    CUI_DayIcon* pInstance = new CUI_DayIcon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_Day");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_DayIcon::Free()
{
    Safe_Release(m_pDayIcon);

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}