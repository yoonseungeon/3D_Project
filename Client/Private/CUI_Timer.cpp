#include "CUI_Timer.h"

#include "CGameInstance.h"

#include "CInGame_Manager.h"

#include "CUI_Day.h"
#include "CUI_DayIcon.h"

CUI_Timer::CUI_Timer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CUI_Timer::CUI_Timer(const CUI_Timer& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CUI_Timer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Timer::Initialize(void* pArg)
{
    CUI_TIMTER_DESC* pDesc = static_cast<CUI_TIMTER_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fLeftEndU = 0.48f;
    m_fRightStartU = 1.f - m_fLeftEndU;

    m_fRenderLeftEnd = 0.166f;
    m_fRenderRightStart = 1.f - m_fRenderLeftEnd;

    m_vColor = { 0.2222f, 0.3439f, 0.4074f };
    m_fImageAlpha = 0.8f;

    CInGame_Manager::GetInstance()->Set_DayTimer(this);

    m_fCenterX = (pDesc->fPosRatioX + 0.5f) * static_cast<_float>(g_iWinSizeX);
    m_fCenterY = -(pDesc->fPosRatioY - 0.5f) * static_cast<_float>(g_iWinSizeY);

    m_wstrTime[2] = L":";
    m_wstrTime[0] = m_wstrTime[1] = m_wstrTime[3] = m_wstrTime[4] = L"0";

    if (FAILED(Ready_Layer_Timer(TEXT("Layer_UI_Timer"))))
        return E_FAIL;

    return S_OK;
}

void CUI_Timer::Priority_Update(_float fTimeDelta)
{
}

void CUI_Timer::Parallel_Update(_float fTimeDelta)
{
}

void CUI_Timer::Update(_float fTimeDelta)
{
}

void CUI_Timer::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_Timer::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(BLENDSTATE::REMAPSLICE_U)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;


    if (FAILED(Render_Text()))
        return E_FAIL;

    return S_OK;
}

void CUI_Timer::Set_Timer(_uint iDay, _float fTime)
{
    _uint iTime = static_cast<_uint>(fTime);

    _uint iMinute = iTime / 60;
    _uint iSecond = iTime % 60;

    iMinute = (std::min)(iMinute, 99u);

    m_wstrTime[0] = to_wstring(iMinute / 10);
    m_wstrTime[1] = to_wstring(iMinute % 10);
    m_wstrTime[3] = to_wstring(iSecond / 10);
    m_wstrTime[4] = to_wstring(iSecond % 10);

    m_pDay->Set_Day(iDay);
    m_pDayIcon->Set_Day(iDay);
}

HRESULT CUI_Timer::Ready_Components()
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
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Texture_Img_HUD_TimerFrame_Big"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Timer::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIdx)))
        return E_FAIL;

    m_pShaderCom->Bind_RawValue("g_Alpha", &m_fImageAlpha, sizeof(m_fImageAlpha));
    m_pShaderCom->Bind_RawValue("g_Color", &m_vColor, sizeof(m_vColor));

    m_pShaderCom->Bind_RawValue("g_LeftEndU", &m_fLeftEndU, sizeof(m_fLeftEndU));
    m_pShaderCom->Bind_RawValue("g_RightStartU", &m_fRightStartU, sizeof(m_fRightStartU));
    m_pShaderCom->Bind_RawValue("g_RenderLeftEnd", &m_fRenderLeftEnd, sizeof(m_fRenderLeftEnd));
    m_pShaderCom->Bind_RawValue("g_RenderRightStart", &m_fRenderRightStart, sizeof(m_fRenderRightStart));

    return S_OK;
}

HRESULT CUI_Timer::Ready_Layer_Timer(const _wstring& strLayerTag)
{
    CUI_Day::CUI_DAY_DESC DescDay{};

    DescDay.fScaleRatioX = 0.060938f;
    DescDay.fScaleRatioY = 0.05f;
    DescDay.fPosRatioX = -0.088281f;
    DescDay.fPosRatioY = 0.5f - DescDay.fScaleRatioY * 0.5f;
    DescDay.iUILayer = ETOUI(UILAYER::BACKGROUND);

    DescDay.iFlipX = true;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_Day"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &DescDay, reinterpret_cast<CGameObject**>(&m_pDay))))
        return E_FAIL;


    CUI_DayIcon::CUI_DAYICON_DESC DescIcon{};

    DescIcon.fScaleRatioX = 0.060938f;
    DescIcon.fScaleRatioY = 0.05f;
    DescIcon.fPosRatioX = 0.088281f;
    DescIcon.fPosRatioY = 0.5f - DescIcon.fScaleRatioY * 0.5f;
    DescIcon.iUILayer = ETOUI(UILAYER::BACKGROUND);

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_DayIcon"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &DescIcon, reinterpret_cast<CGameObject**>(&m_pDayIcon))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Timer::Render_Text()
{
    const float fAdjustFontSize = fDefaultFontSize * 0.8f;

    const wstring strFontTag = L"Font_Pretendard_Middle";

    const _float2 vDigitLength = m_pGameInstance->Cal_FontLength(strFontTag, L"0");
    const _float2 vColonLength = m_pGameInstance->Cal_FontLength(strFontTag, L":");

    const _float fDigitWidth = vDigitLength.x * fAdjustFontSize;
    const _float fDigitHeight = vDigitLength.y * fAdjustFontSize;

    const _float fColonWidth = vColonLength.x * fAdjustFontSize;
    const _float fColonHeight = vColonLength.y * fAdjustFontSize;

    // 숫자 사이 간격
    const _float fDigitGap = fDigitWidth * 0.0f;

    // 콜론과 숫자 사이 간격
    const _float fColonGap = fDigitWidth * 0.4f;

    // : 는 중앙 고정
    const _float fColonX = m_fCenterX - fColonWidth * 0.5f;
    const _float fColonY = m_fCenterY - fColonHeight * 0.5f;

    _float2 vPositions[5]{};

    // 왼쪽 2자리
    vPositions[1] = _float2(fColonX - fColonGap - fDigitWidth, m_fCenterY - fDigitHeight * 0.5f);
    vPositions[0] = _float2(vPositions[1].x - fDigitGap - fDigitWidth, m_fCenterY - fDigitHeight * 0.5f);

    // 콜론
    vPositions[2] = _float2(fColonX, fColonY);

    // 오른쪽 2자리
    vPositions[3] = _float2(fColonX + fColonWidth + fColonGap, m_fCenterY - fDigitHeight * 0.5f);
    vPositions[4] = _float2(vPositions[3].x + fDigitWidth + fDigitGap, m_fCenterY - fDigitHeight * 0.5f);

    for (_uint i = 0; i < 5; ++i)
    {
        m_pGameInstance->Draw_Text(TEXT("Font_Pretendard_Middle"), m_wstrTime[i].c_str(), vPositions[i],
            XMVectorSet(1.f, 1.f, 1.f, 1.f), _float2(fAdjustFontSize, fAdjustFontSize));
    }

    return S_OK;
}

CUI_Timer* CUI_Timer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Timer* pInstance = new CUI_Timer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_Timer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Timer::Clone(void* pArg)
{
    CUI_Timer* pInstance = new CUI_Timer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_Timer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Timer::Free()
{
    Safe_Release(m_pDay);
    Safe_Release(m_pDayIcon);

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}