#include "CInGameCharHUD_Empty.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"

#include "CUI_Image.h"
#include "CUI_CharSkillPanel.h"
#include "CUI_StatPanel.h"

CInGameCharHUD_Empty::CInGameCharHUD_Empty(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CInGameCharHUD_Empty::CInGameCharHUD_Empty(const CInGameCharHUD_Empty& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CInGameCharHUD_Empty::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInGameCharHUD_Empty::Initialize(void* pArg)
{
    CUI_DEFAULT_DESC Desc{};
    Desc.fScaleRatioX = 1.f;
    Desc.fScaleRatioY = 1.f;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (CInGame_Manager::GetInstance()->Get_Player() == nullptr)
    {
        MSG_BOX("CInGameCharHUD_Empty: Create Plyaer First");
        return E_FAIL;
    }

    if (FAILED(Ready_Layer_UI_Image(TEXT("Layer_UI_Image"))))
        return E_FAIL;

    return S_OK;
}

void CInGameCharHUD_Empty::Priority_Update(_float fTimeDelta)
{
}

void CInGameCharHUD_Empty::Parallel_Update(_float fTimeDelta)
{
}

void CInGameCharHUD_Empty::Update(_float fTimeDelta)
{
}

void CInGameCharHUD_Empty::Late_Update(_float fTimeDelta)
{
}

HRESULT CInGameCharHUD_Empty::Render()
{
  
    return S_OK;
}

HRESULT CInGameCharHUD_Empty::Ready_Layer_UI_Image(const _wstring& strLayerTag)
{
    // CInGameCharLevel_Empty
    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CInGameCharLevel_Empty"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag)))
        return E_FAIL;

    // CUI_CharSkillPanel
    CUI_CharSkillPanel::CUI_CAHRSKILLPANEL_DESC SkillPanelDesc{};

    SkillPanelDesc.fScaleRatioX = 0.2764f;
    SkillPanelDesc.fScaleRatioY = 0.125f;
    SkillPanelDesc.fPosRatioX = -0.025f;
    SkillPanelDesc.fPosRatioY = -0.436945f;

    SkillPanelDesc.iUILayer = ETOUI(UILAYER::PANEL);

    SkillPanelDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    SkillPanelDesc.wstrTexturePrototypeTag = L"Prototype_Texture_WhiteBlock";

    SkillPanelDesc.eBlendState = CUI_Default::COLOR_ALPHABLEND;
    SkillPanelDesc.vColor = COLOR_TO_FLOAT(33, 45, 51);

    SkillPanelDesc.fImageAlpha = 0.8f;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_CharSkillPanel"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &SkillPanelDesc)))
        return E_FAIL;


    // CUI_StatPanel
    CUI_StatPanel::CUI_STATPANEL_DESC StatPanelDesc{};

    StatPanelDesc.fScaleRatioX = 0.105f;
    StatPanelDesc.fScaleRatioY = SkillPanelDesc.fScaleRatioY;
    StatPanelDesc.fPosRatioX = -0.34f;
    StatPanelDesc.fPosRatioY = SkillPanelDesc.fPosRatioY;

    StatPanelDesc.iUILayer = ETOUI(UILAYER::PANEL);

    StatPanelDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    StatPanelDesc.wstrTexturePrototypeTag = L"Prototype_Texture_WhiteBlock";

    StatPanelDesc.eBlendState = CUI_Default::COLOR_ALPHABLEND;
    StatPanelDesc.vColor = COLOR_TO_FLOAT(33, 45, 51);

    StatPanelDesc.fImageAlpha = 0.8f;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_StatPanel"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &StatPanelDesc)))
        return E_FAIL;

    return S_OK;
}

CInGameCharHUD_Empty* CInGameCharHUD_Empty::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInGameCharHUD_Empty* pInstance = new CInGameCharHUD_Empty(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CInGameCharHUD_Empty");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInGameCharHUD_Empty::Clone(void* pArg)
{
    CInGameCharHUD_Empty* pInstance = new CInGameCharHUD_Empty(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CInGameCharHUD_Empty");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInGameCharHUD_Empty::Free()
{
    __super::Free();
}
