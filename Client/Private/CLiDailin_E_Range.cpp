#include "CLiDailin_E_Range.h"

#include "CGameInstance.h"

#include "CBody_Player.h"

CLiDailin_E_Range::CLiDailin_E_Range(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartEffect{ pDevice, pContext }
{

}

CLiDailin_E_Range::CLiDailin_E_Range(const CLiDailin_E_Range& Prototype)
    : CPartEffect{ Prototype }
{

}

HRESULT CLiDailin_E_Range::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLiDailin_E_Range::Initialize(void* pArg)
{
    LIDAILIN_E_RANGE_DESC* pDesc = static_cast<LIDAILIN_E_RANGE_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(0.7f, 0.7f, 0.7f);
    m_pTransformCom->Set_Rotation(0.f, XMConvertToRadians(180.f), 0.f);
    m_vColor = COLOR_TO_FLOAT(207, 164, 72);

    m_fMaxTime = 0.6f;

    m_bIsInactive = true;

    return S_OK;
}

void CLiDailin_E_Range::Priority_Update(_float fTimeDelta)
{
}

void CLiDailin_E_Range::Parallel_Update(_float fTimeDelta)
{
}

void CLiDailin_E_Range::Update(_float fTimeDelta)
{

}

void CLiDailin_E_Range::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true)
        return;

    m_pGameInstance->Add_RenderGroup(RENDERID::BLEND, this);
}

HRESULT CLiDailin_E_Range::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pShaderCom->Begin(ETOUI(MESHEFFECT_SHADER::CONVERT_ALPHA))))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

void CLiDailin_E_Range::Set_E_Range(CBody_Player* pBody_Player, _float fTimeDelta)
{
    CMyModel* pModel = pBody_Player->Get_ModelCom();
    _uint iCurAniIndex = pModel->Get_CurAniIndex();

    _float fCurAniRatio = pModel->Get_AniPlayRatio(iCurAniIndex);
    const _float fStartRatio = 0.166f;

    if (iCurAniIndex == ETOUI(LiDailin_Ani::Ani_E) && fCurAniRatio < fStartRatio)
    {
        m_bIsActiveOnce = false;
    }

    if (iCurAniIndex == ETOUI(LiDailin_Ani::Ani_E) && m_bIsActiveOnce == false)
    {
        if (fCurAniRatio >= fStartRatio && m_bIsInactive == true)
        {
            Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
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
        m_fAlpha = 2.f * (1.f - m_fProgressRatio);
        MyHelper::FloatClamp(m_fAlpha, 0.f, 1.f);  
    }
}

HRESULT CLiDailin_E_Range::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMeshEffect"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Component_Model_LiDailin_E_Range",
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Texture*/
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Texture_LiDailin_E_Tex",
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLiDailin_E_Range::Bind_ShaderResources()
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

    return S_OK;
}

CLiDailin_E_Range* CLiDailin_E_Range::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLiDailin_E_Range* pInstance = new CLiDailin_E_Range(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CLiDailin_E_Range");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLiDailin_E_Range::Clone(void* pArg)
{
    CLiDailin_E_Range* pInstance = new CLiDailin_E_Range(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CLiDailin_E_Range");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLiDailin_E_Range::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}