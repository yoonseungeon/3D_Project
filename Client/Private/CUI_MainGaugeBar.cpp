#include "CUI_MainGaugeBar.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"

#include "CAbstractPlayer.h"

CUI_MainGaugeBar::CUI_MainGaugeBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{
}

CUI_MainGaugeBar::CUI_MainGaugeBar(const CUI_MainGaugeBar& Prototype)
    : CUI_Default{ Prototype }
{
}

HRESULT CUI_MainGaugeBar::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_MainGaugeBar::Initialize(void* pArg)
{
    CUI_MAINGAUGEBAR_DESC* pDesc = static_cast<CUI_MAINGAUGEBAR_DESC*>(pArg);

    m_fScaleRatioY = pDesc->fScaleRatioY;

    m_eGaugeType = pDesc->eGaugeType;

    static const _float fStartAdjust{ 0.5f };

    const _float fAdjustY = pDesc->fScaleRatioY * static_cast<_float>(g_iWinSizeY) * 0.52f;


    m_fImagePosX = (pDesc->fPosRatioX + fStartAdjust) * static_cast<_float>(g_iWinSizeX);
    m_fImagePosY = -(pDesc->fPosRatioY - fStartAdjust) * static_cast<_float>(g_iWinSizeY) - fAdjustY;


    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    switch (m_eGaugeType)
    {
        case MAINGAUGE_TYPE::HP:
            m_iTexIdx = 0;
            m_wstrTexturePrototypeTag = L"Prototype_Texture_Img_MainGauge";
            break;

        case MAINGAUGE_TYPE::MP:
            m_iTexIdx = 1;
            m_wstrTexturePrototypeTag = L"Prototype_Texture_Img_MainGauge";
            break;

        case MAINGAUGE_TYPE::INTOXICATION:
            m_iTexIdx = 0;
            m_wstrTexturePrototypeTag = L"Prototype_Texture_WhiteBlock";
            m_vColor = COLOR_TO_FLOAT(241, 227, 106);
            break;

        default:
            return E_FAIL;
    }

    if (FAILED(Ready_Components()))
        return E_FAIL;

    // 폰트 크기
    m_fAdjustFontSize = m_fScaleRatioY * 22.f;

    // 글자 기본 위치
    m_fDefaultStartAdjustLeft = pDesc->fScaleRatioX * 0.06f;
    m_fDefaultStartAdjustRight = pDesc->fScaleRatioX * 0.05f;


    m_fTextGapX = pDesc->fScaleRatioX * 0.068f * m_fAdjustFontSize;

    m_fRightNumStart = m_fDefaultStartAdjustRight * static_cast<_float>(g_iWinSizeX);

    return S_OK;
}

void CUI_MainGaugeBar::Priority_Update(_float fTimeDelta)
{
}

void CUI_MainGaugeBar::Parallel_Update(_float fTimeDelta)
{
}

void CUI_MainGaugeBar::Update(_float fTimeDelta)
{
    CAbstractPlayer* pPlayer = CInGame_Manager::GetInstance()->Get_Player();
    const CUnit::UNIT_STAT& tFinalStat = pPlayer->Get_FinalStat();
    const CUnit::UNIT_STAT& tCurStat = pPlayer->Get_CurStat();

    if (m_eGaugeType == MAINGAUGE_TYPE::HP)
    {
        if(m_iCurData != tCurStat.iHP || m_iMaxData != tFinalStat.iHP)
        {
            m_iCurData = tCurStat.iHP;
            m_iMaxData = tFinalStat.iHP;

            Reset_Gauge();
        }
    }
    else if(m_eGaugeType == MAINGAUGE_TYPE::INTOXICATION)
    {
        if (m_iCurData != tCurStat.iMP || m_iMaxData != tFinalStat.iMP)
        {
            m_iCurData = tCurStat.iMP;
            m_iMaxData = tFinalStat.iMP;

            if (m_iCurData >= 40)
            {
                m_vColor = COLOR_TO_FLOAT(243, 158, 28);
            }
            else
            {
                m_vColor = COLOR_TO_FLOAT(241, 227, 106);
            }

            Reset_Gauge();
        }
    }
    else
    {
        if (m_iCurData != tCurStat.iMP || m_iMaxData != tFinalStat.iMP)
        {
            m_iCurData = tCurStat.iMP;
            m_iMaxData = tFinalStat.iMP;

            Reset_Gauge();
        }
    }

}

void CUI_MainGaugeBar::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_MainGaugeBar::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(m_eBlendState)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    m_pGameInstance->Draw_Text(TEXT("Font_Pretendard_Middle"),
        m_wstrCur.data(),
        _float2(m_fImagePosX - m_fLeftNumStart, m_fImagePosY), XMVectorSet(1.f, 1.f, 1.f, 1.f),
        _float2(fDefaultFontSize * m_fAdjustFontSize, fDefaultFontSize * m_fAdjustFontSize)
    );

    m_pGameInstance->Draw_Text(TEXT("Font_Pretendard_Middle"),
        m_wstrSlash.data(),
        _float2(m_fImagePosX, m_fImagePosY), XMVectorSet(1.f, 1.f, 1.f, 1.f),
        _float2(fDefaultFontSize * m_fAdjustFontSize, fDefaultFontSize * m_fAdjustFontSize)
    );

    m_pGameInstance->Draw_Text(TEXT("Font_Pretendard_Middle"),
        m_wstrMax.data(),
        _float2(m_fImagePosX + m_fRightNumStart, m_fImagePosY), XMVectorSet(1.f, 1.f, 1.f, 1.f),
        _float2(fDefaultFontSize * m_fAdjustFontSize, fDefaultFontSize * m_fAdjustFontSize)
    );

    return S_OK;
}

HRESULT CUI_MainGaugeBar::Ready_Components()
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

HRESULT CUI_MainGaugeBar::Bind_ShaderResources()
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

    m_pShaderCom->Bind_RawValue("g_UVFillX", &m_fFillX, sizeof(m_fFillX));
    m_pShaderCom->Bind_RawValue("g_UVFillY", &m_fFillY, sizeof(m_fFillY));

    return S_OK;
}

void CUI_MainGaugeBar::Reset_Gauge()
{
    m_fFillX = static_cast<_float>(m_iCurData) / static_cast<_float>(m_iMaxData);

    m_wstrCur = to_wstring(m_iCurData);
    m_wstrMax = to_wstring(m_iMaxData);

    m_fLeftNumStart = m_fDefaultStartAdjustLeft + m_fTextGapX * static_cast<_float>(m_wstrCur.length() - 1);
    m_fLeftNumStart *= static_cast<_float>(g_iWinSizeX);
}

CUI_MainGaugeBar* CUI_MainGaugeBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_MainGaugeBar* pInstance = new CUI_MainGaugeBar(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_MainGaugeBar");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_MainGaugeBar::Clone(void* pArg)
{
    CUI_MainGaugeBar* pInstance = new CUI_MainGaugeBar(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_MainGaugeBar");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_MainGaugeBar::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
