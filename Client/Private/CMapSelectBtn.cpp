#include "CMapSelectBtn.h"

#include "CGameInstance.h"
#include "CUI_TextBox.h"

#include "CGame_Manager.h"

CMapSelectBtn::CMapSelectBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Btn{ pDevice, pContext }
{
}

CMapSelectBtn::CMapSelectBtn(const CMapSelectBtn& Prototype)
    : CUI_Btn{ Prototype }
{

}

HRESULT CMapSelectBtn::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMapSelectBtn::Initialize(void* pArg)
{
    m_pGame_Manager = CGame_Manager::GetInstance();
    Safe_AddRef(m_pGame_Manager);

    CMAPSELECTBTN_DESC* pDesc = static_cast<CMAPSELECTBTN_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;  

    m_eMapName = pDesc->eMapName;

    // Select Image
    CUI_Image::CUI_IMAGE_DESC Desc{};

    Desc.fScaleRatioX = pDesc->fScaleRatioX;
    Desc.fScaleRatioY = pDesc->fScaleRatioY;
    Desc.fPosRatioX = pDesc->fPosRatioX;
    Desc.fPosRatioY = pDesc->fPosRatioY;
    Desc.iUILayer = ETOUI(UILAYER::BUTTON_IMAGE);

    Desc.eTexPrototypeLV = LEVEL::LOBBY;
    Desc.eBlendState = CUI_Default::ALPHABLEND;
    Desc.wstrTexturePrototypeTag = MAPS[static_cast<_uint>(m_eMapName)].TEX_SELECT_TAG;
   
    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_Image"),
        ETOUI(LEVEL::LOBBY), TEXT("Layer_UI_Image"), &Desc, reinterpret_cast<CGameObject**>(&m_pSelectImage))))
        return E_FAIL;

    m_pSelectImage->Set_IsInactive(true);

    // Name Panel
    CUI_TextBox::CUI_TEXTBOX_DESC TextBoxDesc{};

    TextBoxDesc.fScaleRatioX = 0.016f + (static_cast<_float>(wcslen(MAPS[static_cast<_uint>(m_eMapName)].KR_MAP_NAME)) - 1.f) * 0.0092f;
    TextBoxDesc.fScaleRatioY = 0.03f; 
    TextBoxDesc.fPosRatioX = pDesc->fPosRatioX;
    TextBoxDesc.fPosRatioY = pDesc->fPosRatioY + 0.02f;
    TextBoxDesc.iUILayer = ETOUI(UILAYER::BUTTON_IMAGE_OVER);

    TextBoxDesc.eTexPrototypeLV = LEVEL::LOBBY;
    TextBoxDesc.eBlendState = CUI_Default::ALPHABLEND;
    TextBoxDesc.wstrTexturePrototypeTag = L"Prototype_Texture_BlackBlock";
    TextBoxDesc.fImageAlpha = 0.4f;
    TextBoxDesc.wstrText = MAPS[static_cast<_uint>(m_eMapName)].KR_MAP_NAME;

    TextBoxDesc.fOffsetX = 2.f;
    TextBoxDesc.fOffsetY = 2.5f;

    TextBoxDesc.fTextureSize = fDefaultFontSize * 0.5f;

    if (m_eMapName == MAP_NAME::UPTOWN) {
        TextBoxDesc.fScaleRatioX -= 0.005f;
    }

    if (FAILED(m_pGameInstance->Add_GameObject(ETOUI(LEVEL::STATIC), TEXT("Prototype_GameObject_CUI_TextBox"),
        ETOUI(LEVEL::LOBBY), TEXT("Layer_UI_Image"), &TextBoxDesc, reinterpret_cast<CGameObject**>(&m_pNameBox))))
        return E_FAIL;

    m_pNameBox->Set_IsInactive(true);


    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CMapSelectBtn::Priority_Update(_float fTimeDelta)
{
}

void CMapSelectBtn::Parallel_Update(_float fTimeDelta)
{
    // m_bIsInactived의 쓰기는 Level Update에서 일어남.(Late Update 후 LevelUpdate 됨.)
    if (m_bIsInactive == true) {
        return;
    }

    Update_BtnState();

    Execute_Btn(fTimeDelta);
}

void CMapSelectBtn::Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    if (m_bIsClicked) {
        BtnClick();
    }
}

void CMapSelectBtn::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true || m_eCurTexState == TEX_STATE::NONE) {
        return;
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CMapSelectBtn::Render()
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

void CMapSelectBtn::Set_Deselect()
{
    m_bIsSelected = false;
    m_pSelectImage->Set_IsInactive(true);
}

void CMapSelectBtn::Set_IsInactive(_bool bIsInactive)
{
    m_bIsInactive = bIsInactive;
    m_pNameBox->Set_IsInactive(bIsInactive);
}

HRESULT CMapSelectBtn::Ready_Components()
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

    /* For.Com_Texture*/
    if (FAILED(__super::Add_Component(ETOUI(m_eTexPrototypeLV), MAPS[static_cast<_uint>(m_eMapName)].IMAGE_OVER_TAG,
        TEXT("Com_Image"), reinterpret_cast<CComponent**>(&m_pImageCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapSelectBtn::Bind_ShaderResources()
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

void CMapSelectBtn::Update_BtnState()
{
    POINT ptMouse = m_pGameInstance->Get_MouseClientPos();
    const _float fImageLocalPosX = static_cast<const _float>(ptMouse.x - m_rcBtnRange.left);
    const _float fImageLocalPosY = static_cast<const _float>(ptMouse.y - m_rcBtnRange.top);

    const _float3 vScale = m_pTransformCom->Get_Scaled();

    const _float fU = fImageLocalPosX / vScale.x;
    const _float fV = fImageLocalPosY / vScale.y;

    if (m_pImageCom->AlphaClick(fU, fV, 5)) {
        if (m_bPressedInBtn && m_pGameInstance->Mouse_Up(DIMB::LBUTTON))
        {
            m_bPressedInBtn = false;
            m_eCurBtnState = CUI_Btn::CLICKED;
            return;
        }

        if (m_bPressedInBtn || m_pGameInstance->Mouse_Down(DIMB::LBUTTON))
        {
            m_bPressedInBtn = true;
            m_eCurBtnState = CUI_Btn::PRESSED;
            return;
        }

        m_eCurBtnState = CUI_Btn::HOVER;
        return;
    }

    if (m_bPressedInBtn)
    {
        m_bPressedInBtn = false;
    }
    m_eCurBtnState = CUI_Btn::NORMAL;
}

void CMapSelectBtn::BtnClick()
{
    m_funcCallBack();
    m_bIsSelected = true;
    m_pSelectImage->Set_IsInactive(false);
    m_pGame_Manager->Set_SelectSpawnMap(m_eMapName);
    m_bIsClicked = false;
}

void CMapSelectBtn::Execute_Btn(_float fTimeDelta)
{
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

CMapSelectBtn* CMapSelectBtn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMapSelectBtn* pInstance = new CMapSelectBtn(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CMapSelectBtn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMapSelectBtn::Clone(void* pArg)
{
    CMapSelectBtn* pInstance = new CMapSelectBtn(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CMapSelectBtn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapSelectBtn::Free()
{    
    Safe_Release(m_pSelectImage);
    Safe_Release(m_pNameBox);

    Safe_Release(m_pImageCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    Safe_Release(m_pGame_Manager);

    __super::Free();
}
