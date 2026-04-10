#include "CUI_SkinPanel.h"

#include "CGameInstance.h"

#include "CGame_Manager.h"
#include "CCharData_Manager.h"

#include "CPickSkin.h"
#include "CUI_Image.h"
#include "CLobbySelectBtn.h"


CUI_SkinPanel::CUI_SkinPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CUI_SkinPanel::CUI_SkinPanel(const CUI_SkinPanel& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CUI_SkinPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_SkinPanel::Initialize(void* pArg)
{
    m_pGame_Manaer = CGame_Manager::GetInstance();
    m_pCharData_Manager = CCharData_Manager::GetInstance();
    Safe_AddRef(m_pGame_Manaer);
    Safe_AddRef(m_pCharData_Manager);

    CUI_SKINPANEL_DESC* pDesc = static_cast<CUI_SKINPANEL_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_fScaleRatioX = pDesc->fScaleRatioX;
    m_fScaleRatioY = pDesc->fScaleRatioY;

    m_fPosRatioX = pDesc->fPosRatioX;
    m_fPosRatioY = pDesc->fPosRatioY;

    m_funcChangeSelectMap = pDesc->funcChangeSelectMap;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Layer_CUI_Image(TEXT("Layer_CUI_Image"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_SkinSlot(TEXT("Layer_SkinSlot"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_LobbySelectBtn(TEXT("Layer_LobbySelectBtn"))))
        return E_FAIL;

    return S_OK;
}

void CUI_SkinPanel::Priority_Update(_float fTimeDelta)
{
}

void CUI_SkinPanel::Parallel_Update(_float fTimeDelta)
{
}

void CUI_SkinPanel::Update(_float fTimeDelta)
{
    m_eCurChar = m_pGame_Manaer->Get_SelectedChar();

    if (m_eCurChar != m_ePreChar) {

        if (!m_bSelectBtnOn && m_eCurChar != CHAR_NAME::CHARNAME_END) {
            m_pSelectBtn->Set_IsInactive(false);
            m_bSelectBtnOn = true;
        }

        const auto pCharInfo = m_pCharData_Manager->Get_CharInfo(m_eCurChar);
        const size_t iSkinCnt = pCharInfo->Skins.size();

        for (size_t i = 0; i < m_PickSkins.size(); ++i) {
            if (i < iSkinCnt)
            {
                m_PickSkins[i]->Set_IsInactive(false);
                m_PickSkins[i]->Set_eCharName(pCharInfo->eCharName);
                m_PickSkins[i]->Set_SkinName(pCharInfo->Skins[i].wstrSkinName);
                m_PickSkins[i]->Set_SkinIdx(pCharInfo->Skins[i].iSkinIdx);
                m_PickSkins[i]->Reset_Skin(LEVEL::LOBBY, pCharInfo->wstrSkinTag);

                if (i == 0) {
                    m_PickSkins[i]->Set_Select();
                }
                else {
                    m_PickSkins[i]->Set_Deselect();
                }
            }
            else
            {
                m_PickSkins[i]->Set_Deselect();
                m_PickSkins[i]->Set_IsInactive(true);
            }
           
        }

        m_ePreChar = m_eCurChar;
    }

}

void CUI_SkinPanel::Late_Update(_float fTimeDelta)
{
    //if (m_bIsInactive == true) {
    //    return;
    //}

    //m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_SkinPanel::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(ETOUI(m_eBlendState))))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_SkinPanel::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* For.Com_Texture*/
    if (FAILED(__super::Add_Component(ETOUI(m_eTexPrototypeLV), m_wstrTexturePrototypeTag,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_SkinPanel::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIdx)))
        return E_FAIL;

    m_pShaderCom->Bind_RawValue("g_FlipX", &m_iFlipX, sizeof(m_iFlipX));
    m_pShaderCom->Bind_RawValue("g_FlipY", &m_iFlipY, sizeof(m_iFlipY));
    m_pShaderCom->Bind_RawValue("g_Alpha", &m_fImageAlpha, sizeof(m_fImageAlpha));

    return S_OK;
}

HRESULT CUI_SkinPanel::Ready_Layer_CUI_Image(const _wstring& strLayerTag)
{
    CUI_Image::CUI_IMAGE_DESC Desc{};

    Desc.fScaleRatioX = 0.25f * 1.7f;
    Desc.fScaleRatioY = 0.5f * 1.7f;
    Desc.fPosRatioX = 0.27f;
    Desc.fPosRatioY = 0.f;
    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_NonFullSkin";
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.iUILayer = ETOUI(UILAYER::DECO_LAYER1);

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::LOBBY), TEXT("LAYER_UI_Image"), &Desc, reinterpret_cast<CGameObject**>(&m_pFullSkin))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_SkinPanel::Ready_Layer_SkinSlot(const _wstring& strLayerTag)
{   
    CPickSkin* pPickSkin{ nullptr };

    constexpr _float fSlotsPerRow = 3.5f;
    constexpr _float fMarginX = 0.005f;

    const _float fSlotWidth = (m_fScaleRatioX / fSlotsPerRow) - (fMarginX * 2.f);
    const _float fSlotHeight = m_fScaleRatioY;

    const _float fStartPosX = m_fPosRatioX - (m_fScaleRatioX / 2.f) + (fSlotWidth / 2.f) + fMarginX;
    const _float fGapX = fMarginX + fSlotWidth;

    const _float fStartPosY = m_fPosRatioY + (m_fScaleRatioY / 2.f) - (fSlotHeight / 2.f);

    CPickSkin::CPICKSKIN_DESC Desc{};

    Desc.fScaleRatioX = fSlotWidth;
    Desc.fScaleRatioY = fSlotHeight;
    Desc.fPosRatioX = fStartPosX;
    Desc.fPosRatioY = fStartPosY;

    Desc.iUILayer = ETOUI(UILAYER::BUTTON);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_SkinSlotFrame";

    Desc.funcCallBack = [this]()->void
        {
            for (auto pPickSkin : m_PickSkins) {
                pPickSkin->Set_Deselect();
            }
        };

    Desc.funcSetFullSkin = [this](const wstring FullSkinTexTag, const _uint iSkinIdx)->void
        {
            m_pFullSkin->Reset_Texture(LEVEL::LOBBY, FullSkinTexTag);
            m_pFullSkin->Set_TexIdx(iSkinIdx);
        };

    for (_uint i = 0; i < 4; ++i)
    {
        if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_PickSkin"),
            ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&pPickSkin))))
            return E_FAIL;

        Desc.fPosRatioX += fGapX;

        pPickSkin->Set_IsInactive(true);
        m_PickSkins.push_back(pPickSkin);
    }


    return S_OK;
}

