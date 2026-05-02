#include "CBounding_AABB.h"
#include "CBounding_OBB.h"
#include "CBounding_Sphere.h"
#include "DebugDraw.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_OBB::Initialize(CBounding::BOUNDING_DESC* pBoundingDesc)
{
    auto pDesc = static_cast<CBounding_OBB::BOUNDING_OBB_DESC*>(pBoundingDesc);

    _float4 vQuaternion = { 0.f, 0.f, 0.f, 1.f };
  
    if (pDesc->pLocalXYZ == nullptr)
    {
        // 사원수를 요구함.
        XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(pDesc->vRadians.x, pDesc->vRadians.y, pDesc->vRadians.z));
        m_pOriginalDesc = new BoundingOrientedBox(pDesc->vCenter, _float3(pDesc->vSize.x * 0.5f, pDesc->vSize.y * 0.5f, pDesc->vSize.z * 0.5f), vQuaternion);
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

        m_pOriginalDesc = new BoundingOrientedBox(pDesc->vCenter, Extents, vQuaternion);
    }

    m_pDesc = new BoundingOrientedBox(*m_pOriginalDesc);

    return S_OK;
}

void XM_CALLCONV CBounding_OBB::Update(_fmatrix TransformMatrix)
{
    m_pOriginalDesc->Transform(*m_pDesc, TransformMatrix);
}

_bool CBounding_OBB::Intersect(COLLIDER eTargetType, CBounding* pBounding)
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

#ifdef _DEBUG
HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
    DX::Draw(pBatch, *m_pDesc, true == m_isColl ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

    return S_OK;
}
#endif

CBounding_OBB* CBounding_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pDesc)
{
    CBounding_OBB* pInstance = new CBounding_OBB(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pDesc)))
    {
        MSG_BOX("Failed to Created: CBounding_OBB");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBounding_OBB::Free()
{
    Safe_Delete(m_pDesc);
    Safe_Delete(m_pOriginalDesc);

    __super::Free();
}
