#include "CUI_LevelPanel.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"

#include "CAbstractPlayer.h"

CUI_LevelPanel::CUI_LevelPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CUI_LevelPanel::CUI_LevelPanel(const CUI_LevelPanel& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CUI_LevelPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_LevelPanel::Initialize(void* pArg)
{
    CUI_LEVELPANEL_DESC* pDesc = static_cast<CUI_LEVELPANEL_DESC*>(pArg);

    const _float fStartAdjust{ 0.5f };

    const _float fAdjustXDigit1 = pDesc->fScaleRatioX * static_cast<_float>(g_iWinSizeX) * 0.24f;
    const _float fAdjustXDigit2 = pDesc->fScaleRatioX * static_cast<_float>(g_iWinSizeX) * 0.38f;
    const _float fAdjustY = pDesc->fScaleRatioY * static_cast<_float>(g_iWinSizeY) * 0.3f;

    m_fImagePosX = (pDesc->fPosRatioX + fStartAdjust) * static_cast<_float>(g_iWinSizeX) - fAdjustXDigit1;
    m_fImagePosY = -(pDesc->fPosRatioY - fStartAdjust) * static_cast<_float>(g_iWinSizeY) - fAdjustY;

    m_fDigit2 = (pDesc->fPosRatioX + fStartAdjust) * static_cast<_float>(g_iWinSizeX) - fAdjustXDigit2;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CUI_LevelPanel::Priority_Update(_float fTimeDelta)
{
}

void CUI_LevelPanel::Parallel_Update(_float fTimeDelta)
{
}

void CUI_LevelPanel::Update(_float fTimeDelta)
{
    CAbstractPlayer* pPlayer = CInGame_Manager::GetInstance()->Get_Player();
    const CUnit::UNIT_STAT& tStat = pPlayer->Get_CurStat();

    if (tStat.iLevel != m_iData)
    {
        m_iData = tStat.iLevel;
        m_wstrText = to_wstring(m_iData);

        if (m_iData >= 10)
        {
            m_fImagePosX = m_fDigit2;
        }
    }
}

void CUI_LevelPanel::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_LevelPanel::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(ETOUI(m_eBlendState))))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    static const _float fAdjustFontSize = 0.47f;

    m_pGameInstance->Draw_Text(TEXT("Font_Pretendard_Middle"),
        m_wstrText.data(),
        _float2(m_fImagePosX, m_fImagePosY), XMVectorSet(1.f, 1.f, 1.f, 1.f),
        _float2(fDefaultFontSize * fAdjustFontSize, fDefaultFontSize * fAdjustFontSize)
    );

    return S_OK;
}

HRESULT CUI_LevelPanel::Ready_Components()
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

HRESULT CUI_LevelPanel::Bind_ShaderResources()
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

CUI_LevelPanel* CUI_LevelPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_LevelPanel* pInstance = new CUI_LevelPanel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_LevelPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_LevelPanel::Clone(void* pArg)
{
    CUI_LevelPanel* pInstance = new CUI_LevelPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_LevelPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_LevelPanel::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
