#include "CUI_NormalSkillIcon.h"

#include "CGameInstance.h"

CUI_NormalSkillIcon::CUI_NormalSkillIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_SkillIcon{ pDevice, pContext }
{
}

CUI_NormalSkillIcon::CUI_NormalSkillIcon(const CUI_NormalSkillIcon& Prototype)
    : CUI_SkillIcon{ Prototype }
{
}

HRESULT CUI_NormalSkillIcon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_NormalSkillIcon::Initialize(void* pArg)
{
    CUI_NORMALSKILLICON_DESC* pDesc = static_cast<CUI_NORMALSKILLICON_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pCoolInfo = pDesc->pCoolInfo;

    return S_OK;
}

void CUI_NormalSkillIcon::Priority_Update(_float fTimeDelta)
{
}

void CUI_NormalSkillIcon::Parallel_Update(_float fTimeDelta)
{
}

void CUI_NormalSkillIcon::Update(_float fTimeDelta)
{
}

void CUI_NormalSkillIcon::Late_Update(_float fTimeDelta)
{

    _float fTime{};

    if (m_pCoolInfo->fAccCoolDown > 0.f && m_pCoolInfo->bCoolWait == false) {
        fTime = m_pCoolInfo->fAccCoolDown;
        m_bTextRender = true;
    }
    else
    {
        m_bTextRender = false;
    }

    if (m_bTextRender == true) {
        if (fTime >= 1.f) {
            m_wstCoolText = to_wstring(static_cast<_int>(fTime));
        }
        else {
            _tchar sztmp[10];
            swprintf_s(sztmp, 10, L"%.1f", fTime);
            m_wstCoolText = sztmp;
        }
    }


    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_NormalSkillIcon::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(ETOUI(m_eBlendState))))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;


    if (FAILED(RenderText()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_NormalSkillIcon::Ready_Components()
{
    return S_OK;
}

HRESULT CUI_NormalSkillIcon::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(CUIObject::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(CUIObject::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIdx)))
        return E_FAIL;

    m_pShaderCom->Bind_RawValue("g_FlipX", &m_iFlipX, sizeof(m_iFlipX));
    m_pShaderCom->Bind_RawValue("g_FlipY", &m_iFlipY, sizeof(m_iFlipY));
    m_pShaderCom->Bind_RawValue("g_Alpha", &m_fImageAlpha, sizeof(m_fImageAlpha));

    return S_OK;
}

CUI_NormalSkillIcon* CUI_NormalSkillIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_NormalSkillIcon* pInstance = new CUI_NormalSkillIcon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_NormalSkillIcon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_NormalSkillIcon::Clone(void* pArg)
{
    CUI_NormalSkillIcon* pInstance = new CUI_NormalSkillIcon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_NormalSkillIcon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_NormalSkillIcon::Free()
{
    __super::Free();
}
