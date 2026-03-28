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

    m_fFrameDelay = pDesc->fFrameDelay;

    if (m_pTextureCom != nullptr) {
        m_iMaxTextureCnt = m_pTextureCom->Get_TextureCnt();
    }

    return S_OK;
}

void CUI_AniImage::Priority_Update(_float fTimeDelta)
{
}

void CUI_AniImage::Parallel_Update(_float fTimeDelta)
{
    // m_bIsInactived의 쓰기는 Level Update에서 일어남.(Late Update 후 LevelUpdate 됨.)
    if (m_bIsInactive == true) {
        return;
    }

    Animation(fTimeDelta);
}

void CUI_AniImage::Update(_float fTimeDelta)
{

}

void CUI_AniImage::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true) {
        return;
    }

    m_pGameInstance->Add_RenderGroup(RENDERID::UI, this);
}

HRESULT CUI_AniImage::Render()
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

HRESULT CUI_AniImage::Ready_Components()
{
    return S_OK;
}

HRESULT CUI_AniImage::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(CUIObject::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(CUIObject::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iCurTextureIdx)))
        return E_FAIL;

    m_pShaderCom->Bind_RawValue("g_FlipX", &m_iFlipX, sizeof(m_iFlipX));
    m_pShaderCom->Bind_RawValue("g_FlipY", &m_iFlipY, sizeof(m_iFlipY));
    m_pShaderCom->Bind_RawValue("g_Alpha", &m_fImageAlpha, sizeof(m_fImageAlpha));

    return S_OK;
}

void CUI_AniImage::Animation(_float fTimeDelta)
{
    m_fAccTime += fTimeDelta;
    if (m_fAccTime >= m_fFrameDelay) {
        ++m_iCurTextureIdx;
        if (m_iCurTextureIdx >= m_iMaxTextureCnt) {
            m_iCurTextureIdx = 0;
        }
        m_fAccTime -= m_fFrameDelay;
    }
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
