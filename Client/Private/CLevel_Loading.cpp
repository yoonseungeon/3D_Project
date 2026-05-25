#include "CLevel_Loading.h"
#include "CLoader.h"

#include "CGameInstance.h"
#include "CLevel_Logo.h"
#include "CLevel_Lobby.h"
#include "CLevel_GamePlay.h"
#include "CLevel_Ending.h"

#include "CUI_Image.h"


CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
    m_pGameInstance->Set_Parallel_Update_Mode(PARALLEL_UPDATE_MODE::SINGLE);

    m_eNextLevelID = eNextLevelID;

    m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
    if (m_pLoader == nullptr)
        return E_FAIL;

    if (FAILED(Ready_Layer_UI_Image(TEXT("Layer_UI_Image"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
    if (m_pLoader->isFinished() == true)
    {
        CLevel* pNextLevel = { nullptr };

        switch (m_eNextLevelID)
        {
        case LEVEL::LOGO:
            pNextLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
            break;
        case LEVEL::LOBBY:
            pNextLevel = CLevel_Lobby::Create(m_pDevice, m_pContext);
            break;
        case LEVEL::GAMEPLAY:
            pNextLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
            break;
        case LEVEL::ENDING:
            pNextLevel = CLevel_Ending::Create(m_pDevice, m_pContext);
            break;
        }

        if (nullptr == pNextLevel)
        {
            MSG_BOX("CLevel_Loading.cpp - Failed to Change Level");
            return;
        }

        // 처음 만들 때 받아온 NextLevel 자원 로딩 완료되면, 현재(Level_Loading)를 정리/해제하고 다음 레벨로 전환
        // 자신이 지워져서 바로 return 해줘야 함.(나중에 구조 바꿀 수도 있음)
        if (SUCCEEDED(m_pGameInstance->Change_Level(ETOI(m_eNextLevelID), pNextLevel)))
            return;
    }
}

HRESULT CLevel_Loading::Render()
{
    m_pLoader->Show_Loading_Status();

    return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_UI_Image(const _wstring& strLayerTag)
{
    CUI_Image::CUI_IMAGE_DESC Desc{};

    Desc.eTexPrototypeLV = LEVEL::STATIC;

    switch (m_eNextLevelID) {
        case LEVEL::LOGO:
        {
            break;
        }
        case LEVEL::LOBBY:
        {
            Desc.iUILayer = ETOUI(UILAYER::DECO_LAYER1);

            Desc.fScaleRatioX = 0.05f;
            Desc.fScaleRatioY = Desc.fScaleRatioX * static_cast<_float>(g_iWinSizeX) / static_cast<_float>(g_iWinSizeY);
            Desc.fPosRatioX = 0.f;
            Desc.fPosRatioY = 0.07f;
            Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Simbol";
            Desc.eBlendState = CUI_Default::ALPHABLEND;
            Desc.iFlipX = false;
            Desc.iFlipY = false;

            if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
                ETOUI(LEVEL::LOADING), strLayerTag, &Desc)))
                return E_FAIL;

            Desc.fScaleRatioX = 0.1f;
            Desc.fScaleRatioY -= 0.05f;
            Desc.fPosRatioX = -0.1f;
            Desc.fPosRatioY;
            Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Loading_Line";
            Desc.eBlendState;
            Desc.iFlipX = false;
            Desc.iFlipY = false;

            if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
                ETOUI(LEVEL::LOADING), strLayerTag, &Desc)))
                return E_FAIL;

            Desc.fScaleRatioX;
            Desc.fScaleRatioY;
            Desc.fPosRatioX *= -1.f;
            Desc.fScaleRatioY;
            Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Loading_Line";
            Desc.eBlendState;
            Desc.iFlipX = true;
            Desc.iFlipY = false;

            if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
                ETOUI(LEVEL::LOADING), strLayerTag, &Desc)))
                return E_FAIL;

            break;
        }
        case LEVEL::GAMEPLAY:
        {
            Desc.iUILayer = ETOUI(UILAYER::BACKGROUND);

            Desc.fScaleRatioX = 1.f;
            Desc.fScaleRatioY = 1.f;
            Desc.fPosRatioX = 0.f;
            Desc.fPosRatioY = 0.f;
            Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Img_Loading";
            Desc.eBlendState = CUI_Default::DEFAULT;
            Desc.iFlipX = false;
            Desc.iFlipY = false;

            if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
                ETOUI(LEVEL::LOADING), strLayerTag, &Desc)))
                return E_FAIL;

            break;
        }
        case LEVEL::ENDING:
        {
            Desc.iUILayer = ETOUI(UILAYER::BACKGROUND);

            Desc.fScaleRatioX = 1.f;
            Desc.fScaleRatioY = 1.f;
            Desc.fPosRatioX = 0.f;
            Desc.fPosRatioY = 0.f;
            Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Img_Blackblock";
            Desc.eBlendState = CUI_Default::DEFAULT;

            if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
                ETOUI(LEVEL::LOADING), strLayerTag, &Desc)))
                return E_FAIL;

            break;
        }
    }

    return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
    CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

    if (FAILED(pInstance->Initialize(eNextLevelID)))
    {
        MSG_BOX("Failed to Created: CLevel_Loading");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Loading::Free()
{
    Safe_Release(m_pLoader);
     
    __super::Free();
}
