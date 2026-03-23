#include "CUI_AniImage.h"

#include "CGameInstance.h"

CUI_AniImage::CUI_AniImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Image{ pDevice, pContext }
{

}

CUI_AniImage::CUI_AniImage(const CUI_AniImage& Prototype)
    : CUI_Image{ Prototype }
{

}

HRESULT CUI_AniImage::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_AniImage::Initialize(void* pArg)
{
    CUI_ANIIMAGE_DESC* pDesc = static_cast<CUI_ANIIMAGE_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CUI_AniImage::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CUI_AniImage::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CUI_AniImage::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CUI_AniImage::Render()
{
    if(FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_AniImage::Ready_Components()
{
    __super::Ready_Components();

    return S_OK;
}

HRESULT CUI_AniImage::Bind_ShaderResources()
{
    __super::Bind_ShaderResources();

    return S_OK;
}

CUI_AniImage* CUI_AniImage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_AniImage* pInstance = new CUI_AniImage(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CUI_AniImage");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_AniImage::Clone(void* pArg)
{
    CUI_AniImage* pInstance = new CUI_AniImage(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CUI_AniImage");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_AniImage::Free()
{
    __super::Free();
}
