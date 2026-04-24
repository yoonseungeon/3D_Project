#include "CUI_StatBox.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"

#include "CAbstractPlayer.h"

CUI_StatBox::CUI_StatBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CUI_StatBox::CUI_StatBox(const CUI_StatBox& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CUI_StatBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_StatBox::Initialize(void* pArg)
{
    CUI_STATBOX_DESC* pDesc = static_cast<CUI_STATBOX_DESC*>(pArg);

    static const _float fStartAdjust{ 0.5f };

    m_fImagePosX = (pDesc->fPosRatioX + fStartAdjust) *  static_cast<_float>(g_iWinSizeX) + static_cast<_float>(g_iWinSizeX) * 0.01f;
    m_fImagePosY = -(pDesc->fPosRatioY - fStartAdjust) *  static_cast<_float>(g_iWinSizeY) - static_cast<_float>(g_iWinSizeY) * 0.012f;

    m_eStatBoxType = pDesc->eStatBoxType;

    pDesc->wstrTexturePrototypeTag = TEXT("Prototype_Texture_IcoStat");
    pDesc->eBlendState = CUI_Default::COLOR_ALPHABLEND;
    pDesc->eTexPrototypeLV = LEVEL::GAMEPLAY;
    pDesc->iUILayer = ETOUI(UILAYER::SLOT);

    m_iTexIdx = m_eStatBoxType;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CUI_StatBox::Priority_Update(_float fTimeDelta)
{
}

void CUI_StatBox::Parallel_Update(_float fTimeDelta)
{
}

void CUI_StatBox::Update(_float fTimeDelta)
{
    CAbstractPlayer* pPlayer =CInGame_Manager::GetInstance()->Get_Player();
    const CUnit::UNIT_STAT& tStat = pPlayer->Get_FinalStat();

    switch (m_eStatBoxType)
    {
    case ATKPower:
        if (tStat.iATKPower != m_iData1)
        {
            m_iData1 = tStat.iATKPower;
            m_wstrText = to_wstring(m_iData1);
        }
        break;

    case SkillAmp:
        if (tStat.iSkillAmp != m_iData1)
        {
            m_iData1 = tStat.iSkillAmp;
            m_wstrText = to_wstring(m_iData1);
        }
        break;

    case PenetrationDefense:
        if (tStat.iPenetrationDefense != m_iData1 || tStat.iPenetrationDefensePercent != m_iData2)
        {
            m_iData1 = tStat.iPenetrationDefense;
            m_iData2 = tStat.iPenetrationDefensePercent;

            m_wstrText = to_wstring(m_iData1) + L" | " + to_wstring(m_iData2) + L"%";
        }
        break;

    case Defense:
        if (tStat.iDefense != m_iData1)
        {
            m_iData1 = tStat.iDefense;
            m_wstrText = to_wstring(m_iData1);
        }
        break;

    case ATKSpeed:
        if (tStat.fATKSpeed != m_fData1)
        {
            m_fData1 = tStat.fATKSpeed;
            _tchar sztmp[10];
            swprintf_s(sztmp, 10, L"%.2f", m_fData1);
            m_wstrText = sztmp;
        }
        break;

    case CoolDown:
        if (tStat.iCoolDown != m_iData1)
        {
            m_iData1 = tStat.iCoolDown;
            m_wstrText = to_wstring(m_iData1);
        }
        break;

    case Critical:
        if (tStat.iCoolDown != m_iData1)
        {
            m_iData1 = tStat.iCoolDown;
            m_wstrText = to_wstring(m_iData1) + L"%";
        }

        break;

    case Speed:
        if (tStat.fSpeed != m_fData1)
        {
            m_fData1 = tStat.fSpeed;
            _tchar sztmp[10];
            swprintf_s(sztmp, 10, L"%.2f", m_fData1);
            m_wstrText = sztmp;
        }
        break;
    }
}

void CUI_StatBox::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_StatBox::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(ETOUI(m_eBlendState))))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    static const _float fAdjustFontSize = 0.46f;

    m_pGameInstance->Draw_Text(TEXT("Font_Pretendard_Middle"),
        m_wstrText.data(),
        _float2(m_fImagePosX, m_fImagePosY), XMVectorSet(1.f, 1.f, 1.f, 1.f),
        _float2(fDefaultFontSize * fAdjustFontSize, fDefaultFontSize * fAdjustFontSize)
    );

    return S_OK;
}

HRESULT CUI_StatBox::Ready_Components()
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

HRESULT CUI_StatBox::Bind_ShaderResources()
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

CUI_StatBox* CUI_StatBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_StatBox* pInstance = new CUI_StatBox(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_StatBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_StatBox::Clone(void* pArg)
{
    CUI_StatBox* pInstance = new CUI_StatBox(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_StatBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_StatBox::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
