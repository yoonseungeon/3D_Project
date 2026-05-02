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

void CPicking_Manager::Get_WorldRay(_float4& vOutRayPos, _float4& vOutRayDir)
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

    XMStoreFloat4(&vOutRayPos, vRayPos);
    XMStoreFloat4(&vOutRayDir, XMVector3Normalize(vRayDir));
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
	Safe_Release(m_pGameInstance);

	__super::Free();
}