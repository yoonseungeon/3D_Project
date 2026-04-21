#include "CInGameCharLevel_Empty.h"

#include "CGameInstance.h"
#include "CUI_Image.h"

CInGameCharLevel_Empty::CInGameCharLevel_Empty(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CInGameCharLevel_Empty::CInGameCharLevel_Empty(const CInGameCharLevel_Empty& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CInGameCharLevel_Empty::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInGameCharLevel_Empty::Initialize(void* pArg)
{
    CUI_DEFAULT_DESC Desc{};
    Desc.fScaleRatioX = 1.f;
    Desc.fScaleRatioY = 1.f;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI_Image(TEXT("Layer_UI_Image"))))
        return E_FAIL;

    return S_OK;
}

void CInGameCharLevel_Empty::Priority_Update(_float fTimeDelta)
{
}

void CInGameCharLevel_Empty::Parallel_Update(_float fTimeDelta)
{
}

void CInGameCharLevel_Empty::Update(_float fTimeDelta)
{
}

void CInGameCharLevel_Empty::Late_Update(_float fTimeDelta)
{
}

HRESULT CInGameCharLevel_Empty::Render()
{

    return S_OK;
}

HRESULT CInGameCharLevel_Empty::Ready_Layer_UI_Image(const _wstring& strLayerTag)
{
    // Char Panel
    CUI_Image::CUI_IMAGE_DESC CharPanelDesc{};

    CharPanelDesc.fScaleRatioX = 0.035f;
    CharPanelDesc.fScaleRatioY = 0.125f;
    CharPanelDesc.fPosRatioX = -0.180469f;
    CharPanelDesc.fPosRatioY = -0.436945f;

    CharPanelDesc.iUILayer = ETOUI(UILAYER::PANEL);

    CharPanelDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    CharPanelDesc.wstrTexturePrototypeTag = L"Prototype_Texture_InGameCharBg";

    CharPanelDesc.eBlendState = CUI_Default::COLOR_ALPHABLEND;
    CharPanelDesc.vColor = COLOR_TO_FLOAT(33, 45, 51);

    CharPanelDesc.fImageAlpha = 0.8f;

    CharPanelDesc.iFlipX = false;
    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &CharPanelDesc)))
        return E_FAIL;

    CharPanelDesc.fPosRatioX += -0.066f;
    CharPanelDesc.iFlipX = true;
    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &CharPanelDesc)))
        return E_FAIL;
    

    // Circle
    CUI_Image::CUI_IMAGE_DESC CircleDesc{};

    CircleDesc.fScaleRatioY = 0.134861f;
    CircleDesc.fScaleRatioX = CircleDesc.fScaleRatioY * g_iWinSizeY / g_iWinSizeX;
    CircleDesc.fPosRatioX = -0.213517f;
    CircleDesc.fPosRatioY = -0.436945f;
    
    CircleDesc.iUILayer = ETOUI(UILAYER::PANEL);
    
    CircleDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    CircleDesc.wstrTexturePrototypeTag = L"Prototype_Texture_CircleMask";
    
    CircleDesc.eBlendState = CUI_Default::COLOR_ALPHABLEND;
    CircleDesc.vColor = COLOR_TO_FLOAT(59, 88, 99);
    
    CircleDesc.fImageAlpha = 0.8f;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &CircleDesc)))
        return E_FAIL;


    return S_OK;
}

CInGameCharLevel_Empty* CInGameCharLevel_Empty::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInGameCharLevel_Empty* pInstance = new CInGameCharLevel_Empty(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CInGameCharLevel_Empty");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInGameCharLevel_Empty::Clone(void* pArg)
{
    CInGameCharLevel_Empty* pInstance = new CInGameCharLevel_Empty(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CInGameCharLevel_Empty");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInGameCharLevel_Empty::Free()
{
    __super::Free();
}
