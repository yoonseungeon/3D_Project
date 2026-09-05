#include "CLayer.h"

#include "CGameObject.h"
#include "CContainerObject.h"

CLayer::CLayer()
{
}

CComponent* CLayer::Get_Component(const _wstring& strComponentTag, _uint iIndex)
{
	auto iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Find_Component(strComponentTag);
}

CComponent* CLayer::Get_Component(const _wstring& strPartTag, const _wstring& strComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return dynamic_cast<CContainerObject*>((*iter))->Find_PartObjectComponent(strPartTag, strComponentTag);
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
	if (pGameObject == nullptr)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

void CLayer::Priority_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (pGameObject != nullptr)
			pGameObject->Priority_Update(fTimeDelta);
	}

}

void CLayer::Parallel_Update_Parallel(_float fTimeDelta, _int& iTotalJobCnt, atomic<_int>& iFinishedJobCnt,
	function<void(function<void()> funcJob)> addJob)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (pGameObject != nullptr)
		{
			++iTotalJobCnt;
			addJob
			(
				[pGameObject, fTimeDelta, &iFinishedJobCnt]()->void
				{
					pGameObject->Parallel_Update(fTimeDelta);
					// 병렬 업데이트 결과가 Main Thread에 반영되도록 release/acquire 사용
					iFinishedJobCnt.fetch_add(1, memory_order_release);
				}
			);
		}
	}
}

void CLayer::Parallel_Update_Single(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (pGameObject != nullptr)
			pGameObject->Parallel_Update(fTimeDelta);
	}
}

void CLayer::Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (pGameObject != nullptr)
			pGameObject->Update(fTimeDelta);
	}
}

void CLayer::Late_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (pGameObject != nullptr)
			pGameObject->Late_Update(fTimeDelta);
	}
}

CLayer* CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	for (CGameObject*& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();

	__super::Free();
}