HRESULT CUI_SkinPanel::Ready_Layer_LobbySelectBtn(const _wstring& strLayerTag)
{
    CLobbySelectBtn::CLOBBY_SELECT_BTN_DESC Desc{};

    Desc.fScaleRatioX = 0.18f;
    Desc.fScaleRatioY = 0.07f;
    Desc.fPosRatioX = 0.28f;
    Desc.fPosRatioY = -0.4f;
    Desc.iUILayer = ETOUI(UILAYER::BUTTON);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_LobbySelectBtn";

    Desc.funcCallBack = [this]()->void
        {
            m_funcChangeSelectMap();
            for (auto& pPiskSkin : m_PickSkins) {
                pPiskSkin->Set_IsInactive(true);
            }
        };

    Desc.wstrText = L"캐릭터 선택";
    Desc.fOffsetX = 70.f;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::LOBBY), TEXT("Prototype_GameObject_LobbySelectBtn"),
        ETOUI(LEVEL::LOBBY), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&m_pSelectBtn))))
        return E_FAIL;

    m_pSelectBtn->Set_IsInactive(true);

    return S_OK;
}

CUI_SkinPanel* CUI_SkinPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_SkinPanel* pInstance = new CUI_SkinPanel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_SkinPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_SkinPanel::Clone(void* pArg)
{
    CUI_SkinPanel* pInstance = new CUI_SkinPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_SkinPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_SkinPanel::Free()
{
    for (auto& pPickSkin : m_PickSkins) {
        Safe_Release(pPickSkin);
    }
    m_PickSkins.clear();

    Safe_Release(m_pSelectBtn);
    Safe_Release(m_pFullSkin);

    Safe_Release(m_pGame_Manaer);
    Safe_Release(m_pCharData_Manager);

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
