#include "CLiDailinSlash2.h"

#include "CGameInstance.h"

#include "CBody_Player.h"

CLiDailinSlash2::CLiDailinSlash2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartEffect{ pDevice, pContext }
{

}

CLiDailinSlash2::CLiDailinSlash2(const CLiDailinSlash2& Prototype)
    : CPartEffect{ Prototype }
{

}

HRESULT CLiDailinSlash2::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLiDailinSlash2::Initialize(void* pArg)
{
    LIDAILIN_SLASH2_DESC* pDesc = static_cast<LIDAILIN_SLASH2_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(0.7f, 0.7f, 0.7f);
    m_pTransformCom->Set_Pos(XMVectorSet(0.f, 1.f, 0.f, 1.f));
    _float4 vQuat = _float4(-0.999630f, -0.007484f, 0.017719f, -0.019248f);
    m_pTransformCom->Set_Rotation(vQuat);

    m_vStartColor = COLOR_TO_FLOAT(253, 0, 2);
    m_vEndColor = COLOR_TO_FLOAT(253, 211, 3);

    return S_OK;
}

void CLiDailinSlash2::Priority_Update(_float fTimeDelta)
{
}

void CLiDailinSlash2::Parallel_Update(_float fTimeDelta)
{
}

void CLiDailinSlash2::Update(_float fTimeDelta)
{

}

void CLiDailinSlash2::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true)
        return;

    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketBoneMatrix);
    Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr())/* * SocketMatrix*/);

    m_pGameInstance->Add_RenderGroup(RENDERID::BLEND, this);
}

HRESULT CLiDailinSlash2::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pShaderCom->Begin(ETOUI(MESHEFFECT_SHADER::Q_DRAGON))))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

void CLiDailinSlash2::Set_SpinEffect(CBody_Player* pBody_Player)
{
    CMyModel* pModel = pBody_Player->Get_ModelCom();
    _uint iCurAniIndex = pModel->Get_CurAniIndex();

    if (iCurAniIndex == ETOUI(LiDailin_Ani::Ani_Q1) ||
        iCurAniIndex == ETOUI(LiDailin_Ani::Ani_Q2) ||
        iCurAniIndex == ETOUI(LiDailin_Ani::Ani_Q3))
    {
        _float fCurAniRatio = pModel->Get_AniPlayRatio(iCurAniIndex);

        _float fStartRatio{};
        _float fEndRatio{};

        if (iCurAniIndex == ETOUI(LiDailin_Ani::Ani_Q1))
        {
            m_bFlipX = false;
            fStartRatio = 0.5f;
            fEndRatio = 1.0f;
        }
        if (iCurAniIndex == ETOUI(LiDailin_Ani::Ani_Q2))
        {
            m_bFlipX = true;
            fStartRatio = 0.5f;
            fEndRatio = 1.0f;
        }
        else
        {
            m_bFlipX = false;
            fStartRatio = 0.1f;
            fEndRatio = 0.6f;
        }

        if (fCurAniRatio >= fStartRatio && fCurAniRatio <= fEndRatio)
        {
            m_bIsInactive = false;
            m_fProgressRatio = (fCurAniRatio - fStartRatio) / (fEndRatio - fStartRatio);

            _float fAlphaSpeed = 4.f;
            m_fAlpha = m_fProgressRatio * fAlphaSpeed;
            MyHelper::FloatClamp(m_fAlpha, 0.f, 1.f);

            m_fDiscardRatio = (m_fProgressRatio - (1.f / fAlphaSpeed)) / (1.f - (1.f / fAlphaSpeed));
            MyHelper::FloatClamp(m_fDiscardRatio, 0.f, 1.f);
        }

        if (Set_DragonPos == false)
        {
            Set_DragonPos = true;
            if (iCurAniIndex == ETOUI(LiDailin_Ani::Ani_Q1) ||
                iCurAniIndex == ETOUI(LiDailin_Ani::Ani_Q2))
            {
                m_pTransformCom->Set_Pos(XMVectorSet(0.f, 1.2f, 0.f, 1.f));
                _float4 vQuat = _float4(-0.999630f, -0.007484f, 0.017719f, -0.019248f);
                m_pTransformCom->Set_Rotation(vQuat);

            }
            else
            {
                m_pTransformCom->Set_Pos(XMVectorSet(0.000000f, 1.242445f, 0.000000f, 1.f));
                _float4 vQuat = _float4(0.483352f, 0.476549f, -0.522932f, 0.515571f);
                m_pTransformCom->Set_Rotation(vQuat);
            }
        }
    }
    else
    {
        if (m_bIsInactive == false)
        {
            Set_DragonPos = false;
            m_bIsInactive = true;
            m_fProgressRatio = 0.f;
            m_fDiscardRatio = 0.f;
        }
    }
}

HRESULT CLiDailinSlash2::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMeshEffect"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Component_Model_LiDailin_Slash_02",
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Texture*/
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Texture_FX_BI_Dragon01",
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    
    /* For.Com_TexNoise*/
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Texture_FX_DragonNoise",
        TEXT("Com_TexNoise"), reinterpret_cast<CComponent**>(&m_pTexNoise))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLiDailinSlash2::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iTexIdx)))
        return E_FAIL;
    if (FAILED(m_pTexNoise->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(m_fAlpha))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_DiscardRatio", &m_fDiscardRatio, sizeof(m_fDiscardRatio))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_StartColor", &m_vStartColor, sizeof(m_vStartColor))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_EndColor", &m_vEndColor, sizeof(m_vEndColor))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_FlipX", &m_bFlipX, sizeof(m_bFlipX))))
        return E_FAIL;

    return S_OK;
}

CLiDailinSlash2* CLiDailinSlash2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLiDailinSlash2* pInstance = new CLiDailinSlash2(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CLiDailinSlash2");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLiDailinSlash2::Clone(void* pArg)
{
    CLiDailinSlash2* pInstance = new CLiDailinSlash2(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CLiDailinSlash2");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLiDailinSlash2::Free()
{
    Safe_Release(m_pTexNoise);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}