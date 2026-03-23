#include "CLevel_Logo.h"

#include "CLevel_Loading.h"
#include "CGameInstance.h"

#include "CUI_Image.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()
{
    if (FAILED(Ready_Layer_CUI_Image(TEXT("Layer_CUI_Image"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
    const _float fIncreaseAlphaTime = 0.f;
    const _float fDecreaseAlphaTime = 2.f;
    const _float fEndTime = 2.7f;


    if (m_eLogoStage != LS_END)
    {
        m_fAccTime += fTimeDelta;
        for (auto pFadeImage : m_FadeImages[m_eLogoStage])
        {
            if (m_fAccTime >= fEndTime)
            {
                pFadeImage->Set_IsInvisible(true);
            }
            else if (m_fAccTime >= fDecreaseAlphaTime)
            {
                pFadeImage->Add_Alpha(-fTimeDelta * 2.f);
            }
            else if (m_fAccTime >= fIncreaseAlphaTime)
            {
                pFadeImage->Set_IsInvisible(false);
                pFadeImage->Add_Alpha(fTimeDelta * 2.f);
            }
        }

        if (m_fAccTime >= fEndTime) {
            m_fAccTime = 0.f;
            m_eLogoStage = static_cast<LogoStage>(static_cast<_int>(m_eLogoStage) + 1);
        }
    }

    if (m_eLogoStage == LS_END)
    {
        CLevel* pLoadingLevel = CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOBBY);

        // 처음 만들 때 받아온 NextLevel 자원 로딩 완료되면, 현재(Level_Loading)를 정리/해제하고 다음 레벨로 전환
        // 자신이 지워져서 바로 return 해줘야 함.(나중에 구조 바꿀 수도 있음)
        if (SUCCEEDED(m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING), pLoadingLevel)))
            return;
    }
}

HRESULT CLevel_Logo::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("Logo 레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_CUI_Image(const _wstring& strLayerTag)
{
    CUI_Image* pImage{ nullptr };

    CUI_Image::CUI_IMAGE_DESC Desc{};

    Desc.fScaleRatioX = 0.75f;
    Desc.fScaleRatioY = 0.25f;
    Desc.fPosRatioX = 0.f;
    Desc.fPosRatioY = 0.f;
    Desc.iFlipX = false;
    Desc.iFlipY = false;
    Desc.iUILayer = ETOUI(UILAYER::BACKGROUND);

    Desc.eTexPrototypeLV = LEVEL::LOGO;
    Desc.eBlendState = CUI_Image::BS_ALPHABLEND;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_NimbleNeuron";

    Desc.fImageAlpha = 0.f;
    Desc.bIsInvisible = true;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::LOGO), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&pImage))))
        return E_FAIL;

    m_FadeImages[LS_1].push_back(pImage);
    ///////////////////////////////////////////////////
    Desc.fScaleRatioX = 0.25f;
    Desc.fScaleRatioY = 0.15f;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Logo";

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::LOGO), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&pImage))))
        return E_FAIL;

    m_FadeImages[LS_2].push_back(pImage);
    ///////////////////////////////////////////////////
    Desc.fScaleRatioX = 0.07f;
    Desc.fScaleRatioY = 0.105f;

    Desc.fPosRatioX = -0.1f;
    Desc.fPosRatioY = 0.05f;

    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Fifteen";
    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::LOGO), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&pImage))))
        return E_FAIL;

    m_FadeImages[LS_3].push_back(pImage);

    Desc.fPosRatioX = 0.f;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Violence";
    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::LOGO), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&pImage))))
        return E_FAIL;

    m_FadeImages[LS_3].push_back(pImage);

    Desc.fPosRatioX = 0.1f;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Sexuality";
    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::LOGO), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&pImage))))
        return E_FAIL;

    m_FadeImages[LS_3].push_back(pImage);

    return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created: CLevel_Logo");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Logo::Free()
{
    __super::Free();

    for (auto& vec : m_FadeImages) {
        for (auto& pFadeImage : vec) {
            Safe_Release(pFadeImage);
        }
        vec.clear();
    }
}
