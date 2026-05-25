#include "CLevel_Ending.h"

#include "CGameInstance.h"

#include "CUI_AniImage.h"

CLevel_Ending::CLevel_Ending(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Ending::Initialize()
{
    m_pGameInstance->Set_Parallel_Update_Mode(PARALLEL_UPDATE_MODE::PARALLEL);

    if (FAILED(Ready_Layer_Ending(TEXT("Layer_Ending"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Ending::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Ending::Render()
{
#ifdef _DEBUG
    //SetWindowText(g_hWnd, TEXT("Ending 레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_Ending::Ready_Layer_Ending(const _wstring& strLayerTag)
{
    CUI_AniImage::CUI_ANIIMAGE_DESC AniImageDesc{};

    AniImageDesc.fScaleRatioX = 1.f;
    AniImageDesc.fScaleRatioY = 1.f;
    AniImageDesc.fPosRatioX = 0.f;
    AniImageDesc.fPosRatioY = 0.f;
    AniImageDesc.iUILayer = ETOUI(UILAYER::DECO_LAYER1);

    AniImageDesc.eTexPrototypeLV = LEVEL::ENDING;
    AniImageDesc.eBlendState = CUI_Default::DEFAULT;
    AniImageDesc.wstrTexturePrototypeTag = L"Prototype_Texture_LiDailin_Win";

    AniImageDesc.fFrameDelay = 0.0166f * 2.f;

    AniImageDesc.bLoop = false;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_AniImage"),
        ETOUI(LEVEL::ENDING), strLayerTag, &AniImageDesc)))
        return E_FAIL;

    return S_OK;
}

CLevel_Ending* CLevel_Ending::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Ending* pInstance = new CLevel_Ending(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created: CLevel_Ending");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Ending::Free()
{
    __super::Free();
}
