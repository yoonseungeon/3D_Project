#include "CSpinWind.h"

#include "CGameInstance.h"

#include "CBody_Player.h"

CSpinWind::CSpinWind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartEffect{ pDevice, pContext }
{

}

CSpinWind::CSpinWind(const CSpinWind& Prototype)
    : CPartEffect{ Prototype }
{

}

HRESULT CSpinWind::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSpinWind::Initialize(void* pArg)
{
    SPIN_WIND_DESC* pDesc = static_cast<SPIN_WIND_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(0.7f, 0.7f, 0.7f);
    m_pTransformCom->Set_Pos(XMVectorSet(0.f, -0.55f, 0.f, 1.f));
    _float4 vQuat = _float4(-0.015443f, -0.364343f, -0.013712f, 0.931036f);
    m_pTransformCom->Set_Rotation(vQuat);

    m_vColor = COLOR_TO_FLOAT(100, 100, 100);
    m_fAlpha = 0.3f;

    m_bIsInactive = true;

    return S_OK;
}

void CSpinWind::Priority_Update(_float fTimeDelta)
{
}

void CSpinWind::Parallel_Update(_float fTimeDelta)
{
}

void CSpinWind::Update(_float fTimeDelta)
{

}

void CSpinWind::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true)
        return;

    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketBoneMatrix);
    Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    m_pGameInstance->Add_RenderGroup(RENDERID::BLEND, this);
}

HRESULT CSpinWind::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pShaderCom->Begin(ETOUI(MESHEFFECT_SHADER::WIND))))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

void CSpinWind::Set_SpinEffect(CBody_Player* pBody_Player)
{
    CMyModel* pModel = pBody_Player->Get_ModelCom();
    _uint iCurAniIndex = pModel->Get_CurAniIndex();

    if (iCurAniIndex == ETOUI(LiDailin_Ani::Ani_Q1) ||
        iCurAniIndex == ETOUI(LiDailin_Ani::Ani_Q2))
    {
        _float fCurAniRatio = pModel->Get_AniPlayRatio(iCurAniIndex);

        //_float fStartRatio = 0.346f;
        _float fStartRatio = 0.01f;
        _float fEndRatio = 0.884f;

        if (fCurAniRatio >= fStartRatio && fCurAniRatio <= fEndRatio)
        {
            m_bIsInactive = false;
            m_fProgressRatio = (fCurAniRatio - fStartRatio) / (fEndRatio - fStartRatio);
            //m_fAlpha = sinf(XMConvertToRadians(m_fProgressRatio * 180.f));
            //MyHelper::FloatClamp(m_fAlpha, 0.f, 1.f);
            //m_fAlpha *= 0.7f;
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

HRESULT CSpinWind::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMeshEffect"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Component_Model_LiDailinE_Wind4",
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Texture*/
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Texture_FX_LiDailin_Q_Spin",
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CSpinWind::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iTexIdx)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(m_fAlpha))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_vColor, sizeof(m_vColor))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_ProgressRatio", &m_fProgressRatio, sizeof(m_fProgressRatio))))
        return E_FAIL;

    return S_OK;
}

CSpinWind* CSpinWind::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSpinWind* pInstance = new CSpinWind(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CSpinWind");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSpinWind::Clone(void* pArg)
{
    CSpinWind* pInstance = new CSpinWind(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CSpinWind");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSpinWind::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}