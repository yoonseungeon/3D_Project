#include "CDragon_R.h"

#include "CGameInstance.h"

#include "CBody_Player.h"

CDragon_R::CDragon_R(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartEffect{ pDevice, pContext }
{

}

CDragon_R::CDragon_R(const CDragon_R& Prototype)
    : CPartEffect{ Prototype }
{

}

HRESULT CDragon_R::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDragon_R::Initialize(void* pArg)
{
    DRAGON_R_DESC* pDesc = static_cast<DRAGON_R_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(8.f, 2.5f, 1.f);
    m_pTransformCom->Set_Pos(XMVectorSet(0.f, 1.f, -3.f, 1.f));
    m_pTransformCom->Set_Rotation(XMConvertToRadians(90.f), XMConvertToRadians(-90.f), 0.f);

    m_bIsInactive = true;

    m_fMaxTime = 0.4f;

    return S_OK;
}

void CDragon_R::Priority_Update(_float fTimeDelta)
{
}

void CDragon_R::Parallel_Update(_float fTimeDelta)
{
}

void CDragon_R::Update(_float fTimeDelta)
{

}

void CDragon_R::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true)
        return;

    Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    m_pGameInstance->Add_RenderGroup(RENDERID::BLEND, this);
}

HRESULT CDragon_R::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iDragonRPass = 13;
    if (FAILED(m_pShaderCom->Begin(iDragonRPass)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CDragon_R::Active_Dragon_R(CBody_Player* pBody_Player, _float fTimeDelta)
{
    CMyModel* pModel = pBody_Player->Get_ModelCom();
    _uint iCurAniIndex = pModel->Get_CurAniIndex();

    _float fCurAniRatio = pModel->Get_AniPlayRatio(iCurAniIndex);
    const _float fStartRatio = 0.166f;

    if (iCurAniIndex == ETOUI(LiDailin_Ani::Ani_R1) && fCurAniRatio < fStartRatio)
    {
        m_bIsActiveOnce = false;
    }

    if (iCurAniIndex == ETOUI(LiDailin_Ani::Ani_R1) && m_bIsActiveOnce == false)
    {
        if (fCurAniRatio >= fStartRatio && m_bIsInactive == true)
        {
            m_bIsActiveOnce = true;
            m_bIsInactive = false;
            m_fAccTime = m_fMaxTime;
        }
    }

    if (m_bIsInactive == false)
    {
        m_fAccTime -= fTimeDelta;
        if (m_fAccTime <= 0.f)
        {
            m_bIsInactive = true;
            m_fAccTime = 0.f;
            m_fProgressRatio = 1.f;
            return;
        }
        m_fProgressRatio = (m_fMaxTime - m_fAccTime) / m_fMaxTime;
        //m_fAlpha = 2.f * (1.f - m_fProgressRatio);
        //MyHelper::FloatClamp(m_fAlpha, 0.f, 1.f);
    }
}

HRESULT CDragon_R::Ready_Components()
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
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Texture_FX_BI_Dragon01",
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* For.Com_Mask*/
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Texture_FXt_Noise_03",
        TEXT("Com_MaskColor"), reinterpret_cast<CComponent**>(&m_pMaskComColor))))
        return E_FAIL;

    /* For.Com_Mask*/
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Texture_FXt_Noise_11",
        TEXT("Com_MaskDisappear"), reinterpret_cast<CComponent**>(&m_pMaskComDisapper))))
        return E_FAIL;

    return S_OK;
}

HRESULT CDragon_R::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;
    if (FAILED(m_pMaskComColor->Bind_ShaderResource(m_pShaderCom, "g_MaskColor", 0)))
        return E_FAIL;
    if (FAILED(m_pMaskComDisapper->Bind_ShaderResource(m_pShaderCom, "g_MaskDisappear", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(m_fAlpha))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_ProgressRatio", &m_fProgressRatio, sizeof(m_fProgressRatio))))
        return E_FAIL;

    return S_OK;
}

CDragon_R* CDragon_R::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDragon_R* pInstance = new CDragon_R(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CDragon_R");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDragon_R::Clone(void* pArg)
{
    CDragon_R* pInstance = new CDragon_R(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CDragon_R");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDragon_R::Free()
{
    Safe_Release(m_pMaskComColor);
    Safe_Release(m_pMaskComDisapper);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}