#include "CUI_CraftSlot.h"

#include "CGameInstance.h"
#include "CItem_Manager.h"

#include "CUI_Image.h"
#include "CUI_ItemImage.h"

CUI_CraftSlot::CUI_CraftSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Btn{ pDevice, pContext }
{

}

CUI_CraftSlot::CUI_CraftSlot(const CUI_CraftSlot& Prototype)
    : CUI_Btn{ Prototype }
{

}

HRESULT CUI_CraftSlot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_CraftSlot::Initialize(void* pArg)
{
    CUI_CRAFTSLOT_DESC* pDesc = static_cast<CUI_CRAFTSLOT_DESC*>(pArg);

    m_fScaleRatioX = pDesc->fScaleRatioX;
    m_fScaleRatioY = pDesc->fScaleRatioY;
    m_fPosRatioX = pDesc->fPosRatioX;
    m_fPosRatioY = pDesc->fPosRatioY;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI_CraftItem(TEXT("Layer_UI_ICraftItem"))))
        return E_FAIL;

    return S_OK;
}

void CUI_CraftSlot::Priority_Update(_float fTimeDelta)
{
}

void CUI_CraftSlot::Parallel_Update(_float fTimeDelta)
{
    __super::Update_BtnState();
}

void CUI_CraftSlot::Update(_float fTimeDelta)
{
}

void CUI_CraftSlot::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true)
    {
        return;
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_CraftSlot::Render()
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

void CUI_CraftSlot::Set_Item(_int iItemId)
{
  /*  if (iItemId == -1)
    {
        m_pItemImage->Set_IsInactive(true);
        return;
    }

    m_pItemImage->Set_IsInactive(false);

    const ITEM_DESC* pItemDesc = CItem_Manager::GetInstance()->Find_ItemInfo(iItemId);
    m_pItemImage->Set_CurItem(iItemId);*/
}

HRESULT CUI_CraftSlot::Ready_Components()
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
    if (FAILED(__super::Add_Component(ETOUI(m_eTexPrototypeLV), TEXT("Prototype_Texture_Ico_ItemGradeBg"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_CraftSlot::Bind_ShaderResources()
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

HRESULT CUI_CraftSlot::Ready_Layer_UI_CraftItem(const _wstring& strLayerTag)
{
    //CUI_Image::CUI_IMAGE_DESC Desc{};

    //Desc.fScaleRatioX = m_fScaleRatioX;
    //Desc.fScaleRatioY = m_fScaleRatioY;
    //Desc.fPosRatioX = m_fPosRatioX;
    //Desc.fPosRatioY = m_fPosRatioY;

    //Desc.iUILayer = ETOUI(UILAYER::SLOT);

    //Desc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    //Desc.wstrTexturePrototypeTag = L"Prototype_Texture_Item";

    //Desc.eBlendState = CUI_Default::ALPHABLEND;

    //if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_ItemImage"),
    //    ETOUI(LEVEL::GAMEPLAY), strLayerTag, &Desc, reinterpret_cast<CGameObject**>(&m_pItemImage))))
    //    return E_FAIL;

    //m_pItemImage->Set_IsInactive(true);

    return S_OK;
}

void CUI_CraftSlot::BtnClick()
{

}

CUI_CraftSlot* CUI_CraftSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_CraftSlot* pInstance = new CUI_CraftSlot(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_CraftSlot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_CraftSlot::Clone(void* pArg)
{
    CUI_CraftSlot* pInstance = new CUI_CraftSlot(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_CraftSlot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_CraftSlot::Free()
{
    Safe_Release(m_pItemImage);

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
