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
}

void CStage_Lobby::Disable_Stage()
{
}

HRESULT CStage_Lobby::Initialize(function<void(STAGE)> funcBtnCallBack)
{
    m_funcBtnCallBack = funcBtnCallBack;

    if (FAILED(Ready_Layer_Deco(TEXT("Layer_Deco"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Btn(TEXT("Layer_Btn"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CStage_Lobby::Ready_Layer_Deco(const _wstring& strLayerTag)
{

    CUI_AniImage::CUI_ANIIMAGE_DESC Desc{};

    Desc.fScaleRatioX = 1.1f;
    Desc.fScaleRatioY = 1.1f;
    Desc.fPosRatioX = 0.f;
    Desc.fPosRatioY = 0.f;
    Desc.iFlipX = false;
    Desc.iFlipY = false;
    Desc.iUILayer = ETOUI(UILAYER::BACKGROUND);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::DEFAULT;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_LobbyAni";

    Desc.fFrameDelay = 0.0166f;

    //if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_AniImage"),
    //    ETOUI(LEVEL::LOBBY), strLayerTag, &Desc)))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CStage_Lobby::Ready_Layer_Btn(const _wstring& strLayerTag)
{
    CLobbyTabBtn::CLOBBY_TAB_BTN_DESC Desc{};

    Desc.fScaleRatioX = 0.2f;
    Desc.fScaleRatioY = 0.07f;
    Desc.fPosRatioX = -0.35f;
    Desc.fPosRatioY = 0.27f;
    Desc.iUILayer = ETOUI(UILAYER::BUTTON);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::ALPHABLEDN_GAUGE;
    Desc.wstrTexturePrototypeTag = TEXT("Prototype_Texture_LobbyTabBtnOrange");
    Desc.funcLobbyCallBack = m_funcBtnCallBack;
    Desc.eNextStage = STAGE::SELECT;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_CLobbyTabBtn"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc)))
        return E_FAIL;

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
