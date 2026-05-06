#include "CBounding_Frustum.h"
#include "CBounding_AABB.h"
#include "CBounding_OBB.h"
#include "CBounding_Sphere.h"

#include "DebugDraw.h"

CBounding_Frustum::CBounding_Frustum(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_Frustum::Initialize(CBounding::BOUNDING_DESC* pBoundingDesc)
{
    auto pDesc = static_cast<CBounding_Frustum::BOUNDING_FRUSTUM_DESC*>(pBoundingDesc);

    _float4 vQuaternion = { 0.f, 0.f, 0.f, 1.f };

    if (pDesc->pLocalXYZ == nullptr)
    {
        XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(pDesc->vRadians.x, pDesc->vRadians.y, pDesc->vRadians.z));

        m_pOriginalDesc = new BoundingFrustum(pDesc->vOrigin, vQuaternion, pDesc->fRightSlope, pDesc->fLeftSlope, pDesc->fTopSlope, pDesc->fBottomSlope, pDesc->fNear, pDesc->fFar);
    }
    else
    {
        // Min Max 가지고 딱히 만들 모양이 없음
        return E_FAIL;
    }

    m_pDesc = new BoundingFrustum(*m_pOriginalDesc);

    return S_OK;
}

void XM_CALLCONV CBounding_Frustum::Update(_fmatrix TransformMatrix)
{
    m_pOriginalDesc->Transform(*m_pDesc, TransformMatrix);
}

_bool CBounding_Frustum::Intersect(COLLIDER eTargetType, CBounding* pBounding)
{
    _bool isColl = false;

    switch (eTargetType)
    {
    case COLLIDER::AABB:
        isColl = m_pDesc->Intersects(*dynamic_cast<CBounding_AABB*>(pBounding)->Get_Desc());
        break;

    case COLLIDER::OBB:
        isColl = m_pDesc->Intersects(*dynamic_cast<CBounding_OBB*>(pBounding)->Get_Desc());
        break;

    case COLLIDER::SPHERE:
        isColl = m_pDesc->Intersects(*dynamic_cast<CBounding_Sphere*>(pBounding)->Get_Desc());
        break;

    case COLLIDER::FRUSTUM:
        isColl = m_pDesc->Intersects(*dynamic_cast<CBounding_Frustum*>(pBounding)->Get_Desc());
        break;
    }

    return isColl;
}

_bool XM_CALLCONV CBounding_Frustum::Intersect_Ray(_fvector vRayPos, _fvector vRayDir, _float& fDist)
{
    _bool m_bResult = m_pDesc->Intersects(vRayPos, vRayDir, fDist);

#ifdef _DEBUG
    if (m_bResult == true)
        m_isPicked = true;
#endif

    return m_bResult;
}

#ifdef _DEBUG
HRESULT CBounding_Frustum::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
#ifdef _DEBUG
    if (m_isColl == true)
    {
        DX::Draw(pBatch, *m_pDesc, XMVectorSet(1.f, 0.f, 0.f, 1.f));
        return S_OK;
    }

    DX::Draw(pBatch, *m_pDesc, m_isPicked == true ? XMVectorSet(0.f, 0.f, 1.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));
    return S_OK;
#endif

    // 1. Batch 2. BoundingBox 3. 색깔 설정
    DX::Draw(pBatch, *m_pDesc, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

    return S_OK;
}
#endif

CBounding_Frustum* CBounding_Frustum::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pDesc)
{
    CBounding_Frustum* pInstance = new CBounding_Frustum(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pDesc)))
    {
        MSG_BOX("Failed to Created: CBounding_Frustum");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBounding_Frustum::Free()
{
    Safe_Delete(m_pDesc);
    Safe_Delete(m_pOriginalDesc);

    __super::Free();
}
