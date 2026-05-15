#include "CTrailEffect.h"

#include "CGameInstance.h"

CTrailEffect::CTrailEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CTrailEffect::CTrailEffect(const CTrailEffect& Prototype)
    : CGameObject{ Prototype }
{

}

HRESULT CTrailEffect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTrailEffect::Initialize(void* pArg)
{
    TRAIL_EFFECT_DESC* pDesc = static_cast<TRAIL_EFFECT_DESC*>(pArg);

    m_wstrTexturePrototypeTag = pDesc->wstrTexturePrototypeTag;
    m_iTexIndex = pDesc->iTexIndex;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CTrailEffect::Priority_Update(_float fTimeDelta)
{
}

void CTrailEffect::Parallel_Update(_float fTimeDelta)
{
}

void CTrailEffect::Update(_float fTimeDelta)
{
    Check_LifeTime(fTimeDelta);
    Make_Vertices(fTimeDelta);
}

void CTrailEffect::Late_Update(_float fTimeDelta)
{
    if (m_fAccLifeTime <= 0.f)
        return;

    m_pGameInstance->Add_RenderGroup(RENDERID::NONLIGHT, this);
}

HRESULT CTrailEffect::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(ETOUI(TRAIL_SHADER::ALPHABLEND))))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CTrailEffect::Add_TrailPoint(const _float3& vTopWorldPos, const _float3& vBottomWorldPos, const _float& fProgressRatio)
{
    TRAIL_POINT_INFO tTrailPointInfo{ vTopWorldPos , vBottomWorldPos, fProgressRatio };
    m_TrailPointsInfo.push_back(tTrailPointInfo);

    m_fAccLifeTime = m_fMaxLifeTime;
    m_fAccAlpha = m_fMaxAlpha;
}

void CTrailEffect::End_Trail()
{
    m_TrailPointsInfo.clear();
    m_Vertices.clear();
    m_fAccLifeTime = 0.f;
}

HRESULT CTrailEffect::Ready_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxTexTrail"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Trail"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* For.Com_Texture*/
    if (FAILED(__super::Add_Component(ETOUI(LEVEL::GAMEPLAY), m_wstrTexturePrototypeTag,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CTrailEffect::Bind_ShaderResources()
{
    //if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
    //    return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTexIndex)))
        return E_FAIL;

    return S_OK;
}

void CTrailEffect::Check_LifeTime(_float fTimeDelta)
{
    if (m_fAccLifeTime > 0.f)
    {
        m_fAccLifeTime -= fTimeDelta;
        if (m_fAccLifeTime <= 0.f)
        {
            m_fAccLifeTime = 0.f;
            End_Trail();
        }
    }
 }

void CTrailEffect::Make_Vertices(_float fTimeDelta)
{
    if (m_fAccLifeTime <= 0.f)
    {
        m_pVIBufferCom->Trail_Map_Discard(nullptr, 0);
        return;
    }

    m_Vertices.clear();

    for (const TRAIL_POINT_INFO& tTrailPoint : m_TrailPointsInfo)
    {
        _float fU = tTrailPoint.fProgressRatio;

        VTXTEX tVexTex{};
        tVexTex.vPosition = tTrailPoint.vTopWorldPos;
        tVexTex.vTexcoord = _float2{ fU, 0.f };

        m_Vertices.push_back(tVexTex);

        tVexTex.vPosition = tTrailPoint.vBottomWorldPos;
        tVexTex.vTexcoord = _float2{ fU, 1.f };

        m_Vertices.push_back(tVexTex);
    }

    m_pVIBufferCom->Trail_Map_Discard(m_Vertices.data(), m_Vertices.size());
}

CTrailEffect* CTrailEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTrailEffect* pInstance = new CTrailEffect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created: CTrailEffect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTrailEffect::Clone(void* pArg)
{
    CTrailEffect* pInstance = new CTrailEffect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CTrailEffect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTrailEffect::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
