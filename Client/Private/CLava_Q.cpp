#include "CLava_Q.h"

#include "CGameInstance.h"

#include "CBody_Player.h"

CLava_Q::CLava_Q(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartEffect{ pDevice, pContext }
{

}

CLava_Q::CLava_Q(const CLava_Q& Prototype)
    : CPartEffect{ Prototype }
{

}

HRESULT CLava_Q::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLava_Q::Initialize(void* pArg)
{
    LAVA_Q_DESC* pDesc = static_cast<LAVA_Q_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(4.f, 4.f, 1.f);
    m_pTransformCom->Set_Pos(XMVectorSet(0.f, 0.01f, 0.7f, 1.f));
    m_pTransformCom->Set_Rotation(XMConvertToRadians(90.f), 0.f, 0.f);

    return S_OK;
}

void CLava_Q::Priority_Update(_float fTimeDelta)
{
}

void CLava_Q::Parallel_Update(_float fTimeDelta)
{
}

void CLava_Q::Update(_float fTimeDelta)
{

}

void CLava_Q::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true)
        return;

    m_pGameInstance->Add_RenderGroup(RENDERID::BLEND, this);
}

HRESULT CLava_Q::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iLavaPass = 12;
    if (FAILED(m_pShaderCom->Begin(iLavaPass)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CLava_Q::Set_SpinEffect(CBody_Player* pBody_Player, _float fTimeDelta)
{
    CMyModel* pModel = pBody_Player->Get_ModelCom();
    _uint iCurAniIndex = pModel->Get_CurAniIndex();

    if (iCurAniIndex == ETOUI(LiDailin_Ani::Ani_Q3))
    {
        _float fCurAniRatio = pModel->Get_AniPlayRatio(iCurAniIndex);

        const _float fAppearStartRatio = 0.283f;
        const _float fAppearEndRatio = 0.683f;

        if (fCurAniRatio >= fAppearStartRatio)
        {
            if (m_bSetPos == false)
            {
                m_bSetPos = true;
                Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
            }

            m_bIsInactive = false;
        }
    }
    else
    {
        if (m_bIsInactive == false)
        {
            m_fAlpha -= fTimeDelta;
            if (m_fAlpha <= 0.f)
            {
                m_bIsInactive = true;
                m_fAlpha = 1.f;
                m_bSetPos = false;
            }
        }
    }
}

HRESULT CLava_Q::Ready_Components()
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
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Texture_Lava",
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLava_Q::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIdx)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(m_fAlpha))))
        return E_FAIL;

    return S_OK;
}

CLava_Q* CLava_Q::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLava_Q* pInstance = new CLava_Q(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CLava_Q");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLava_Q::Clone(void* pArg)
{
    CLava_Q* pInstance = new CLava_Q(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CLava_Q");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLava_Q::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}