#include "CUI_CharSkillPanel.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"

#include "CAbstractPlayer.h"
#include "CUI_StackSkillIcon.h"
#include "CUI_NormalSkillIcon.h"

CUI_CharSkillPanel::CUI_CharSkillPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Default{ pDevice, pContext }
{

}

CUI_CharSkillPanel::CUI_CharSkillPanel(const CUI_CharSkillPanel& Prototype)
    : CUI_Default{ Prototype }
{

}

HRESULT CUI_CharSkillPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_CharSkillPanel::Initialize(void* pArg)
{
    CUI_CAHRSKILLPANEL_DESC* pDesc = static_cast<CUI_CAHRSKILLPANEL_DESC*>(pArg);

    m_fScaleRatioX = pDesc->fScaleRatioX;
    m_fScaleRatioY = pDesc->fScaleRatioY;
    m_fPosRatioX = pDesc->fPosRatioX;
    m_fPosRatioY = pDesc->fPosRatioY;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI_Image(TEXT("Layer_UI_Image"))))
        return E_FAIL;

    return S_OK;
}

void CUI_CharSkillPanel::Priority_Update(_float fTimeDelta)
{
}

void CUI_CharSkillPanel::Parallel_Update(_float fTimeDelta)
{
}

void CUI_CharSkillPanel::Update(_float fTimeDelta)
{
}

void CUI_CharSkillPanel::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_CharSkillPanel::Render()
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

HRESULT CUI_CharSkillPanel::Ready_Components()
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

HRESULT CUI_CharSkillPanel::Bind_ShaderResources()
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

HRESULT CUI_CharSkillPanel::Ready_Layer_UI_Image(const _wstring& strLayerTag)
{
    SKILL_DESC Desc{};
    CAbstractPlayer* pPlayer = CInGame_Manager::GetInstance()->Get_Player();
    pPlayer->Get_SkillSlotType(SKILL_SLOT::Q, Desc);

    // Q
    CUI_StackSkillIcon::CUI_STACKSKILLICON_DESC QDesc{};

    QDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    QDesc.iUILayer = ETOUI(UILAYER::SLOT);

    QDesc.fScaleRatioX = 0.04f;
    QDesc.fScaleRatioY = 0.071f;
    QDesc.fPosRatioX = m_fPosRatioX + (-0.5f) * m_fScaleRatioX + QDesc.fScaleRatioX * 0.5f;
    QDesc.fPosRatioY = -0.4f;
    QDesc.wstrTexturePrototypeTag = L"Prototype_Texture_LiDailin_Q";
    QDesc.eBlendState = CUI_Default::DEFAULT;

    QDesc.eSkillSlot = Desc.eSkillSlot;

    SkillSlotCreator(strLayerTag, Desc.eCoolDownType, &QDesc);
    
    // W
    pPlayer->Get_SkillSlotType(SKILL_SLOT::W, Desc);

    CUI_NormalSkillIcon::CUI_NORMALSKILLICON_DESC WDesc{};

    WDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    WDesc.iUILayer = ETOUI(UILAYER::SLOT);
    
    WDesc.fScaleRatioX = 0.04f;
    WDesc.fScaleRatioY = 0.071f;
    WDesc.fPosRatioX = 0.0f;
    WDesc.fPosRatioY = -0.4f;
    WDesc.wstrTexturePrototypeTag = L"Prototype_Texture_LiDailin_W";
    WDesc.eBlendState = CUI_Default::DEFAULT;

    WDesc.eSkillSlot = Desc.eSkillSlot;

    SkillSlotCreator(strLayerTag, Desc.eCoolDownType, &WDesc);

    // E
    pPlayer->Get_SkillSlotType(SKILL_SLOT::E, Desc);

    CUI_NormalSkillIcon::CUI_NORMALSKILLICON_DESC EDesc{};

    EDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
    EDesc.iUILayer = ETOUI(UILAYER::SLOT);
    
    EDesc.fScaleRatioX = 0.04f;
    EDesc.fScaleRatioY = 0.071f;
    EDesc.fPosRatioX = 0.07f;
    EDesc.fPosRatioY = -0.4f;
    EDesc.wstrTexturePrototypeTag = L"Prototype_Texture_LiDailin_E";
    EDesc.eBlendState = CUI_Default::DEFAULT;

    EDesc.eSkillSlot = Desc.eSkillSlot;

    SkillSlotCreator(strLayerTag, Desc.eCoolDownType, &EDesc);

    // R
    pPlayer->Get_SkillSlotType(SKILL_SLOT::R, Desc);

    CUI_NormalSkillIcon::CUI_NORMALSKILLICON_DESC RDesc{};

   RDesc.eTexPrototypeLV = LEVEL::GAMEPLAY;
   RDesc.iUILayer = ETOUI(UILAYER::SLOT);
   
   RDesc.fScaleRatioX = 0.04f;
   RDesc.fScaleRatioY = 0.071f;
   RDesc.fPosRatioX = 0.14f;
   RDesc.fPosRatioY = -0.4f;
   RDesc.wstrTexturePrototypeTag = L"Prototype_Texture_LiDailin_R";
   RDesc.eBlendState = CUI_Default::DEFAULT;

   RDesc.eSkillSlot = Desc.eSkillSlot;

   SkillSlotCreator(strLayerTag, Desc.eCoolDownType, &RDesc);

    return S_OK;
}

HRESULT CUI_CharSkillPanel::SkillSlotCreator(const _wstring& strLayerTag, COOLDOWN_TYPE eCoolDownType, void* pDesc)
{
    switch (eCoolDownType)
    {
        case COOLDOWN_TYPE::NORMAL:
        {   
            if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_NormalSkillIcon"),
                ETOUI(LEVEL::GAMEPLAY), strLayerTag, pDesc)))
                return E_FAIL;
            break;
        }
        case COOLDOWN_TYPE::STACK:
        {
            if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_CUI_StackSkillIcon"),
                ETOUI(LEVEL::GAMEPLAY), strLayerTag, pDesc)))
                return E_FAIL;
            break;
        }
    }

    return S_OK;
}

CUI_CharSkillPanel* CUI_CharSkillPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_CharSkillPanel* pInstance = new CUI_CharSkillPanel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_CharSkillPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_CharSkillPanel::Clone(void* pArg)
{
    CUI_CharSkillPanel* pInstance = new CUI_CharSkillPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_CharSkillPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_CharSkillPanel::Free()
{ 
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
