#include "CShadow.h"
#include "CGameInstance.h"

CShadow::CShadow()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CShadow::Initialize(_uint iNumLevels)
{
    m_ShadowCount.resize(iNumLevels);

    return S_OK;
}

HRESULT CShadow::Add_ShadowLight(_uint iNumLevels, const SHADOW_LIGHT_DESC& ShadowDesc)
{
    m_iCurLevelIndex = iNumLevels;
    m_ShadowCount[iNumLevels] = 1;

    m_vEye = ShadowDesc.vEye;
    m_vAt = ShadowDesc.vAt;
    m_vUp = _float4{ 0.f, 1.f, 0.f, 0.f };

    XMStoreFloat4x4(&m_TransformStateMatrices[ETOUI(D3DTS::VIEW)],
        XMMatrixLookAtLH(
            XMLoadFloat4(&m_vEye),
            XMLoadFloat4(&m_vAt),
            XMLoadFloat4(&m_vUp)
        )
    );

    XMStoreFloat4x4(&m_TransformStateMatrices[ETOUI(D3DTS::PROJ)],
        XMMatrixOrthographicLH(
            ShadowDesc.fWidth,
            ShadowDesc.fHeight,
            ShadowDesc.fNear,
            ShadowDesc.fFar
        )
    );

    return S_OK;
}

void CShadow::Update_Shadow()
{
    if (m_iCurLevelIndex == -1)
        return;

    if (m_ShadowCount[m_iCurLevelIndex] == 0)
        return;

    const _float4* vCamPos = m_pGameInstance->Get_CamPosition();

    _vector vEye = XMLoadFloat4(&m_vEye) + XMLoadFloat4(vCamPos);
    _vector vAt = XMLoadFloat4(&m_vAt) + XMLoadFloat4(vCamPos);

    vEye = XMVectorSetW(vEye, 1.f);
    vAt = XMVectorSetW(vAt, 1.f);

    XMStoreFloat4x4(&m_TransformStateMatrices[ETOUI(D3DTS::VIEW)],
        XMMatrixLookAtLH(
            vEye,
            vAt,
            XMLoadFloat4(&m_vUp)
        )
    );
}

void CShadow::Clear_Shadow(_uint iNumLevels)
{
    if (iNumLevels >= m_ShadowCount.size())
        return;

    m_ShadowCount[iNumLevels] = 0;

    // 다음 Level이 생성되고 이전 레벨이 지워져서 무조건 -1로 하면 안됨.
    // 사실 -1로 굳이 초기화하지 않아도 문제없음.
    if(m_iCurLevelIndex == iNumLevels)
        m_iCurLevelIndex = -1;
}

CShadow* CShadow::Create(_uint iNumLevels)
{
    CShadow* pInstance = new CShadow();

    if (FAILED(pInstance->Initialize(iNumLevels)))
    {
        MSG_BOX("Failed to Created: CShadow");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CShadow::Free()
{
    Safe_Release(m_pGameInstance);

    __super::Free();
}
