#include "CInGameHPBar.h"

#include "CGameInstance.h"

#include "CUI_Default.h"

CInGameHPBar::CInGameHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInGameUI{ pDevice, pContext }
{
}

CInGameHPBar::CInGameHPBar(const CInGameHPBar& Prototype)
    : CInGameUI{ Prototype }
{
}

HRESULT CInGameHPBar::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInGameHPBar::Initialize(void* pArg)
{
    INGAMEHPBAR_DESC* pDesc = static_cast<INGAMEHPBAR_DESC*>(pArg);

    m_pMaxHp = pDesc->pMaxHp;
    m_pCurHp = pDesc->pCurHp;

    m_pMaxMp = pDesc->pMaxMp;
    m_pCurMp = pDesc->pCurMp;

    m_bNoMp = pDesc->bNoMp;

    m_vHpColor = pDesc->vHPColor;
    m_vMpColor = pDesc->vMPColor;

    m_eUnitType = pDesc->eUnitType;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    _float fAdjuset = 0.7f;
    _float fSizeY{};

    if (m_eUnitType == CHAR)
    {
        fSizeY = 32.f * fAdjuset / static_cast<_float>(g_iWinSizeY) * 2.f;
        m_bDrawSmallLine = true;
    }
    else if (m_eUnitType == MONSTER)
    {
        fSizeY = 24.f * fAdjuset / static_cast<_float>(g_iWinSizeY) * 2.f;
        m_bDrawSmallLine = false;
    }

    m_pTransformCom->Set_Scale(
        148.f * fAdjuset / static_cast<_float>(g_iWinSizeX) *  2.f,
        fSizeY,
        1.f
    );

    m_iUILayer = ETOUI(UILAYER::INGMAE);

    return S_OK;
}

void CInGameHPBar::Priority_Update(_float fTimeDelta)
{
}

void CInGameHPBar::Parallel_Update(_float fTimeDelta)
{
}

void CInGameHPBar::Update(_float fTimeDelta)
{
    Update_Pos();
}

void CInGameHPBar::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CInGameHPBar::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(ETOUI(CUI_Default::BLENDSTATE::INGAME_HPBAR))))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CInGameHPBar::Ready_Components()
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
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_HealthBar"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CInGameHPBar::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    _float fMaxHp = static_cast<_float>(*m_pMaxHp);
    _float fCurHp = static_cast<_float>(*m_pCurHp);

    m_pShaderCom->Bind_RawValue("g_fMaxHp", &fMaxHp, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fCurHp", &fCurHp, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_HpColor", &m_vHpColor, sizeof(_float3));

    m_pShaderCom->Bind_RawValue("g_NoMp", &m_bNoMp, sizeof(_bool));

    m_pShaderCom->Bind_RawValue("g_DrawSmallLine", &m_bDrawSmallLine, sizeof(_bool));

    if(m_bNoMp == true)
        return S_OK;

    _float fMpRatio = static_cast<_float>(*m_pCurMp) / static_cast<_float>(*m_pMaxMp);
    m_pShaderCom->Bind_RawValue("g_fMpRatio", &fMpRatio, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_MpColor", &m_vMpColor, sizeof(_float3));

    return S_OK;
}

CInGameHPBar* CInGameHPBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInGameHPBar* pInstance = new CInGameHPBar(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CInGameHPBar");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInGameHPBar::Clone(void* pArg)
{
    CInGameHPBar* pInstance = new CInGameHPBar(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CInGameHPBar");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInGameHPBar::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
