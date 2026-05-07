#include "CShadow.h"
#include "CGameInstance.h"

CShadow::CShadow()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CShadow::Add_ShadowLight(const SHADOW_LIGHT_DESC& ShadowDesc)
{
    XMStoreFloat4x4(&m_TransformStateMatrices[ETOUI(D3DTS::VIEW)],
        XMMatrixLookAtLH(
            XMLoadFloat4(&ShadowDesc.vEye),
            XMLoadFloat4(&ShadowDesc.vAt),
            XMVectorSet(0.f, 1.f, 0.f, 0.f)
        )
    );

    auto tViewportDesc = m_pGameInstance->Get_ViewportDesc();

    XMStoreFloat4x4(&m_TransformStateMatrices[ETOUI(D3DTS::PROJ)],
        XMMatrixPerspectiveFovLH(
            ShadowDesc.fFovy,
            static_cast<_float>(tViewportDesc.x) / static_cast<_float>(tViewportDesc.y),
            ShadowDesc.fNear,
            ShadowDesc.fFar
        )
    );

    return S_OK;
}

CShadow* CShadow::Create()
{
    return new CShadow();
}

void CShadow::Free()
{
    Safe_Release(m_pGameInstance);

    __super::Free();
}
