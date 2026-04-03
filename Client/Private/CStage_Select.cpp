#include "CStage_Select.h"

#include "CGameInstance.h"

#include "CUI_AniImage.h"
#include "CUI_Btn.h"
#include "CUI_PickPanel.h"
#include "CUI_SkinPanel.h"
#include "CUI_MapPanel.h"

CStage_Select::CStage_Select(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CStage(pDevice, pContext)
{
}

void CStage_Select::Enable_Stage()
{
    __super::Enable_Stage();
}

void CStage_Select::Disable_Stage()
{
    __super::Disable_Stage();
}

void CStage_Select::Change_SelectMap()
{
    for (auto pObj : m_UIs[PICK]) {
        pObj->Set_IsInactive(true);
    }

    for (auto pObj : m_UIs[MAP]) {
        pObj->Set_IsInactive(false);
    }
}

HRESULT CStage_Select::Initialize(function<void(STAGE)> funcBtnCallBack)
{
    m_funcBtnCallBack = funcBtnCallBack;

    if (FAILED(Ready_Layer_UI_Image(TEXT("Layer_UI_Image"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_PickPanel(TEXT("Layer_PickPanel"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_SkinPanel(TEXT("Layer_SkinPanel"))))
        return E_FAIL;

    if (FAILED(Ready_Map()))
        return E_FAIL;

    return S_OK;
}

HRESULT CStage_Select::Ready_Layer_UI_Image(const _wstring& strLayerTag)
{ 
    CGameObject* pObj{ nullptr };

    CUI_Image::CUI_IMAGE_DESC Desc{};

    Desc.fScaleRatioX = 1.0f;
    Desc.fScaleRatioY = 1.0f;
    Desc.fPosRatioX = 0.f;
    Desc.fPosRatioY = 0.f;
    Desc.iUILayer = ETOUI(UILAYER::BACKGROUND);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::DEFAULT;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Select_BG";

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_vecGameObjects.push_back(pObj);

    return S_OK;
}

HRESULT CStage_Select::Ready_Layer_PickPanel(const _wstring& strLayerTag)
{
    CGameObject* pObj{ nullptr };

    CUI_PickPanel::CUI_PICKPANEL_DESC Desc{};

    Desc.fScaleRatioX = 0.35f;
    Desc.fScaleRatioY = 0.6f;
    Desc.fPosRatioX = -0.26f;
    Desc.fPosRatioY = 0.f;
    Desc.iFlipX = false;
    Desc.iFlipY = false;
    Desc.iUILayer = ETOUI(UILAYER::PANEL);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.fImageAlpha = 0.6f;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_PickPanel";

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_PickPanel"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_UIs[VIEW_TYPE::PICK].push_back(pObj);

    return S_OK;
}

HRESULT CStage_Select::Ready_Layer_SkinPanel(const _wstring& strLayerTag)
{
    CGameObject* pObj{ nullptr };

    CUI_SkinPanel::CUI_SKINPANEL_DESC Desc{};

    Desc.fScaleRatioX = 0.33f;
    Desc.fScaleRatioY = 0.23f;
    Desc.fPosRatioX = 0.15f;
    Desc.fPosRatioY = -0.15f;
    Desc.iFlipX = false;
    Desc.iFlipY = false;
    Desc.iUILayer = ETOUI(UILAYER::PANEL);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::DEFAULT;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_PickPanel";

    Desc.funcChangeSelectMap = [this]()->void
        {
        Change_SelectMap();
        };

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_SkinPanel"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_UIs[VIEW_TYPE::SKIN].push_back(pObj);

    return S_OK;
}

HRESULT CStage_Select::Ready_Map()
{
    if (FAILED(Ready_Map_Layer_UI_Image(L"Layer_UI_Image")))
        return E_FAIL;

    if (FAILED(Ready_Map_Layer_MapPanel(L"Layer_MapPanel")))
        return E_FAIL;

    return S_OK;
}

HRESULT CStage_Select::Ready_Map_Layer_UI_Image(const _wstring& strLayerTag)
{
    CGameObject* pObj{ nullptr };

    CUI_Image::CUI_IMAGE_DESC Desc{};

    Desc.fScaleRatioX = 0.35f;
    Desc.fScaleRatioY = 0.45f;
    Desc.fPosRatioX = -0.23f;
    Desc.fPosRatioY = -0.1f;
    Desc.iUILayer = ETOUI(UILAYER::DECO_LAYER1);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_MapDeco";

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_UIs[VIEW_TYPE::MAP].push_back(pObj);


    Desc.fScaleRatioX = 0.45f;
    Desc.fScaleRatioY = 0.55f;
    Desc.fPosRatioY += 0.13f;
    Desc.iUILayer = ETOUI(UILAYER::DECO_LAYER2);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_MapGrid";

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_UIs[VIEW_TYPE::MAP].push_back(pObj);


    Desc.fScaleRatioX = 0.32552f;   // 1000.f / static_cast<_float>(g_iWinSizeX) / 2.4f
    Desc.fScaleRatioY = 0.56886f;   // 983.f / static_cast<_float>(g_iWinSizeY) / 2.4f;
    Desc.fPosRatioY += 0.05f;
    Desc.iUILayer = ETOUI(UILAYER::DECO_LAYER3);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_MapPatternBg";

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_UIs[VIEW_TYPE::MAP].push_back(pObj);
    return S_OK;
}

HRESULT CStage_Select::Ready_Map_Layer_MapPanel(const _wstring& strLayerTag)
{
    CGameObject* pObj{ nullptr };

    CUI_MapPanel::CUI_MAPPANEL_DESC Desc{};

    Desc.fScaleRatioX = 0.32096f;  //986.f / static_cast<_float>(g_iWinSizeX) / 2.4f;
    Desc.fScaleRatioY = 0.56828f;  //982.f / static_cast<_float>(g_iWinSizeY) / 2.4f;
    Desc.fPosRatioX = -0.23f;
    Desc.fPosRatioY = 0.1f;
    Desc.iUILayer = ETOUI(UILAYER::PANEL);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_MapPanel";

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_MapPanel"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_UIs[VIEW_TYPE::MAP].push_back(pObj);

    return S_OK;
}

CStage_Select* CStage_Select::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, function<void(STAGE)> funcBtnCallBack)
{
    CStage_Select* pInstance = new CStage_Select(pDevice, pContext);

    if (FAILED(pInstance->Initialize(funcBtnCallBack)))
    {
        MSG_BOX("Failed to Created: CStage_Select");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStage_Select::Free()
{
    for (auto vecUI : m_UIs) {
        for (auto pObj : vecUI) {
            Safe_Release(pObj);
        }
        vecUI.clear();
    }

    __super::Free();
}
