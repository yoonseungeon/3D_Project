#include "CStage_Lobby.h"

#include "CGameInstance.h"

#include "CUI_AniImage.h"
#include "CLobbyTabBtn.h"

CStage_Lobby::CStage_Lobby(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CStage(pDevice, pContext)
{
}

void CStage_Lobby::Enable_Stage()
{
    __super::Enable_Stage();
}

void CStage_Lobby::Disable_Stage()
{
    __super::Disable_Stage();
}

HRESULT CStage_Lobby::Initialize(function<void(STAGE)> funcBtnCallBack)
{
    m_funcBtnCallBack = funcBtnCallBack;

    if (FAILED(Ready_Layer_UI_Image(TEXT("Layer_Deco"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Btn(TEXT("Layer_Btn"))))
        return E_FAIL;

    m_pGameInstance->PlaySound_Loop(ETOUI(SOUND_KEY::BGM_LOBBY), ETOUI(SOUND_CHANNEL_LOBBY::BGM));

    return S_OK;
}

HRESULT CStage_Lobby::Ready_Layer_UI_Image(const _wstring& strLayerTag)
{
    CGameObject* pObj{ nullptr };

    CUI_AniImage::CUI_ANIIMAGE_DESC AniImageDesc{};

    AniImageDesc.fScaleRatioX = 1.1f;
    AniImageDesc.fScaleRatioY = 1.1f;
    AniImageDesc.fPosRatioX = 0.f;
    AniImageDesc.fPosRatioY = 0.f;
    AniImageDesc.iFlipX = false;
    AniImageDesc.iFlipY = false;
    AniImageDesc.iUILayer = ETOUI(UILAYER::BACKGROUND);

    AniImageDesc.eTexPrototypeLV = LEVEL::LOBBY;
    AniImageDesc.eBlendState = CUI_Default::DEFAULT;
    AniImageDesc.wstrTexturePrototypeTag = L"Prototype_Texture_LobbyAni";

    AniImageDesc.fFrameDelay = 0.0166f;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_AniImage"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &AniImageDesc, &pObj)))
        return E_FAIL;

    m_vecGameObjects.push_back(pObj);


    CUI_Image::CUI_IMAGE_DESC Desc{};

    Desc.fScaleRatioX = 0.5f;
    Desc.fScaleRatioY = 1.0f;
    Desc.fPosRatioX = -0.25f;
    Desc.fPosRatioY = 0.f;
    Desc.iFlipX = false;
    Desc.iFlipY = false;
    Desc.iUILayer = ETOUI(UILAYER::DECO_LAYER1);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_LobbyShadow_Deco";

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_vecGameObjects.push_back(pObj);


    Desc.fScaleRatioX = 0.1f;
    Desc.fScaleRatioY = 0.05f;
    Desc.fPosRatioX = -0.445f;
    Desc.fPosRatioY = 0.37f;
    Desc.iFlipX = false;
    Desc.iFlipY = false;
    Desc.iUILayer = ETOUI(UILAYER::DECO_LAYER2);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Lobby_GameStartBtn_Deco1";

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_vecGameObjects.push_back(pObj);


    Desc.fScaleRatioX = 0.2f;
    Desc.fScaleRatioY = 0.07f;
    Desc.fPosRatioX = -0.38f;
    Desc.fPosRatioY = 0.29f;
    Desc.iFlipX = false;
    Desc.iFlipY = false;
    Desc.iUILayer = ETOUI(UILAYER::DECO_LAYER3);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Lobby_GameStartBtn_Deco2";

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_vecGameObjects.push_back(pObj);


    Desc.fScaleRatioX = 0.1f;
    Desc.fScaleRatioY = 0.1f * g_iWinSizeX / g_iWinSizeY;
    Desc.fPosRatioX = -0.475f;
    Desc.fPosRatioY = 0.3f;
    Desc.iFlipX = false;
    Desc.iFlipY = false;
    Desc.iUILayer = ETOUI(UILAYER::DECO_LAYER2);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Lobby_GameStartBtn_Deco3";

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_vecGameObjects.push_back(pObj);

    return S_OK;
}

HRESULT CStage_Lobby::Ready_Layer_Btn(const _wstring& strLayerTag)
{
    CGameObject* pObj{nullptr};

    CLobbyTabBtn::CLOBBY_TAB_BTN_DESC Desc{};

    Desc.fScaleRatioX = 0.2f;
    Desc.fScaleRatioY = 0.055f;
    Desc.fPosRatioX = -0.355f;
    Desc.fPosRatioY = 0.3f;
    Desc.iUILayer = ETOUI(UILAYER::BUTTON);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::ALPHABLEDN_LOBBY_TAB_BTN;
    Desc.wstrTexturePrototypeTag = TEXT("Prototype_Texture_LobbyTabBtnOrange");
    Desc.funcLobbyCallBack = m_funcBtnCallBack;
    Desc.eNextStage = STAGE::SELECT;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_CLobbyTabBtn"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_vecGameObjects.push_back(pObj);


    Desc.fScaleRatioX = 0.2f;
    Desc.fScaleRatioY = 0.055f;
    Desc.fPosRatioX = -0.355f;
    Desc.fPosRatioY = 0.23f;
    Desc.iUILayer = ETOUI(UILAYER::BUTTON);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::ALPHABLEDN_LOBBY_TAB_BTN;
    Desc.wstrTexturePrototypeTag = TEXT("Prototype_Texture_LobbyTabBtnBlue");
    Desc.funcLobbyCallBack = m_funcBtnCallBack;
    Desc.eNextStage = STAGE::SELECT;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_CLobbyTabBtn"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_vecGameObjects.push_back(pObj);


    Desc.fScaleRatioX = 0.15f;
    Desc.fScaleRatioY = 0.055f;
    Desc.fPosRatioX = -0.377f;
    Desc.fPosRatioY = 0.14f;

    Desc.funcLobbyCallBack = m_funcBtnCallBack;
    Desc.eNextStage = STAGE::SELECT;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_CLobbyTabBtn"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_vecGameObjects.push_back(pObj);


    const _float fGap = 0.055f;

    Desc.fPosRatioY -= fGap;

    Desc.funcLobbyCallBack = m_funcBtnCallBack;
    Desc.eNextStage = STAGE::SELECT;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_CLobbyTabBtn"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_vecGameObjects.push_back(pObj);


    Desc.fPosRatioY -= fGap;

    Desc.funcLobbyCallBack = m_funcBtnCallBack;
    Desc.eNextStage = STAGE::SELECT;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_CLobbyTabBtn"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_vecGameObjects.push_back(pObj);


    Desc.fPosRatioY -= fGap;

    Desc.funcLobbyCallBack = m_funcBtnCallBack;
    Desc.eNextStage = STAGE::SELECT;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_CLobbyTabBtn"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_vecGameObjects.push_back(pObj);


    Desc.fPosRatioY -= fGap;

    Desc.funcLobbyCallBack = m_funcBtnCallBack;
    Desc.eNextStage = STAGE::SELECT;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_CLobbyTabBtn"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_vecGameObjects.push_back(pObj);


    Desc.fPosRatioY -= fGap;

    Desc.funcLobbyCallBack = m_funcBtnCallBack;
    Desc.eNextStage = STAGE::SELECT;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_CLobbyTabBtn"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_vecGameObjects.push_back(pObj);


    Desc.fPosRatioY -= fGap;

    Desc.funcLobbyCallBack = m_funcBtnCallBack;
    Desc.eNextStage = STAGE::SELECT;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_CLobbyTabBtn"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_vecGameObjects.push_back(pObj);

    Desc.fPosRatioY -= fGap;

    Desc.funcLobbyCallBack = m_funcBtnCallBack;
    Desc.eNextStage = STAGE::SELECT;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_CLobbyTabBtn"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_vecGameObjects.push_back(pObj);

    Desc.fPosRatioY -= fGap;

    Desc.funcLobbyCallBack = m_funcBtnCallBack;
    Desc.eNextStage = STAGE::SELECT;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_CLobbyTabBtn"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, &pObj)))
        return E_FAIL;

    m_vecGameObjects.push_back(pObj);
    return S_OK;
}

CStage_Lobby* CStage_Lobby::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, function<void(STAGE)> funcBtnCallBack)
{
    CStage_Lobby* pInstance = new CStage_Lobby(pDevice, pContext);

    if (FAILED(pInstance->Initialize(funcBtnCallBack)))
    {
        MSG_BOX("Failed to Created: CStage_Lobby");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStage_Lobby::Free()
{
	__super::Free();
}
