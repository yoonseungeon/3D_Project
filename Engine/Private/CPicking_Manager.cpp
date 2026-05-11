#include "CPicking_Manager.h"

#include "CGameInstance.h"

CPicking_Manager::CPicking_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPicking_Manager::Initialize()
{
	return S_OK;
}

void CPicking_Manager::Update_Picking_Manager()
{
    Cal_MouseWorld();
    Cal_PickingCollider();
}

HRESULT CPicking_Manager::Add_PickingCollider(CCollider* pCollider)
{
    if (pCollider == nullptr)
    {
        return E_FAIL;
    }

    auto iter = std::find(m_PickingColliders.begin(), m_PickingColliders.end(), pCollider);

    if (iter != m_PickingColliders.end())
    {
        return E_FAIL;
    }

    Safe_AddRef(pCollider);
    m_PickingColliders.push_back(pCollider);

    return S_OK;
}

void CPicking_Manager::Get_WorldRay(_float4& vOutRayPos, _float4& vOutRayDir)
{
    vOutRayPos = m_vWorldRayPos;
    vOutRayDir = m_vWroldRayDir;
}

_bool CPicking_Manager::Picking_Object(COLLISION_RAY_INFO& tOutColInfo)
{
    tOutColInfo = m_tPickingInfo;

    if (m_tPickingInfo.pColCollider != nullptr)
        return true;

    return false;
}

void CPicking_Manager::Cal_MouseWorld()
{
    const POINT ptMouse = m_pGameInstance->Get_MouseClientPos();

    VIEWPORT_SIZE tViewportDesc = m_pGameInstance->Get_ViewportDesc();

    _float4 vMouse = { (static_cast<_float>(ptMouse.x) / tViewportDesc.x) * 2.f - 1.f,
                        (static_cast<_float>(ptMouse.y) / tViewportDesc.y) * -2.f + 1.f,
                         0.f,
                         1.f
    };

    // Åõ¿µ
    XMVECTOR vMouseViewPos = XMVector3TransformCoord(XMLoadFloat4(&vMouse),
        XMLoadFloat4x4(m_pGameInstance->Get_Transform_Inverse(D3DTS::PROJ))
    );

    // ºä
    XMVECTOR vRayPos = vMouseViewPos;
    XMVECTOR vRayDir = XMVector3Normalize(vMouseViewPos - XMVectorSet(0.f, 0.f, 0.f, 1.f));

    vRayPos = XMVector3TransformCoord(vRayPos,
        XMLoadFloat4x4(m_pGameInstance->Get_Transform_Inverse(D3DTS::VIEW))
    );

    vRayDir = XMVector3TransformNormal(vRayDir,
        XMLoadFloat4x4(m_pGameInstance->Get_Transform_Inverse(D3DTS::VIEW))
    );

    XMStoreFloat4(&m_vWorldRayPos, vRayPos);
    XMStoreFloat4(&m_vWroldRayDir, XMVector3Normalize(vRayDir));
}

void CPicking_Manager::Cal_PickingCollider()
{
    m_tPickingInfo.pColObject = nullptr;
    m_tPickingInfo.pColCollider = nullptr;

    _float fMinDist = { FLT_MAX };
    _bool bFinalCol = { false };

    _vector vRayPos = XMLoadFloat4(&m_vWorldRayPos);
    _vector vRayDir = XMLoadFloat4(&m_vWroldRayDir);

    for (auto pCollider : m_PickingColliders)
    {
        if (pCollider->Get_CanMousePicking() == false)
            continue;

#ifdef _DEBUG
        //m_pGameInstance->Add_DebugComponent(pCollider);
#endif   

        _float fDist{};
        _bool bCol{};

        bCol = pCollider->Intersect_Ray(vRayPos, vRayDir, fDist);

        if (bCol == true && fMinDist > fDist)
        {
            fMinDist = fDist;

            m_tPickingInfo.pColObject = pCollider->Get_Owner();
            m_tPickingInfo.pColCollider = pCollider;
        }
    }
}

CPicking_Manager* CPicking_Manager::Create()
{
	CPicking_Manager* pInstance = new CPicking_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created: CPicking_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPicking_Manager::Free()
{
    for (auto pCollider : m_PickingColliders)
        Safe_Release(pCollider);
    m_PickingColliders.clear();

	Safe_Release(m_pGameInstance);

	__super::Free();
}