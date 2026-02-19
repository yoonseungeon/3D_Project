#include "CLevel_Manager.h"

#include"CGameInstance.h"
#include "CLevel.h"

CLevel_Manager::CLevel_Manager()
    :m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel_Manager::Change_Level(_int iNewLevelIndex, CLevel* pNewLevel)
{   
    if (pNewLevel == nullptr) {
        return E_FAIL;
    }

    /* 기존 레벨용 자원을 정리. */
    // GameInstance에서 Mgr들 포인터를 들고 있을 것이기 때문에 GameInstance를 통해서 지우자.
    // 따로 지워도 되는데 Mgr들 싱클톤 아님. Mgr들 여러 개 생길 위험있음. 그래서 GameInstance에서
    // Engine의 다른 Mgr 접근하고 싶다. -> GameInstance를 통해서
    m_pGameInstance->Clear_Resources(m_iCurrentLevelIndex);

    Safe_Release(m_pCurrentLevel);

    m_pCurrentLevel = pNewLevel;
    m_iCurrentLevelIndex = iNewLevelIndex;

    return S_OK;
}

void CLevel_Manager::Update(_float fTimeDelta)
{
    if (m_pCurrentLevel == nullptr)
    {
        return;
    }

    m_pCurrentLevel->Update(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
    if (m_pCurrentLevel == nullptr)
    {
        return E_FAIL;
    }

    return m_pCurrentLevel->Render();
}

CLevel_Manager* CLevel_Manager::Create()
{
    return new CLevel_Manager();
}

void CLevel_Manager::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pCurrentLevel);
}
