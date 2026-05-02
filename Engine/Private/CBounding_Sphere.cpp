#include "CBounding_AABB.h"
#include "CBounding_OBB.h"
#include "CBounding_Sphere.h"
#include "DebugDraw.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_Sphere::Initialize(CBounding::BOUNDING_DESC* pBoundingDesc)
{
    auto pDesc = static_cast<CBounding_Sphere::BOUNDING_SPHERE_DESC*>(pBoundingDesc);

    if (pDesc->pLocalXYZ == nullptr)
    {
        m_pOriginalDesc = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
    }
    else
    {
        pDesc->vCenter.x = (pDesc->pLocalXYZ->vMin.x + pDesc->pLocalXYZ->vMax.x) * 0.5f;
        pDesc->vCenter.y = (pDesc->pLocalXYZ->vMin.y + pDesc->pLocalXYZ->vMax.y) * 0.5f;
        pDesc->vCenter.z = (pDesc->pLocalXYZ->vMin.z + pDesc->pLocalXYZ->vMax.z) * 0.5f;

        pDesc->fRadius = 0.f;
        pDesc->fRadius = (std::max)(pDesc->fRadius, (pDesc->pLocalXYZ->vMax.x - pDesc->pLocalXYZ->vMin.x) * 0.5f);
        pDesc->fRadius = (std::max)(pDesc->fRadius, (pDesc->pLocalXYZ->vMax.y - pDesc->pLocalXYZ->vMin.y) * 0.5f);
        pDesc->fRadius = (std::max)(pDesc->fRadius, (pDesc->pLocalXYZ->vMax.z - pDesc->pLocalXYZ->vMin.z) * 0.5f);

        m_pOriginalDesc = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
    }

    m_pDesc = new BoundingSphere(*m_pOriginalDesc);

    return S_OK;
}

void XM_CALLCONV CBounding_Sphere::Update(_fmatrix TransformMatrix)
{
    m_pOriginalDesc->Transform(*m_pDesc, TransformMatrix);
}

_bool CBounding_Sphere::Intersect(COLLIDER eTargetType, CBounding* pBounding)
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
    }

    return isColl;
}

_bool XM_CALLCONV CBounding_Sphere::Intersect_Ray(_fvector vRayPos, _fvector vRayDir, _float& fDist)
{
    _bool m_bResult = m_pDesc->Intersects(vRayPos, vRayDir, fDist);

#ifdef _DEBUG
    if (m_bResult == true)
        m_isPicked = true;
#endif

    return m_bResult;
}

#ifdef _DEBUG
HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
#ifdef _DEBUG
    DX::Draw(pBatch, *m_pDesc, m_isPicked == true ? XMVectorSet(0.f, 0.f, 1.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));
    return S_OK;
#endif

    DX::Draw(pBatch, *m_pDesc, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

    return S_OK;
}
#endif

CBounding_Sphere* CBounding_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pDesc)
{
    CBounding_Sphere* pInstance = new CBounding_Sphere(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pDesc)))
    {
        MSG_BOX("Failed to Created: CBounding_Sphere");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBounding_Sphere::Free()
{
    Safe_Delete(m_pDesc);
    Safe_Delete(m_pOriginalDesc);

    __super::Free();
}
