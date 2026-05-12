#include "CFrustum.h"
#include "CGameInstance.h"

CFrustum::CFrustum()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CFrustum::Initialize()
{
    m_vOriginalPoints[0] = _float4(-1.f, 1.f, 0.f, 1.f);
    m_vOriginalPoints[1] = _float4(1.f, 1.f, 0.f, 1.f);
    m_vOriginalPoints[2] = _float4(1.f, -1.f, 0.f, 1.f);
    m_vOriginalPoints[3] = _float4(-1.f, -1.f, 0.f, 1.f);

    m_vOriginalPoints[4] = _float4(-1.f, 1.f, 1.f, 1.f);
    m_vOriginalPoints[5] = _float4(1.f, 1.f, 1.f, 1.f);
    m_vOriginalPoints[6] = _float4(1.f, -1.f, 1.f, 1.f);
    m_vOriginalPoints[7] = _float4(-1.f, -1.f, 1.f, 1.f);

    return S_OK;
}

void CFrustum::Update()
{
    for (size_t i = 0; i < 8; i++)
    {
        XMStoreFloat4(&m_vWorldPoints[i],
            XMVector3TransformCoord(XMLoadFloat4(&m_vOriginalPoints[i]),
                XMLoadFloat4x4(m_pGameInstance->Get_Transform_Inverse(D3DTS::PROJ))));

        XMStoreFloat4(&m_vWorldPoints[i],
            XMVector3TransformCoord(XMLoadFloat4(&m_vWorldPoints[i]),
                XMLoadFloat4x4(m_pGameInstance->Get_Transform_Inverse(D3DTS::VIEW))));
    }

    Make_Planes(m_vWorldPoints, m_vWorldPlanes);
}

_bool CFrustum::isIn_WorldSpace(_fvector vWorldPos, _float fRange)
{
    for (size_t i = 0; i < 6; ++i)
    {
        if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vWorldPlanes[i]), vWorldPos)))
            return false;
    }

    return true;
}

void CFrustum::Make_Planes(const _float4* pPoints, _float4* pPlanes)
{
    _vector vPoints[8] =
    {
        XMLoadFloat4(&pPoints[0]) ,XMLoadFloat4(&pPoints[1]) ,
        XMLoadFloat4(&pPoints[2]) ,XMLoadFloat4(&pPoints[3]) ,
        XMLoadFloat4(&pPoints[4]) ,XMLoadFloat4(&pPoints[5]) ,
        XMLoadFloat4(&pPoints[6]) ,XMLoadFloat4(&pPoints[7])
    };

    XMStoreFloat4(&pPlanes[0], XMPlaneNormalize(XMPlaneFromPoints(vPoints[1], vPoints[5], vPoints[6])));
    XMStoreFloat4(&pPlanes[1], XMPlaneNormalize(XMPlaneFromPoints(vPoints[4], vPoints[0], vPoints[3])));
    XMStoreFloat4(&pPlanes[2], XMPlaneNormalize(XMPlaneFromPoints(vPoints[4], vPoints[5], vPoints[1])));
    XMStoreFloat4(&pPlanes[3], XMPlaneNormalize(XMPlaneFromPoints(vPoints[3], vPoints[2], vPoints[6])));
    XMStoreFloat4(&pPlanes[4], XMPlaneNormalize(XMPlaneFromPoints(vPoints[5], vPoints[4], vPoints[7])));
    XMStoreFloat4(&pPlanes[5], XMPlaneNormalize(XMPlaneFromPoints(vPoints[0], vPoints[1], vPoints[2])));
}

CFrustum* CFrustum::Create()
{
    CFrustum* pInstance = new CFrustum();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created: CFrustum");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFrustum::Free()
{
    Safe_Release(m_pGameInstance);

    __super::Free();
}
