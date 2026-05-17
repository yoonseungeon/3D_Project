#include "CShockWave_Q.h"

#include "CGameInstance.h"

#include "CBody_Player.h"

CShockWave_Q::CShockWave_Q(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartEffect{ pDevice, pContext }
{

}

CShockWave_Q::CShockWave_Q(const CShockWave_Q& Prototype)
    : CPartEffect{ Prototype }
{

}

HRESULT CShockWave_Q::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CShockWave_Q::Initialize(void* pArg)
{
    SHOKEWAVE_Q_DESC* pDesc = static_cast<SHOKEWAVE_Q_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(3.f, 3.f, 1.f);
    m_pTransformCom->Set_Pos(XMVectorSet(0.f, 0.f, 0.7f, 1.f));
    m_pTransformCom->Set_Rotation(XMConvertToRadians(90.f), 0.f, 0.f);

    return S_OK;
}

void CShockWave_Q::Priority_Update(_float fTimeDelta)
{
    //__super::Priority_Update(fTimeDelta);
}

void CShockWave_Q::Parallel_Update(_float fTimeDelta)
{
}

void CShockWave_Q::Update(_float fTimeDelta)
{

}

void CShockWave_Q::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true)
        return;

    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketBoneMatrix);
    Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr())/* * SocketMatrix*/);

    m_pGameInstance->Add_RenderGroup(RENDERID::BLEND, this);
}

HRESULT CShockWave_Q::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iShockWavePass = 11;
    if (FAILED(m_pShaderCom->Begin(iShockWavePass)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CShockWave_Q::Set_SpinEffect(CBody_Player* pBody_Player)
{
    CMyModel* pModel = pBody_Player->Get_ModelCom();
    _uint iCurAniIndex = pModel->Get_CurAniIndex();

    if (iCurAniIndex == ETOUI(LiDailin_Ani::Ani_Q1) || iCurAniIndex == ETOUI(LiDailin_Ani::Ani_Q2))
    {
        _float fCurAniRatio = pModel->Get_AniPlayRatio(iCurAniIndex);

        //_float fStartRatio = 0.346f;
        _float fStartRatio = 0.01f;
        _float fEndRatio = 0.884f;

        if (fCurAniRatio >= fStartRatio && fCurAniRatio <= fEndRatio)
        {
            m_bIsInactive = false;
            m_fProgressRatio = (fCurAniRatio - fStartRatio) / (fEndRatio - fStartRatio);
            m_fAlpha = sinf(XMConvertToRadians(m_fProgressRatio * 180.f));
            MyHelper::FloatClamp(m_fAlpha, 0.f, 1.f);
            m_fAlpha *= 0.7f;
        }
    }
    else
    {
        if (m_bIsInactive == false)
        {
            m_bIsInactive = true;
            m_fProgressRatio = 0.f;
        }
    }
}

HRESULT CShockWave_Q::Ready_Components()
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
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Texture_ShockWave",
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CShockWave_Q::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIdx)))
        return E_FAIL;

    //if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(m_fAlpha))))
    //    return E_FAIL;
    //if (FAILED(m_pShaderCom->Bind_RawValue("g_ProgressRatio", &m_fProgressRatio, sizeof(m_fProgressRatio))))
    //    return E_FAIL;

    return S_OK;
}

CShockWave_Q* CShockWave_Q::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CShockWave_Q* pInstance = new CShockWave_Q(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CShockWave_Q");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CShockWave_Q::Clone(void* pArg)
{
    CShockWave_Q* pInstance = new CShockWave_Q(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CShockWave_Q");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CShockWave_Q::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}