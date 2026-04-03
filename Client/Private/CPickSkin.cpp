#include "CPickSkin.h"

#include "CGameInstance.h"

#include "CGame_Manager.h"
#include "CCharData_Manager.h"

#include "CUI_Image.h"

CPickSkin::CPickSkin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Btn{ pDevice, pContext }
{
}

CPickSkin::CPickSkin(const CPickSkin& Prototype)
    : CUI_Btn{ Prototype }
{

}

HRESULT CPickSkin::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPickSkin::Initialize(void* pArg)
{
    m_pGame_Manager = CGame_Manager::GetInstance();
    Safe_AddRef(m_pGame_Manager);

    m_pCharData_Manager = CCharData_Manager::GetInstance();
    Safe_AddRef(m_pCharData_Manager);

    CPICKSKIN_DESC* pDesc = static_cast<CPICKSKIN_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_funcSetFullSkin = pDesc->funcSetFullSkin;

    CUI_Image::CUI_IMAGE_DESC Desc{};

    Desc.fScaleRatioX = pDesc->fScaleRatioX;
    Desc.fScaleRatioY = pDesc->fScaleRatioY;
    Desc.fPosRatioX = pDesc->fPosRatioX;
    Desc.fPosRatioY = pDesc->fPosRatioY;
    Desc.eTexPrototypeLV = pDesc->eTexPrototypeLV;
    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_NonFullSkin"; // 더미
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.iUILayer = ETOUI(UILAYER::BUTTON_IMAGE);

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::LOBBY), TEXT("LAYER_UI_Image"), &Desc, reinterpret_cast<CGameObject**>(&m_pSkin))))
        return E_FAIL;


    Desc.wstrTexturePrototypeTag = L"Prototype_Texture_SkinSlot";
    Desc.iUILayer = ETOUI(UILAYER::SLOT);

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::LOBBY), TEXT("LAYER_UI_Image"), &Desc, reinterpret_cast<CGameObject**>(&m_pSlotBg))))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CPickSkin::Priority_Update(_float fTimeDelta)
{
}

void CPickSkin::Parallel_Update(_float fTimeDelta)
{
    // m_bIsInactived의 쓰기는 Level Update에서 일어남.(Late Update 후 LevelUpdate 됨.)
    if (m_bIsInactive == true) {
        return;
    }

    __super::Update_BtnState();

    Execute_Btn(fTimeDelta);
}

void CPickSkin::Update(_float fTimeDelta)
{
    if (m_bIsInactive == true /*|| m_eCurTexState == TEX_STATE::NONE*/) {
        return;
    }

    if (m_bIsClicked) {
        BtnClick();
    }
}

void CPickSkin::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true || m_eCurTexState == TEX_STATE::NONE) {
        return;
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CPickSkin::Render()
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

void CPickSkin::Reset_Skin(LEVEL eTexPrototypeLV, const wstring& wstrTexturePrototypeTag)
{
    m_pSkin->Reset_Texture(eTexPrototypeLV, wstrTexturePrototypeTag);

    const auto pCharInfo = m_pCharData_Manager->Get_CharInfo(m_eCharName);
    
    m_pSlotBg->Set_TexIdx(ETOUI(pCharInfo->Skins[m_iSkinIdx].eSkinClass));
}

void CPickSkin::Set_IsInactive(_bool bIsInactive)
{
    m_bIsInactive = bIsInactive;
    m_pSkin->Set_IsInactive(bIsInactive);
    m_pSlotBg->Set_IsInactive(bIsInactive);
}

void CPickSkin::Set_eCharName(CHAR_NAME eCharName)
{
    m_eCharName = eCharName;
}

void CPickSkin::Set_SkinIdx(_uint iSkinIdx)
{
    m_iSkinIdx = iSkinIdx;
    m_pSkin->Set_TexIdx(iSkinIdx);
}

void CPickSkin::Set_Deselect()
{
    m_bIsSelected = false;
}

void CPickSkin::Set_Select()
{
    m_bIsSelected = true;

    const auto pCharInfo = m_pCharData_Manager->Get_CharInfo(m_eCharName);
    m_funcSetFullSkin(pCharInfo->wstrFullSkinTag, m_iSkinIdx);

    m_pGame_Manager->Set_SelectSkin(m_wstrSkinName);
}

void CPickSkin::Set_SkinName(const wstring& wstrSkinName)
{
    m_wstrSkinName = wstrSkinName;
}

HRESULT CPickSkin::Ready_Components()
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

HRESULT CPickSkin::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", ETOUI(m_eCurTexState))))
        return E_FAIL;

    m_pShaderCom->Bind_RawValue("g_FlipX", &m_iFlipX, sizeof(m_iFlipX));
    m_pShaderCom->Bind_RawValue("g_FlipY", &m_iFlipY, sizeof(m_iFlipY));
    m_pShaderCom->Bind_RawValue("g_Alpha", &m_fImageAlpha, sizeof(m_fImageAlpha));

    return S_OK;
}

void CPickSkin::BtnClick()
{
    m_pGame_Manager->Set_SelectSkin(m_wstrSkinName);
    m_funcCallBack();

    const auto pCharInfo = m_pCharData_Manager->Get_CharInfo(m_eCharName);
    m_funcSetFullSkin(pCharInfo->wstrFullSkinTag, m_iSkinIdx);

    m_bIsSelected = true;
    m_bIsClicked = false;
}

void CPickSkin::Execute_Btn(_float fTimeDelta)
{    
    if (m_bIsSelected == true) {
        m_eCurTexState = TEX_STATE::SELECTED;
        return;
    }

    switch (m_eCurBtnState) {
    case BTN_STATE::NORMAL:
    {
        m_eCurTexState = TEX_STATE::NONE;
        break;
    }

    case BTN_STATE::HOVER:
    {
        m_eCurTexState = TEX_STATE::HOVER;
        break;
    }

    case BTN_STATE::PRESSED:
    {
        m_eCurTexState = TEX_STATE::SELECTED;
        break;
    }

    case BTN_STATE::CLICKED:
    {
        m_eCurTexState = TEX_STATE::SELECTED;
        m_bIsClicked = true;
        break;
    }
    }
}

CPickSkin* CPickSkin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPickSkin* pInstance = new CPickSkin(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CPickSkin");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPickSkin::Clone(void* pArg)
{
    CPickSkin* pInstance = new CPickSkin(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CPickSkin");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPickSkin::Free()
{
    Safe_Release(m_pSlotBg);
    Safe_Release(m_pSkin);

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    Safe_Release(m_pCharData_Manager);
    Safe_Release(m_pGame_Manager);

    __super::Free();
}
