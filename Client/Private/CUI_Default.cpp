#include "CUI_Default.h"

CUI_Default::CUI_Default(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{

}

CUI_Default::CUI_Default(const CUI_Default& Prototype)
    : CUIObject{ Prototype }
{

}

HRESULT CUI_Default::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Default::Initialize(void* pArg)
{
    CUI_DEFAULT_DESC* pDesc = static_cast<CUI_DEFAULT_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_eTexPrototypeLV = pDesc->eTexPrototypeLV;
    m_wstrTexturePrototypeTag = pDesc->wstrTexturePrototypeTag;
    m_eBlendState = pDesc->eBlendState;

    m_fImageAlpha = pDesc->fImageAlpha;

    return S_OK;
}

void CUI_Default::Priority_Update(_float fTimeDelta)
{

}

void CUI_Default::Parallel_Update(_float fTimeDelta)
{
}

void CUI_Default::Update(_float fTimeDelta)
{

}

void CUI_Default::Late_Update(_float fTimeDelta)
{
 
}

HRESULT CUI_Default::Render()
{
    return S_OK;
}

void CUI_Default::Add_Alpha(_float fAlpha)
{
    m_fImageAlpha += fAlpha;
    if (m_fImageAlpha > 1.f)
    {
        m_fImageAlpha = 1.f;
    }
    else if (m_fImageAlpha < 0.f)
    {
        m_fImageAlpha = 0.f;
    }
}

void CUI_Default::Free()
{
    __super::Free();
}
