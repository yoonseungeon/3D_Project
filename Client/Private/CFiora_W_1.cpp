#include "CFiora_W_1.h"

#include "CGameInstance.h"

#include "CBody_Player.h"

CFiora_W_1::CFiora_W_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartEffect{ pDevice, pContext }
{

}

CFiora_W_1::CFiora_W_1(const CFiora_W_1& Prototype)
    : CPartEffect{ Prototype }
{

}

HRESULT CFiora_W_1::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFiora_W_1::Initialize(void* pArg)
{
    FIORA_W_1_DESC* pDesc = static_cast<FIORA_W_1_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_vColor[0] = COLOR_TO_FLOAT(121, 163, 222);
    m_vColor[1] = COLOR_TO_FLOAT(124, 217, 224);

    m_fAlpha[0] = { 1.0f };
    m_fAlpha[1] = { 0.1f };

    m_iBlendState[0] = ETOUI(MESHEFFECT_SHADER::COLOR_ADD);
    m_iBlendState[1] = ETOUI(MESHEFFECT_SHADER::COLOR_ADD);

    m_pTransformCom->Set_Pos(XMVectorSet(0.f, 0.21f, 0.f, 1.f));
    m_pTransformCom->Set_Scale(5.f, 5.f, 5.f);

    m_bIsInactive = true;

    return S_OK;
}

void CFiora_W_1::Priority_Update(_float fTimeDelta)
{
}

void CFiora_W_1::Parallel_Update(_float fTimeDelta)
{
}

void CFiora_W_1::Update(_float fTimeDelta)
{

}

void CFiora_W_1::Late_Update(_float fTimeDelta)
{
    if (m_bIsInactive == true)
        return;

    Compute_CombinedWorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    m_pGameInstance->Add_RenderGroup(RENDERID::BLEND, this);
}

HRESULT CFiora_W_1::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    for (_uint i = 0; i < sizeof(m_pModelCom) / sizeof(m_pModelCom[0]); ++i)
    {
        if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_vColor[i], sizeof(_float3))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha[i], sizeof(_float))))
            return E_FAIL;

        _uint iNumMeshes = m_pModelCom[i]->Get_NumMeshes();

        for (_uint j = 0; j < iNumMeshes; ++j)
        {
            if (FAILED(m_pShaderCom->Begin(m_iBlendState[i])))
                return E_FAIL;

            if (FAILED(m_pModelCom[i]->Render(j)))
                return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CFiora_W_1::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMeshEffect"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Component_Model_FX_BI_Fiora_Skill02_Range1_S004",
        TEXT("Com_Model1"), reinterpret_cast<CComponent**>(&m_pModelCom[0]))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), L"Prototype_Component_Model_FX_BI_Fiora_Skill02_Range2_S004",
        TEXT("Com_Model2"), reinterpret_cast<CComponent**>(&m_pModelCom[1]))))
        return E_FAIL;

    return S_OK;
}

HRESULT CFiora_W_1::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CFiora_W_1* CFiora_W_1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFiora_W_1* pInstance = new CFiora_W_1(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CFiora_W_1");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFiora_W_1::Clone(void* pArg)
{
    CFiora_W_1* pInstance = new CFiora_W_1(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CFiora_W_1");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFiora_W_1::Free()
{
    for (_uint i = 0; i < sizeof(m_pModelCom) / sizeof(m_pModelCom[0]); ++i)
        Safe_Release(m_pModelCom[i]);

    Safe_Release(m_pShaderCom);

    __super::Free();
}