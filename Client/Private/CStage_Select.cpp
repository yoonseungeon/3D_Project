#include "CStage_Select.h"

#include "CGameInstance.h"

#include "CUI_AniImage.h"
#include "CUI_Btn.h"
#include "CUI_PickPanel.h"
#include "CUI_SkinPanel.h"

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

HRESULT CStage_Select::Initialize(function<void(STAGE)> funcBtnCallBack)
{
    m_funcBtnCallBack = funcBtnCallBack;

    if (FAILED(Ready_Layer_UI_Image(TEXT("Layer_UI_Image"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Btn(TEXT("Layer_Btn"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_PickPanel(TEXT("Layer_PickPanel"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_SkinPanel(TEXT("Layer_SkinPanel"))))
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

HRESULT CStage_Select::Ready_Layer_Btn(const _wstring& strLayerTag)
{
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

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_SkinPanel"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_UIs[VIEW_TYPE::SKIN].push_back(pObj);

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
