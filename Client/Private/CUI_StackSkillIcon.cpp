#include "CUI_StackSkillIcon.h"

#include "CGameInstance.h"

CUI_StackSkillIcon::CUI_StackSkillIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Image{ pDevice, pContext }
{
}

CUI_StackSkillIcon::CUI_StackSkillIcon(const CUI_StackSkillIcon& Prototype)
    : CUI_Image{ Prototype }
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

    m_pStackCoolInfo = pDesc->pStackCoolInfo;

    _float fCenterX = (pDesc->fPosRatioX + 0.5f) * static_cast<_float>(g_iWinSizeX);
    _float fCenterY = -(pDesc->fPosRatioY - 0.5f) * static_cast<_float>(g_iWinSizeY);

    m_fHalfSizeX = pDesc->fScaleRatioX * static_cast<_float>(g_iWinSizeX) * 0.5f;
    m_fHalfSizeY = pDesc->fScaleRatioY * static_cast<_float>(g_iWinSizeY) * 0.5f;

    m_rcTextRange.left = static_cast<_long>(fCenterX - m_fHalfSizeX);
    m_rcTextRange.right = static_cast<_long>(fCenterX + m_fHalfSizeX);
    m_rcTextRange.top = static_cast<_long>(fCenterY - m_fHalfSizeY);
    m_rcTextRange.bottom = static_cast<_long>(fCenterY + m_fHalfSizeY);

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

    _float fTime{};

    if (m_pStackCoolInfo->fAccCoolDown > 0.f) {
        fTime = m_pStackCoolInfo->fAccCoolDown;
        m_bTextRender = true;
    }
    else if (m_pStackCoolInfo->fAccSubCoolDown > 0.f) {
        fTime = m_pStackCoolInfo->fAccSubCoolDown;
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

    const _float fTextStartX = static_cast<_float>(m_rcTextRange.left);
    const _float fTextStartY = static_cast<_float>(m_rcTextRange.top);

    if (m_bTextRender) {
        m_pGameInstance->Draw_Text(TEXT("Font_Pretendard_Middle"),
            m_wstCoolText.data(),
            _float2(fTextStartX, fTextStartY),
            XMVectorSet(1.f, 1.f, 1.f, 1.f),
            _float2(fDefaultFontSize * 0.8f, fDefaultFontSize * 0.8f)
        );
    }


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
