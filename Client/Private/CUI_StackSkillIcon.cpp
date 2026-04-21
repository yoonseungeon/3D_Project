#include "CUI_StackSkillIcon.h"

#include "CGameInstance.h"
#include "CInGame_Manager.h"
#include "CPlayer.h"

CUI_StackSkillIcon::CUI_StackSkillIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_SkillIcon{ pDevice, pContext }
{
}

CUI_StackSkillIcon::CUI_StackSkillIcon(const CUI_StackSkillIcon& Prototype)
    : CUI_SkillIcon{ Prototype }
{
}

HRESULT CUI_StackSkillIcon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_StackSkillIcon::Initialize(void* pArg)
{
    CUI_STACKSKILLICON_DESC* pDesc = static_cast<CUI_STACKSKILLICON_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CUI_StackSkillIcon::Priority_Update(_float fTimeDelta)
{
}

void CUI_StackSkillIcon::Parallel_Update(_float fTimeDelta)
{
}

void CUI_StackSkillIcon::Update(_float fTimeDelta)
{
}

void CUI_StackSkillIcon::Late_Update(_float fTimeDelta)
{
    STACK_COOL_INFO* pStackCoolInfo = static_cast<STACK_COOL_INFO*>(m_pInGameManager->Get_Player()->Get_CoolInfo(m_eSkillSlot));

    _float fTime{};

    if (pStackCoolInfo->fAccCoolDown > 0.f) {
        fTime = pStackCoolInfo->fAccCoolDown;
        m_bTextRender = true;
    }
    else if (pStackCoolInfo->fAccSubCoolDown > 0.f) {
        fTime = pStackCoolInfo->fAccSubCoolDown;
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

HRESULT CUI_StackSkillIcon::Render()
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

HRESULT CUI_StackSkillIcon::Ready_Components()
{
    return S_OK;
}

HRESULT CUI_StackSkillIcon::Bind_ShaderResources()
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

CUI_StackSkillIcon* CUI_StackSkillIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_StackSkillIcon* pInstance = new CUI_StackSkillIcon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_StackSkillIcon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_StackSkillIcon::Clone(void* pArg)
{
    CUI_StackSkillIcon* pInstance = new CUI_StackSkillIcon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_StackSkillIcon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_StackSkillIcon::Free()
{
    __super::Free();
}
