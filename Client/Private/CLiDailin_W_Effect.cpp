#include "CLiDailin_W_Effect.h"

#include "CGameInstance.h"

#include "CBody_Player.h"

CLiDailin_W_Effect::CLiDailin_W_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartEffect{ pDevice, pContext }
{

}

CLiDailin_W_Effect::CLiDailin_W_Effect(const CLiDailin_W_Effect& Prototype)
    : CPartEffect{ Prototype }
{

}

HRESULT CLiDailin_W_Effect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLiDailin_W_Effect::Initialize(void* pArg)
{
    LIDAILIN_W_EFFECT_DESC* pDesc = static_cast<LIDAILIN_W_EFFECT_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fScale = 0.8f;
    m_pTransformCom->Set_Scale(m_fScale, m_fScale, 1.f);
    m_pTransformCom->Set_Pos(XMVectorSet(0.f, 3.3f, 0.0f, 1.f));
    //m_pTransformCom->Set_Rotation(XMConvertToRadians(90.f), 0.f, 0.f);

    return S_OK;
}

void CLiDailin_W_Effect::Priority_Update(_float fTimeDelta)
{
}

void CLiDailin_W_Effect::Parallel_Update(_float fTimeDelta)
{
}

void CLiDailin_W_Effect::Update(_float fTimeDelta)
{

}

void CLiDailin_W_Effect::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true)
        return;
    
    Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    m_pGameInstance->Add_RenderGroup(RENDERID::BLEND, this);
}

HRESULT CLiDailin_W_Effect::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iWPass = 14;
    if (FAILED(m_pShaderCom->Begin(iWPass)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CLiDailin_W_Effect::Active_Effect_W(CBody_Player* pBody_Player, _float fTimeDelta)
{
    CMyModel* pModel = pBody_Player->Get_ModelCom();
    const _uint iCurAniIndex = pModel->Get_CurOverlayAniIndex();
    _bool m_bIsOveray = pModel->IsAniOverlay();

    if (m_bIsOveray && iCurAniIndex == ETOUI(LiDailin_Ani::Ani_W1))
    {
        m_bIsInactive = false;

        
        _float fRatio = pModel->Get_AniPlayRatio(iCurAniIndex);

        // Color
        const _float fRedMaxRatio = 0.533f;
        const _float fMaxColor = 0.7f;

        _float fColor = fRatio / fRedMaxRatio;
        MyHelper::FloatClamp(fColor, 0.f, 1.f);

        _float fGBColor = fMaxColor - fColor * fMaxColor;
        MyHelper::FloatClamp(fGBColor, 0.f, 1.f);
        m_vColor = { 1.f, fGBColor, fGBColor };

        // Scale
        const _float fScaleMaxRatio = 1.f;
        const _float fMinScale = m_fScale * 0.71f;

        _float fScaleRatio = sinf(XMConvertToRadians(fRatio / fScaleMaxRatio * 90.f));
        MyHelper::FloatClamp(fScaleRatio, 0.f, 1.f);

        _float fScale = m_fScale - (m_fScale - fMinScale) * (1.f - fScaleRatio);
        m_pTransformCom->Set_Scale(fScale, fScale, 1.f);

        // Alpha
        _float fFadeStartRatio = 0.833f;
        m_fAlpha = (fRatio - fFadeStartRatio) / (1.f - fFadeStartRatio);
        MyHelper::FloatClamp(m_fAlpha, 0.f, 1.f);
        m_fAlpha = 1.f - m_fAlpha;
    }
    else
    {
        m_bIsInactive = true;
    }
}

HRESULT CLiDailin_W_Effect::Ready_Components()
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
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Texture_FX_BI_Dailin_W",
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLiDailin_W_Effect::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(m_fAlpha))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_vColor, sizeof(m_vColor))))
        return E_FAIL;

    return S_OK;
}

CLiDailin_W_Effect* CLiDailin_W_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLiDailin_W_Effect* pInstance = new CLiDailin_W_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CLiDailin_W_Effect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLiDailin_W_Effect::Clone(void* pArg)
{
    CLiDailin_W_Effect* pInstance = new CLiDailin_W_Effect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CLiDailin_W_Effect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLiDailin_W_Effect::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}