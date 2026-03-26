#include "CLayer.h"

#include "CGameObject.h"

CLayer::CLayer()
{
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

void CLayer::Parallel_Update_Parallel(_float fTimeDelta, _int& iTotalJobCnt, atomic<_int>& iFinishedJobCnt, function<void(function<void()> funcJob)> func)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (pGameObject != nullptr)
		{
			++iTotalJobCnt;
			func(
				[pGameObject, fTimeDelta, &iFinishedJobCnt]()->void
				{
					pGameObject->Parallel_Update(fTimeDelta);
					// main이 Update할 때 객체에 대한 최신 정보를 보장 위해서 release/acquire 사용
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
