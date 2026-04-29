#include "CUI_EquipmentSlot.h"

#include "CGameInstance.h"
#include "CItem_Manager.h"

#include "CUI_ItemImage.h"

CUI_EquipmentSlot::CUI_EquipmentSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Btn{ pDevice, pContext }
{

}

CUI_EquipmentSlot::CUI_EquipmentSlot(const CUI_EquipmentSlot& Prototype)
    : CUI_Btn{ Prototype }
{

}

HRESULT CUI_EquipmentSlot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_EquipmentSlot::Initialize(void* pArg)
{
    CUI_EQUIPMENT_SLOT_DESC* pDesc = static_cast<CUI_EQUIPMENT_SLOT_DESC*>(pArg);

    m_fScaleRatioX = pDesc->fScaleRatioX;
    m_fScaleRatioY = pDesc->fScaleRatioY;
    m_fPosRatioX = pDesc->fPosRatioX;
    m_fPosRatioY = pDesc->fPosRatioY;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI_EquipmentBg(TEXT("Layer_UI_EquipmentBg"))))
        return E_FAIL;

    return S_OK;
}

void CUI_EquipmentSlot::Priority_Update(_float fTimeDelta)
{
}

void CUI_EquipmentSlot::Parallel_Update(_float fTimeDelta)
{
    __super::Update_BtnState();
}

void CUI_EquipmentSlot::Update(_float fTimeDelta)
{
}

void CUI_EquipmentSlot::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_EquipmentSlot::Render()
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

HRESULT CUI_EquipmentSlot::Ready_Components()
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

HRESULT CUI_EquipmentSlot::Bind_ShaderResources()
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

void CUI_EquipmentSlot::BtnClick()
{

}

HRESULT CUI_EquipmentSlot::Ready_Layer_UI_EquipmentBg(const _wstring& strLayerTag)
{


    return S_OK;
}

CUI_EquipmentSlot* CUI_EquipmentSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_EquipmentSlot* pInstance = new CUI_EquipmentSlot(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_EquipmentSlot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_EquipmentSlot::Clone(void* pArg)
{
    CUI_EquipmentSlot* pInstance = new CUI_EquipmentSlot(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_EquipmentSlot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_EquipmentSlot::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
