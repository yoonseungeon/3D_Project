#include "CSharedUI_Manager.h"

#include "CGameInstance.h"

#include "CUI_ItemBoxPanel.h"

IMPLEMENT_SINGLETON(CSharedUI_Manager)

CSharedUI_Manager::CSharedUI_Manager()
{
    if (FAILED(Initialize()))
        MSG_BOX("CSharedUI_Manager: Error");
}

HRESULT CSharedUI_Manager::Initialize()
{
    m_pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(m_pGameInstance);

    if (FAILED(Ready_Layer_UI_ItemBox_Inventory(TEXT("Layer_UI_ItemBox_Inventory"))))
        return E_FAIL;

    return S_OK;
}

void CSharedUI_Manager::PopUp_ItemBoxUI(CInvenOwner* InvenOwner)
{
    m_pItemBoxPanel->PopUp_ItemBoxUI(InvenOwner);
}

void CSharedUI_Manager::PopDown_ItemBoxUI()
{
    m_pItemBoxPanel->PopDown_ItemBoxUI();
}

HRESULT CSharedUI_Manager::Ready_Layer_UI_ItemBox_Inventory(const _wstring& strLayerTag)
{
    CUI_ItemBoxPanel::CUI_ITEMBOXPANEL_DESC ItemBoxPanelDesc{};

    ItemBoxPanelDesc.fScaleRatioX = 0.218750f;
    ItemBoxPanelDesc.fScaleRatioY = 0.263889f;
    ItemBoxPanelDesc.fPosRatioX = 0.f;
    ItemBoxPanelDesc.fPosRatioY = 0.f;

    ItemBoxPanelDesc.iUILayer = ETOUI(UILAYER::BACKGROUND);

    ItemBoxPanelDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    ItemBoxPanelDesc.wstrTexturePrototypeTag = L"Prototype_Texture_WhiteBlock";

    ItemBoxPanelDesc.eBlendState = CUI_Default::COLOR_ALPHABLEND;
    ItemBoxPanelDesc.vColor = _float3(0.055f, 0.031f, 0.086f);

    ItemBoxPanelDesc.fImageAlpha = 0.7f;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_ItemBoxPanel"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &ItemBoxPanelDesc, reinterpret_cast<CGameObject**>(&m_pItemBoxPanel))))
        return E_FAIL;

    m_pItemBoxPanel->Set_IsInactive(true);

    return S_OK;
}

void CSharedUI_Manager::Free()
{
    Safe_Release(m_pItemBoxPanel);

    Safe_Release(m_pGameInstance);

    __super::Free();
}