#include "CStage.h"

#include "CGameInstance.h"
#include "CGameObject.h"

CStage::CStage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

void CStage::Enable_Stage()
{
    for (auto pObj : m_vecGameObjects) {
        pObj->Set_IsInactive(false);
    }
}

void CStage::Disable_Stage()
{
    for (auto pObj : m_vecGameObjects) {
        pObj->Set_IsInactive(true);
    }
}

void CStage::Free()
{
    for (auto pObj : m_vecGameObjects) {
        Safe_Release(pObj);
    }
    m_vecGameObjects.clear();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    __super::Free();
}
