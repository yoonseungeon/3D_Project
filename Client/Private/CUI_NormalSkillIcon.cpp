#include "CUI_NormalSkillIcon.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"
#include "CAbstractPlayer.h"

#include "CSkillLevelUpBtn.h"
#include "CSkillCoolDisplay.h"

CUI_NormalSkillIcon::CUI_NormalSkillIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_SkillIcon{ pDevice, pContext }
{
}

CUI_NormalSkillIcon::CUI_NormalSkillIcon(const CUI_NormalSkillIcon& Prototype)
    : CUI_SkillIcon{ Prototype }
{
}

HRESULT CUI_NormalSkillIcon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_NormalSkillIcon::Initialize(void* pArg)
{
    CUI_NORMALSKILLICON_DESC* pDesc = static_cast<CUI_NORMALSKILLICON_DESC*>(pArg);

    m_fScaleRatioX = pDesc->fScaleRatioX;
    m_fScaleRatioY = pDesc->fScaleRatioY;
    m_fPosRatioX = pDesc->fPosRatioX;
    m_fPosRatioY = pDesc->fPosRatioY;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Layer_SkillLevelUpBtn(TEXT("Layer_UI_SkillLevelUpBtn"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_SkillCoolDisplay(TEXT("Layer_UI_SkillCoolDisplay"))))
        return E_FAIL;

    return S_OK;
}

void CUI_NormalSkillIcon::Priority_Update(_float fTimeDelta)
{
}

void CUI_NormalSkillIcon::Parallel_Update(_float fTimeDelta)
{
}

void CUI_NormalSkillIcon::Update(_float fTimeDelta)
{
    m_iCurSkillLevel = m_pInGameManager->Get_Player()->Get_CurSkillLevel(m_eSkillSlot);
}

void CUI_NormalSkillIcon::Late_Update(_float fTimeDelta)
{
    COOL_INFO* pCoolInfo = m_pInGameManager->Get_Player()->Get_CoolInfo(m_eSkillSlot);

    m_pSkillcoolDisplay->Set_CoolTime(pCoolInfo->fCurCoolDown, pCoolInfo->fAccCoolDown);

    _float fTime{};

    if (pCoolInfo->fAccCoolDown > 0.f && pCoolInfo->bCoolWait == false) {
        fTime = pCoolInfo->fAccCoolDown;
        m_bTextRender = true;
    }
    else
    {
        m_bTextRender = false;
    }

    if (m_bTextRender == true) {
        if (fTime >= 1.f) {
            m_wstCoolText = to_wstring(static_cast<_int>(fTime));
        }
        else {
            _tchar sztmp[10];
            swprintf_s(sztmp, 10, L"%.1f", fTime);
            m_wstCoolText = sztmp;
        }
    }


    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_NormalSkillIcon::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(BLENDSTATE::SKILL_ICON)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    if (FAILED(RenderText()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_NormalSkillIcon::Ready_Components()
{
    return S_OK;
}

HRESULT CUI_NormalSkillIcon::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(CUIObject::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(CUIObject::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIdx)))
        return E_FAIL;

    m_pShaderCom->Bind_RawValue("g_CurLevel", &m_iCurSkillLevel, sizeof(m_iCurSkillLevel));
    m_pShaderCom->Bind_RawValue("g_MaxLevel", &m_iMaxSkillLevel, sizeof(m_iMaxSkillLevel));

    return S_OK;
}

HRESULT CUI_NormalSkillIcon::Ready_Layer_SkillLevelUpBtn(const _wstring& strLayerTag)
{
    CSkillLevelUpBtn::SKILL_LEVELUP_BTN_DESC Desc{};

    Desc.fScaleRatioX = m_fScaleRatioX * 1.2f;
    Desc.fScaleRatioY = m_fRatioYNoExtend * 1.2f;
    Desc.fPosRatioX = m_fPosRatioX;
    Desc.fPosRatioY = m_fPosRatioY + 0.07f;

    Desc.iUILayer = ETOUI(UILAYER::BUTTON);
    Desc.eSkillSlot = m_eSkillSlot;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_SkillLevelUpBtn"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_NormalSkillIcon::Ready_Layer_SkillCoolDisplay(const _wstring& strLayerTag)
{
    CSkillCoolDisplay::SKILLCOOLDISPLAY_DESC Desc{};

    Desc.fScaleRatioX = m_fScaleRatioX;
    Desc.fScaleRatioY = m_fRatioYNoExtend;
    Desc.fPosRatioX = m_fPosRatioX;
    Desc.fPosRatioY = m_fPosRatioY + (m_fScaleRatioY - m_fRatioYNoExtend) * 0.5f;  // ?

    Desc.iUILayer = ETOUI(UILAYER::SLOT_DECO);

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_SkillCoolDisplay"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&m_pSkillcoolDisplay))))
        return E_FAIL;

    return S_OK;
}

CUI_NormalSkillIcon* CUI_NormalSkillIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_NormalSkillIcon* pInstance = new CUI_NormalSkillIcon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_NormalSkillIcon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_NormalSkillIcon::Clone(void* pArg)
{
    CUI_NormalSkillIcon* pInstance = new CUI_NormalSkillIcon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_NormalSkillIcon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_NormalSkillIcon::Free()
{
    __super::Free();
}
