#include "CUI_InventorySlot.h"

#include "CGameInstance.h"
#include "CItem_Manager.h"

#include "CUI_InvenItemBg.h"
#include "CUI_ItemImage.h"

CUI_InventorySlot::CUI_InventorySlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Btn{ pDevice, pContext }
{

}

CUI_InventorySlot::CUI_InventorySlot(const CUI_InventorySlot& Prototype)
    : CUI_Btn{ Prototype }
{

}

HRESULT CUI_InventorySlot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_InventorySlot::Initialize(void* pArg)
{
    CUI_INVENTORYSLOT_DESC* pDesc = static_cast<CUI_INVENTORYSLOT_DESC*>(pArg);

    m_fScaleRatioX = pDesc->fScaleRatioX;
    m_fScaleRatioY = pDesc->fScaleRatioY;
    m_fPosRatioX = pDesc->fPosRatioX;
    m_fPosRatioY = pDesc->fPosRatioY;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI_InventoryItemBg(TEXT("Layer_UI_InventoryItemBg"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI_InventoryItem(TEXT("Layer_UI_InventoryItem"))))
        return E_FAIL;
    
    return S_OK;
}

void CUI_InventorySlot::Priority_Update(_float fTimeDelta)
{
}

void CUI_InventorySlot::Parallel_Update(_float fTimeDelta)
{
    __super::Update_BtnState();
}

void CUI_InventorySlot::Update(_float fTimeDelta)
{
}

void CUI_InventorySlot::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_InventorySlot::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(m_eBlendState)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CUI_InventorySlot::Sync_Slot_Bg_Item(_int iItemId, _uint iItemCnt)
{
    Set_ItemBg(iItemId, iItemCnt);
    Set_Item(iItemId, iItemCnt);

    iItemCnt = iItemCnt;
}

void CUI_InventorySlot::Set_ItemBg(_int iItemId, _uint iItemCnt)
{
    if (iItemId == -1)
    {
        m_pItemBg->Set_IsInactive(true);
        m_pItemBg->Set_ItemCnt(0);
        return;
    }

    m_pItemBg->Set_IsInactive(false);

    const ITEM_DESC* pItemDesc = CItem_Manager::GetInstance()->Find_ItemInfo(iItemId);
    m_pItemBg->Set_TexIdx(ETOUI(pItemDesc->eGrade));    

    m_pItemBg->Set_ItemCnt(iItemCnt);
}

void CUI_InventorySlot::Set_Item(_int iItemId, _uint iItemCnt)
{
    if (iItemId == -1)
    {
        m_pItemImage->Set_IsInactive(true);
        return;
    }

    m_pItemImage->Set_IsInactive(false);

    m_pItemImage->Set_CurItem(iItemId, iItemCnt);
}

HRESULT CUI_InventorySlot::Ready_Components()
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

HRESULT CUI_InventorySlot::Bind_ShaderResources()
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
    m_pShaderCom->Bind_RawValue("g_Color", &m_vColor, sizeof(m_vColor));

    return S_OK;
}

HRESULT CUI_InventorySlot::Ready_Layer_UI_InventoryItemBg(const _wstring& strLayerTag)
{
    CUI_InvenItemBg::CUI_INVENITEMBG_DESC Desc{};

    Desc.fScaleRatioX = m_fScaleRatioX;
    Desc.fScaleRatioY = m_fScaleRatioY;
    Desc.fPosRatioX = m_fPosRatioX;
    Desc.fPosRatioY = m_fPosRatioY;

    Desc.iUILayer = ETOUI(UILAYER::PANEL_DECO);

    Desc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Ico_ItemGradeBg";

    Desc.eBlendState = CUI_Default::DEFAULT;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_InvenItemBg"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&m_pItemBg))))
        return E_FAIL;

    m_pItemBg->Set_IsInactive(true);

    return S_OK;
}

HRESULT CUI_InventorySlot::Ready_Layer_UI_InventoryItem(const _wstring& strLayerTag)
{
    CUI_ItemImage::CUI_ITEMIMAGE_DESC Desc{};

    Desc.fScaleRatioX = m_fScaleRatioX;
    Desc.fScaleRatioY = m_fScaleRatioY;
    Desc.fPosRatioX = m_fPosRatioX;
    Desc.fPosRatioY = m_fPosRatioY;

    Desc.iUILayer = ETOUI(UILAYER::SLOT);

    Desc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Item";

    Desc.eBlendState = CUI_Default::ALPHABLEND;

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_ItemImage"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&m_pItemImage))))
        return E_FAIL;

    m_pItemImage->Set_IsInactive(true);

    return S_OK;
}

void CUI_InventorySlot::BtnClick()
{

}

CUI_InventorySlot* CUI_InventorySlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_InventorySlot* pInstance = new CUI_InventorySlot(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_InventorySlot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_InventorySlot::Clone(void* pArg)
{
    CUI_InventorySlot* pInstance = new CUI_InventorySlot(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_InventorySlot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_InventorySlot::Free()
{
    Safe_Release(m_pItemBg);
    Safe_Release(m_pItemImage);

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
