#include "CObject_Manager.h"

#include "CGameInstance.h"
#include "CGameObject.h"
#include "CLayer.h"

CObject_Manager::CObject_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	if (m_pLayers != nullptr)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pLayers = new LAYERS[iNumLevels];

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg, CGameObject** ppOut)
{
	if (iLayerLevelIndex >= m_iNumLevels || m_pLayers == nullptr)
		return E_FAIL;

	// Prototype_Manager에서 원형을 찾고, Clone(복사)해서 가지고 옴.
	// return 타입이 CBase*임
	CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (pGameObject == nullptr)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (pLayer == nullptr)
	{
		pLayer = CLayer::Create();
		m_pLayers[iLayerLevelIndex].emplace(strLayerTag, pLayer);
	}

	pLayer->Add_GameObject(pGameObject);

	if (ppOut != nullptr) {
		*ppOut = pGameObject;
		Safe_AddRef(pGameObject);
	}

	return S_OK;
}

void CObject_Manager::Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Priority_Update(fTimeDelta);
	}
}

void CObject_Manager::Parallel_Update(_float fTimeDelta)
{
	switch (m_eParallelMode)
	{
	case PARALLEL_UPDATE_MODE::PARALLEL:
	{
		m_iTotalJobCnt = 0;
		m_iFinishedJobCnt.store(0, memory_order_relaxed);

		for (size_t i = 0; i < m_iNumLevels; ++i)
		{
			for (auto& Pair : m_pLayers[i])
				Pair.second->Parallel_Update_Parallel(fTimeDelta, m_iTotalJobCnt, m_iFinishedJobCnt,
					[this](function<void()> funcJob)->void {
						m_pGameInstance->Add_Job(funcJob);
					}
				);
		}
		break;
	}
	case PARALLEL_UPDATE_MODE::SINGLE:
	{
		for (size_t i = 0; i < m_iNumLevels; ++i)
		{
			for (auto& Pair : m_pLayers[i])
				Pair.second->Parallel_Update_Single(fTimeDelta);
		}
		break;
	}
	}
}

void CObject_Manager::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Update(fTimeDelta);
	}
}

void CObject_Manager::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Late_Update(fTimeDelta);
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();
}

_bool CObject_Manager::Is_Parallel_Update_Finished()
{
	if (m_iFinishedJobCnt.load(memory_order_acquire) == m_iTotalJobCnt) {
		return true;
	}

	return false;
}

void CObject_Manager::Set_Parallel_Update_Mode(PARALLEL_UPDATE_MODE eParallelMode)
{
	m_eParallelMode = eParallelMode;
}

CLayer* CObject_Manager::Find_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	auto iter = m_pLayers[iLayerLevelIndex].find(strLayerTag);
	if (iter == m_pLayers[iLayerLevelIndex].end())
		return nullptr;

	return iter->second;
}

CObject_Manager* CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager* pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created: CObject_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;

}

void CObject_Manager::Free()
{
	for (size_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	Safe_Release(m_pGameInstance);

	__super::Free();
}
