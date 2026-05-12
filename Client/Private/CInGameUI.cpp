#include "CInGameUI.h"

#include "CGameInstance.h"

#include "CUI_Default.h"

CInGameUI::CInGameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CInGameUI::CInGameUI(const CInGameUI& Prototype)
    : CPartObject{ Prototype }
{
}

HRESULT CInGameUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInGameUI::Initialize(void* pArg)
{
    INGAMEUI_DESC* pDesc = static_cast<INGAMEUI_DESC*>(pArg);

    m_fHeight = pDesc->fHeight;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    auto tViewportDesc = m_pGameInstance->Get_ViewportDesc();

    /* 직교 투영으로 그려주기위한 뷰행렬 셋 */
    XMStoreFloat4x4(&m_TransformMatrices[ETOUI(D3DTS::VIEW)], XMMatrixIdentity());

    /* 직교 투영으로 그려주기위한 투영행렬 셋 */
    XMStoreFloat4x4(&m_TransformMatrices[ETOUI(D3DTS::PROJ)], XMMatrixOrthographicLH(2.f, 2.f, 0.1f, 1.f));

    return S_OK;
}

void CInGameUI::Priority_Update(_float fTimeDelta)
{
}

void CInGameUI::Parallel_Update(_float fTimeDelta)
{
}

void CInGameUI::Update(_float fTimeDelta)
{
}

void CInGameUI::Late_Update(_float fTimeDelta)
{
}

HRESULT CInGameUI::Render()
{
    return S_OK;
}

HRESULT CInGameUI::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, D3DTS eType)
{
    return pShader->Bind_Matrix(pConstantName, &m_TransformMatrices[ETOUI(eType)]);
}

void CInGameUI::Update_Pos()
{
    _float4 vPos{};
    memcpy(&vPos, m_pParentMatrix->m[3], sizeof(_float4));

    vPos.y += m_fHeight;

    _vector vWorldPos = XMLoadFloat4(&vPos);

    if (m_pGameInstance->isIn_Frustum_WorldSpace(vWorldPos, 2.f) == false)
    {
        m_bIsInactive = true;
        return;
    }

    _vector vViewPos = XMVector3TransformCoord(vWorldPos, XMLoadFloat4x4(m_pGameInstance->Get_Transform(D3DTS::VIEW)));
    _vector vNDCPos = XMVector3TransformCoord(vViewPos, XMLoadFloat4x4(m_pGameInstance->Get_Transform(D3DTS::PROJ)));

    _float4 vUIPos{ XMVectorGetX(vNDCPos), XMVectorGetY(vNDCPos) ,XMVectorGetZ(vNDCPos), 1.f };

    if (vUIPos.x < -1.f || vUIPos.x > 1.f ||
        vUIPos.y < -1.f || vUIPos.y > 1.f ||
        vUIPos.z < 0.f || vUIPos.z > 1.f)
    {
        m_bIsInactive = true;
        return;
    }

    m_bIsInactive = false;

    vUIPos.z = 0.1f;

    m_pTransformCom->Set_Pos(XMLoadFloat4(&vUIPos));
}

void CInGameUI::Free()
{
    __super::Free();
}
