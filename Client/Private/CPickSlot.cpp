#include "CPickSlot.h"

#include "CGameInstance.h"
#include "CUI_Image.h"
#include "CGame_Manager.h"

CPickSlot::CPickSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Btn{ pDevice, pContext }
{
}

CPickSlot::CPickSlot(const CPickSlot& Prototype)
    : CUI_Btn{ Prototype }
{

}

HRESULT CPickSlot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPickSlot::Initialize(void* pArg)
{
    m_pGame_Manager = CGame_Manager::GetInstance();
    Safe_AddRef(m_pGame_Manager);

    CPICKSLOT_DESC* pDesc = static_cast<CPICKSLOT_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_eCharName = pDesc->eCharName;

    CUI_Image::CUI_IMAGE_DESC Desc{};

    Desc.fScaleRatioX = pDesc->fScaleRatioX * 0.98f;
    Desc.fScaleRatioY = pDesc->fScaleRatioY *0.955f;
    Desc.fPosRatioX = pDesc->fPosRatioX;
    Desc.fPosRatioY = pDesc->fPosRatioY;
    Desc.eTexPrototypeLV = pDesc->eTexPrototypeLV;
    Desc.wstrTexturePrototypeTag = pDesc->tCharInfo.wstrTexturePrototypeTag;
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.iUILayer = ETOUI(UILAYER::BUTTON_IMAGE);

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
          ETOUI(LEVEL::LOBBY), TEXT("LAYER_UI_Image"), &Desc, reinterpret_cast<CGameObject**>(&m_pChar))))
          return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CPickSlot::Priority_Update(_float fTimeDelta)
{
}

void CPickSlot::Parallel_Update(_float fTimeDelta)
{
    // m_bIsInactived의 쓰기는 Level Update에서 일어남.(Late Update 후 LevelUpdate 됨.)
    if (m_bIsInactive == true) {
        return;
    }

    __super::Update_BtnState();

    Execute_Btn(fTimeDelta);
}

void CPickSlot::Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    if (m_bIsClicked) {
        BtnClick();
        m_pGameInstance->PlaySound_Once(ETOUI(SOUND_KEY::CHAR_CLICK));
    }

    if (m_bEnterHover)
        m_pGameInstance->PlaySound_Once(ETOUI(SOUND_KEY::CHAR_HOVER));
}

void CPickSlot::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CPickSlot::Render()
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

void CPickSlot::Set_IsInactive(_bool bIsInactive)
{
    m_bIsInactive = bIsInactive;
    m_pChar->Set_IsInactive(bIsInactive);
}

void CPickSlot::Set_Deselect()
{
    m_bIsSelected = false;
}

HRESULT CPickSlot::Ready_Components()
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

HRESULT CPickSlot::Bind_ShaderResources()
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

void CPickSlot::BtnClick()
{
    m_pGame_Manager->Set_SelectChar(m_eCharName);
    m_funcCallBack();
    m_bIsSelected = true;
    m_bIsClicked = false;
}

void CPickSlot::Execute_Btn(_float fTimeDelta)
{
    if (m_bIsSelected) {
        m_eCurTexState = TEX_STATE::HOVER;
        return;
    }

    switch (m_eCurBtnState) {
    case BTN_STATE::NORMAL:
    {
        m_eCurTexState = TEX_STATE::NORMAL;
        break;
    }

    case BTN_STATE::HOVER:
    {
        m_eCurTexState = TEX_STATE::HOVER;
        break;
    }

    case BTN_STATE::PRESSED:
    {
        m_eCurTexState = TEX_STATE::HOVER;
        break;
    }

    case BTN_STATE::CLICKED:
    {
        m_eCurTexState = TEX_STATE::HOVER;
        m_bIsClicked = true;
        break;
    }
    }
}

CPickSlot* CPickSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPickSlot* pInstance = new CPickSlot(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CPickSlot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPickSlot::Clone(void* pArg)
{
    CPickSlot* pInstance = new CPickSlot(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CPickSlot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPickSlot::Free()
{
    Safe_Release(m_pChar);

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    Safe_Release(m_pGame_Manager);

    __super::Free();
}
