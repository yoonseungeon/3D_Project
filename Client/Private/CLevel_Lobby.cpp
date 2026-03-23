#include "CLevel_Lobby.h"

#include "CLevel_Loading.h"
#include "CGameInstance.h"

#include "CUI_AniImage.h"

CLevel_Lobby::CLevel_Lobby(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Lobby::Initialize()
{
    if (FAILED(Ready_Layer_CUI_Image(TEXT("Layer_CUI_Image"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Lobby::Update(_float fTimeDelta)
{
    if (GetKeyState(VK_RETURN) & 0x8000)
    {
        CLevel* pLoadingLevel = CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY);

        // 처음 만들 때 받아온 NextLevel 자원 로딩 완료되면, 현재(Level_Loading)를 정리/해제하고 다음 레벨로 전환
        // 자신이 지워져서 바로 return 해줘야 함.(나중에 구조 바꿀 수도 있음)
        if (SUCCEEDED(m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING), pLoadingLevel)))
            return;
    }
}

HRESULT CLevel_Lobby::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("Lobby 레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_Lobby::Ready_Layer_CUI_Image(const _wstring& strLayerTag)
{

    CUI_AniImage::CUI_ANIIMAGE_DESC Desc{};

    Desc.fScaleRatioX = 1.f;
    Desc.fScaleRatioY = 1.f;
    Desc.fPosRatioX = 0.f;
    Desc.fPosRatioY = 0.f;
    Desc.iFlipX = false;
    Desc.iFlipY = false;
    Desc.iUILayer = ETOUI(UILAYER::BACKGROUND);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Image::BS_DEFAULT;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_LobbyAni";

    Desc.fFrameDelay = 0.0166f;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_AniImage"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

CLevel_Lobby* CLevel_Lobby::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Lobby* pInstance = new CLevel_Lobby(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created: CLevel_Lobby");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Lobby::Free()
{
    __super::Free();
}
