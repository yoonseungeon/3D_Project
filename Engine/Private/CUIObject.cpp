#include "CUIObject.h"

#include "CShader.h"

CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CUIObject::CUIObject(const CUIObject& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CUIObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUIObject::Initialize(void* pArg)
{
    if (pArg == nullptr)
        return E_FAIL;

    UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_iUILayer = pDesc->iUILayer;
    m_iFlipX = pDesc->iFlipX;
    m_iFlipY = pDesc->iFlipY;

    _uint iNumViewport = { 1 };
    D3D11_VIEWPORT ViewportDesc = {};
    m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

    /* 직교 투영으로 그려주기위한 월드행렬 셋 */
    // 윈도우 크기 대비 UI 크기 비율
    m_pTransformCom->Set_Scale(ViewportDesc.Width * pDesc->fScaleRatioX, ViewportDesc.Height * pDesc->fScaleRatioY, 1.f);

    // 화면 중심 기준 UI 위치 비율(범위 -0.5 ~ 0.5)
    m_pTransformCom->Set_State(STATE::POSITION,
        XMVectorSet(ViewportDesc.Width * pDesc->fPosRatioX, ViewportDesc.Height * pDesc->fPosRatioY, 0.1f, 1.f));

    /* 직교 투영으로 그려주기위한 뷰행렬 셋 */
    XMStoreFloat4x4(&m_TransformMatrices[ETOUI(D3DTS::VIEW)], XMMatrixIdentity());

    /* 직교 투영으로 그려주기위한 투영행렬 셋 */
    XMStoreFloat4x4(&m_TransformMatrices[ETOUI(D3DTS::PROJ)], XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.1f, 1.f));

    return S_OK;
}

void CUIObject::Priority_Update(_float fTimeDelta)
{
}

void CUIObject::Update(_float fTimeDelta)
{
}

void CUIObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIObject::Render()
{
    return S_OK;
}

HRESULT CUIObject::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, D3DTS eType)
{
    return pShader->Bind_Matrix(pConstantName, &m_TransformMatrices[ETOUI(eType)]);
}

void CUIObject::Free()
{
    __super::Free();
}
