#include "CStage_Select.h"

#include "CGameInstance.h"

#include "CUI_AniImage.h"
#include "CUI_Btn.h"

CStage_Select::CStage_Select(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CStage(pDevice, pContext)
{
}

void CStage_Select::Enable_Stage()
{
}

void CStage_Select::Disable_Stage()
{
}

HRESULT CStage_Select::Initialize(function<void(STAGE)> funcBtnCallBack)
{
    m_funcBtnCallBack = funcBtnCallBack;

    if (FAILED(Ready_Layer_Deco(TEXT("Layer_Deco"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Btn(TEXT("Layer_Btn"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CStage_Select::Ready_Layer_Deco(const _wstring& strLayerTag)
{ 
    CUI_Image::CUI_IMAGE_DESC Desc{};

    Desc.fScaleRatioX = 1.1f;
    Desc.fScaleRatioY = 1.1f;
    Desc.fPosRatioX = 0.f;
    Desc.fPosRatioY = 0.f;
    Desc.iFlipX = false;
    Desc.iFlipY = false;
    Desc.iUILayer = ETOUI(UILAYER::BACKGROUND);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::DEFAULT;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Select_BG";

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CStage_Select::Ready_Layer_Btn(const _wstring& strLayerTag)
{
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
    __super::Free();
}
