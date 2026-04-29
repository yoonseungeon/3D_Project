#include "CUI_InGameCharProfile.h"

#include "CGameInstance.h"
#include "CCharData_Manager.h"
#include "CGame_Manager.h"

CUI_InGameCharProfile::CUI_InGameCharProfile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Image{ pDevice, pContext }
{
}

CUI_InGameCharProfile::CUI_InGameCharProfile(const CUI_InGameCharProfile& Prototype)
    : CUI_Image{ Prototype }
{
}

HRESULT CUI_InGameCharProfile::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_InGameCharProfile::Initialize(void* pArg)
{
    CUI_INGAMECHARPROFILE_DESC* pDesc = static_cast<CUI_INGAMECHARPROFILE_DESC*>(pArg);

    CHAR_NAME eCharName = CGame_Manager::GetInstance()->Get_SelectedChar();
    pDesc->iTexIdx = CGame_Manager::GetInstance()->Get_SkinIndex();

    // enter로 스킵하면 일단 용다이린
    if (eCharName == CHAR_NAME::CHARNAME_END) {
        eCharName = CHAR_NAME::LIDAILIN;
        pDesc->iTexIdx = 1;
    }

    const auto pCharInfo = CCharData_Manager::GetInstance()->Get_CharInfo(eCharName);

    pDesc->eTexPrototypeLV = LEVEL::GAMEPLAY;
    pDesc->wstrTexturePrototypeTag = pCharInfo->wstrProfileTag;
    pDesc->eBlendState = BLENDSTATE::CHARPROFILE;

    m_vClipRatioY = { 0.1f, 0.9f };

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;


    return S_OK;
}

void CUI_InGameCharProfile::Priority_Update(_float fTimeDelta)
{
}

void CUI_InGameCharProfile::Parallel_Update(_float fTimeDelta)
{

}

void CUI_InGameCharProfile::Update(_float fTimeDelta)
{

}

void CUI_InGameCharProfile::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_InGameCharProfile::Render()
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

HRESULT CUI_InGameCharProfile::Ready_Components()
{
    /* For.Com_Mask*/
    if (FAILED(__super::Add_Component(ETOUI(m_eTexPrototypeLV), L"Prototype_Texture_CircleMask",
        TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pMaskCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_InGameCharProfile::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(CUIObject::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(CUIObject::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIdx)))
        return E_FAIL;

    if (FAILED(m_pMaskCom->Bind_ShaderResource(m_pShaderCom, "g_Mask", 0)))
        return E_FAIL;

    m_pShaderCom->Bind_RawValue("g_ClipYRatio", &m_vClipRatioY, sizeof(m_vClipRatioY));

    return S_OK;
}

CUI_InGameCharProfile* CUI_InGameCharProfile::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_InGameCharProfile* pInstance = new CUI_InGameCharProfile(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_InGameCharProfile");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_InGameCharProfile::Clone(void* pArg)
{
    CUI_InGameCharProfile* pInstance = new CUI_InGameCharProfile(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_InGameCharProfile");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_InGameCharProfile::Free()
{
    Safe_Release(m_pMaskCom);

    __super::Free();
}
