#include "CUI_FadeImage.h"

#include "CGameInstance.h"

CUI_FadeImage::CUI_FadeImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Image{ pDevice, pContext }
{

}

CUI_FadeImage::CUI_FadeImage(const CUI_FadeImage& Prototype)
    : CUI_Image{ Prototype }
{

}

HRESULT CUI_FadeImage::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_FadeImage::Initialize(void* pArg)
{
    CUI_FADEIMAGE_DESC* pDesc = static_cast<CUI_FADEIMAGE_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CUI_FadeImage::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CUI_FadeImage::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CUI_FadeImage::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CUI_FadeImage::Render()
{
    if(FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CUI_FadeImage::Add_Alpha(_float fAlpha)
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

HRESULT CUI_FadeImage::Ready_Components()
{
    __super::Ready_Components();

    return S_OK;
}

HRESULT CUI_FadeImage::Bind_ShaderResources()
{
    __super::Bind_ShaderResources();

    return S_OK;
}

CUI_FadeImage* CUI_FadeImage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_FadeImage* pInstance = new CUI_FadeImage(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_FadeImage");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_FadeImage::Clone(void* pArg)
{
    CUI_FadeImage* pInstance = new CUI_FadeImage(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_FadeImage");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_FadeImage::Free()
{
    __super::Free();
}
