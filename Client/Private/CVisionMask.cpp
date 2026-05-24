#include "CVisionMask.h"

#include "CGameInstance.h"
#include "CLiDailin.h"

CVisionMask::CVisionMask(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CVisionMask::CVisionMask(const CVisionMask& Prototype)
    : CPartObject{ Prototype }
{
}

HRESULT CVisionMask::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVisionMask::Initialize(void* pArg)
{
    VISIONMASK_DESC* pDesc = static_cast<VISIONMASK_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Rotation(XMConvertToRadians(90.f), 0.f, 0.f);
    m_pTransformCom->Set_Scale(20.f, 20.f, 1.f);

    return S_OK;
}

void CVisionMask::Priority_Update(_float fTimeDelta)
{
}

void CVisionMask::Parallel_Update(_float fTimeDelta)
{
}

void CVisionMask::Update(_float fTimeDelta)
{
}

void CVisionMask::Late_Update(_float fTimeDelta)
{
    Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    m_pGameInstance->Add_RenderGroup(RENDERID::VISION_MASK, this);
}

HRESULT CVisionMask::Render()
{
    return S_OK;
}

HRESULT CVisionMask::Render_VisionMask()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    const _uint iVisionMaskPass = 16;
    if (FAILED(m_pShaderCom->Begin(iVisionMaskPass)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CVisionMask::Set_Range(_float fRange)
{
    m_pTransformCom->Set_Scale(fRange, fRange, 1.f);
}

HRESULT CVisionMask::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CVisionMask::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CVisionMask* CVisionMask::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVisionMask* pInstance = new CVisionMask(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CVisionMask");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CVisionMask::Clone(void* pArg)
{
    CVisionMask* pInstance = new CVisionMask(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CVisionMask");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVisionMask::Free()
{
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
