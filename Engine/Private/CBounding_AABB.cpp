#include "CBounding_AABB.h"
#include "CBounding_OBB.h"
#include "CBounding_Sphere.h"
#include "DebugDraw.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_AABB::Initialize(CBounding::BOUNDING_DESC* pBoundingDesc)
{
    auto pDesc = static_cast<CBounding_AABB::BOUNDING_AABB_DESC*>(pBoundingDesc);
    
    if (pDesc->pLocalXYZ == nullptr)
    {
        m_pOriginalDesc = new BoundingBox(pDesc->vCenter, _float3(pDesc->vSize.x * 0.5f, pDesc->vSize.y * 0.5f, pDesc->vSize.z * 0.5f));
    }
    else
    {
        pDesc->vCenter.x = (pDesc->pLocalXYZ->vMin.x + pDesc->pLocalXYZ->vMax.x) * 0.5f;
        pDesc->vCenter.y = (pDesc->pLocalXYZ->vMin.y + pDesc->pLocalXYZ->vMax.y) * 0.5f;
        pDesc->vCenter.z = (pDesc->pLocalXYZ->vMin.z + pDesc->pLocalXYZ->vMax.z) * 0.5f;

        _float3 Extents;
        Extents.x = (pDesc->pLocalXYZ->vMax.x - pDesc->pLocalXYZ->vMin.x) * 0.5f;
        Extents.y = (pDesc->pLocalXYZ->vMax.y - pDesc->pLocalXYZ->vMin.y) * 0.5f;
        Extents.z = (pDesc->pLocalXYZ->vMax.z - pDesc->pLocalXYZ->vMin.z) * 0.5f;

        m_pOriginalDesc = new BoundingBox(pDesc->vCenter, Extents);
    }

    m_pDesc = new BoundingBox(*m_pOriginalDesc);

    return S_OK;
}

void XM_CALLCONV CBounding_AABB::Update(_fmatrix TransformMatrix)
{
    _matrix   Matrix = TransformMatrix;

    // 회전될 때마다 크기가 변해서 회전을 제거
    Matrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(Matrix.r[0]);
    Matrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(Matrix.r[1]);
    Matrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(Matrix.r[2]);

    m_pOriginalDesc->Transform(*m_pDesc, Matrix);
}

_bool CBounding_AABB::Intersect(COLLIDER eTargetType, CBounding* pBounding)
{
    m_isColl = false;

    switch (eTargetType)
    {
    case COLLIDER::AABB:
        m_isColl = m_pDesc->Intersects(*dynamic_cast<CBounding_AABB*>(pBounding)->Get_Desc());
        break;

    case COLLIDER::OBB:
        m_isColl = m_pDesc->Intersects(*dynamic_cast<CBounding_OBB*>(pBounding)->Get_Desc());
        break;

    case COLLIDER::SPHERE:
        m_isColl = m_pDesc->Intersects(*dynamic_cast<CBounding_Sphere*>(pBounding)->Get_Desc());
        break;
    }

    return m_isColl;
}

#ifdef _DEBUG
HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
    // 1. Batch 2. BoundingBox 3. 색깔 설정
    DX::Draw(pBatch, *m_pDesc, true == m_isColl ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

    return S_OK;
}
#endif

CBounding_AABB* CBounding_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pDesc)
{
    CBounding_AABB* pInstance = new CBounding_AABB(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pDesc)))
    {
        MSG_BOX("Failed to Created: CBounding_AABB");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBounding_AABB::Free()
{
    Safe_Delete(m_pDesc);
    Safe_Delete(m_pOriginalDesc);

    __super::Free();
}
