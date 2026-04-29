#include "CUI_Equipment.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"
#include "CItem_Manager.h"

#include "CAbstractPlayer.h"
#include "CEquipment.h"
#include "CUI_EquipmentSlot.h"

CUI_Equipment::CUI_Equipment(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CUI_Equipment::CUI_Equipment(const CUI_Equipment& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CUI_Equipment::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Equipment::Initialize(void* pArg)
{
    m_pCItem_Manager = CItem_Manager::GetInstance();
    Safe_AddRef(m_pCItem_Manager);

    CUI_EQUIPMENT_DESC* pDesc = static_cast<CUI_EQUIPMENT_DESC*>(pArg);

    m_fScaleRatioX = pDesc->fScaleRatioX;
    m_fScaleRatioY = pDesc->fScaleRatioY;
    m_fPosRatioX = pDesc->fPosRatioX;
    m_fPosRatioY = pDesc->fPosRatioY;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Initialize_Equipment()))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI_EquipmentSlot(TEXT("Layer_UI_EquipmentSlot"))))
        return E_FAIL;

    return S_OK;
}

void CUI_Equipment::Priority_Update(_float fTimeDelta)
{
}

void CUI_Equipment::Parallel_Update(_float fTimeDelta)
{
}

void CUI_Equipment::Update(_float fTimeDelta)
{
}

void CUI_Equipment::Late_Update(_float fTimeDelta)
{
    Sync_Equipment();

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_Equipment::Render()
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

HRESULT CUI_Equipment::Ready_Components()
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

HRESULT CUI_Equipment::Bind_ShaderResources()
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

HRESULT CUI_Equipment::Initialize_Equipment()
{
    const CAbstractPlayer* pPlayer = CInGame_Manager::GetInstance()->Get_Player();

    if (pPlayer == nullptr)
    {
        MSG_BOX("Failed to Initialize Inventory: CUI_Equipment");
        return E_FAIL;
    }

    const CEquipment* pEquipment = pPlayer->Get_Equipment();

    if (pEquipment == nullptr)
    {
        MSG_BOX("Failed to Initialize Equipment: CUI_Equipment");
        return E_FAIL;
    }

    const vector<EQUIPMENT_SLOT>& Equipment = pEquipment->Get_EquipmentsVec();

    if (Equipment.size() == 0)
    {
        MSG_BOX("Failed to Initialize Equipment: CUI_Equipment");
        return E_FAIL;
    }

    m_UIEquipments = Equipment;

    return S_OK;
}

HRESULT CUI_Equipment::Ready_Layer_UI_EquipmentSlot(const _wstring& strLayerTag)
{
    CUI_EquipmentSlot::CUI_EQUIPMENT_SLOT_DESC SlotDesc{};

    SlotDesc.iUILayer = ETOUI(UILAYER::PANEL);

    SlotDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    SlotDesc.wstrTexturePrototypeTag = L"Prototype_Texture_WhiteBlock";

    SlotDesc.eBlendState = CUI_Default::COLOR_ALPHABLEND;
    SlotDesc.vColor = COLOR_TO_FLOAT(51, 61, 63);
    SlotDesc.fImageAlpha = 0.9f;

    SlotDesc.fScaleRatioX = m_fScaleRatioX;

    _float fPanelRatioYPerSlot = m_fScaleRatioY / static_cast<_float>(m_UIEquipments.size());
    SlotDesc.fScaleRatioY = fPanelRatioYPerSlot * 0.9f;

    const _float fGapRow = fPanelRatioYPerSlot;

    const _float fStartPosX = m_fPosRatioX;
    const _float fStartPosY = m_fPosRatioY + (m_fScaleRatioY * 0.5f) - (fPanelRatioYPerSlot * 0.5f);

    SlotDesc.fPosRatioX = m_fPosRatioX;

    for (_uint i = 0; i < 5; ++i)
    {
        SlotDesc.eSlotType = static_cast<CUI_EquipmentSlot::EQUIPMENT_SLOT_TYPE>(i);
        SlotDesc.fPosRatioY = fStartPosY - fGapRow * i;

        Slot_Creator(strLayerTag, &SlotDesc);
    }


    return S_OK;
}

HRESULT CUI_Equipment::Slot_Creator(const _wstring& strLayerTag, void* pSlotDesc)
{
    CUI_EquipmentSlot* pEquipmentSlot = { nullptr };

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_EquipmentSlot"),
        ETOUI(LEVEL::GAMEPLAY), strLayerTag, pSlotDesc, reinterpret_cast<CGameObject**>(&pEquipmentSlot))))
        return E_FAIL;

    m_Slots.push_back(pEquipmentSlot);

    return S_OK;
}

void CUI_Equipment::Sync_Equipment()
{
    const CEquipment* pEquipment = CInGame_Manager::GetInstance()->Get_Player()->Get_Equipment();

    if (m_iEquipmentChangeFlag == pEquipment->Get_ChangeEquipmentChangeFlag())
    {
        return;
    }

    m_iEquipmentChangeFlag = pEquipment->Get_ChangeEquipmentChangeFlag();

    const vector<EQUIPMENT_SLOT>& Equipment = pEquipment->Get_EquipmentsVec();
    m_UIEquipments = Equipment;

    Sync_EquipmentSlot();
}

void CUI_Equipment::Sync_EquipmentSlot()
{
    for (_uint i = 0; i < m_Slots.size(); ++i)
    {
        m_Slots[i]->Sync_Slot_Bg_Item(m_UIEquipments[i].iItemId);
    }
}

CUI_Equipment* CUI_Equipment::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Equipment* pInstance = new CUI_Equipment(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_Equipment");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Equipment::Clone(void* pArg)
{
    CUI_Equipment* pInstance = new CUI_Equipment(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_Equipment");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Equipment::Free()
{
    for (auto& pSlot : m_Slots)
    {
        Safe_Release(pSlot);
    }
    m_Slots.clear();

    Safe_Release(m_pCItem_Manager);

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
