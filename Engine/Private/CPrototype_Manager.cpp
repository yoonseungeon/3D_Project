#include "CPrototype_Manager.h"

CPrototype_Manager::CPrototype_Manager()
{
}

HRESULT CPrototype_Manager::Initialize(_uint iNumLevels)
{
	if (m_pPrototypes != nullptr)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pPrototypes = new PROTOTYPES[iNumLevels];

	return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	if (m_pPrototypes == nullptr || iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	auto pair = m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);

	if (pair.second == false) {
		MSG_BOX("CPrototype_Manager - Failed to add prototype");
		return E_FAIL;
	}

	return S_OK;
}

CBase* CPrototype_Manager::Clone_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag)
{
	return nullptr;
}

CBase* CPrototype_Manager::Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag)
{
	auto iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);
	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

CPrototype_Manager* CPrototype_Manager::Create(_uint iNumLevels)
{
	CPrototype_Manager* pInstance = new CPrototype_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created: CPrototype_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPrototype_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);
		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);
}
