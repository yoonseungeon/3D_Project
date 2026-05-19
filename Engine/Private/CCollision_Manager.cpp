#include "CCollision_Manager.h"

#include "CGameInstance.h"
#include "CCollider.h"
#include "CGameObject.h"

CCollision_Manager::CCollision_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCollision_Manager::Initialize()
{
	return S_OK;
}

HRESULT CCollision_Manager::Add_Collider(CCollider* pCollider)
{
	if (pCollider == nullptr)
		return E_FAIL;

	for (auto& pair : m_Colliders)
	{
		if(pair.second == pCollider)
			return E_FAIL;
	}

	pCollider->Set_ID(Acquire_ColliderId());
	Safe_AddRef(pCollider);

	m_Colliders.emplace(pCollider->Get_ID(), pCollider);

	return S_OK;
}

HRESULT CCollision_Manager::Substract_Collider(CCollider* pCollider)
{
	if (pCollider == nullptr)
		return E_FAIL;

	auto iter = m_Colliders.find(pCollider->Get_ID());

	if (iter == m_Colliders.end())
		return E_FAIL;

	Substract_CollisionPair(pCollider);

	Safe_Release(iter->second);
	m_Colliders.erase(iter);

	return S_OK;
}

void CCollision_Manager::Update_Collision()
{
	for (auto& pair : m_Colliders)
	{
		pair.second->Set_IsColl(false);

		if (pair.second->Get_Active() == true)
		{
			m_ActiveColliders.push_back(pair.second);
#ifdef _DEBUG
			m_pGameInstance->Add_DebugComponent(pair.second);
#endif   
		}
	}

	for (_uint i = 0; i < m_ActiveColliders.size(); ++i)
	{
		for (_uint j = i + 1; j < m_ActiveColliders.size(); ++j)
		{
			if (CanCollision(m_ActiveColliders[i], m_ActiveColliders[j]) == false)
				continue;

			if (m_ActiveColliders[i]->Intersect(m_ActiveColliders[j]) == false)
				continue;

			m_ActiveColliders[i]->Set_IsColl(true);
			m_ActiveColliders[j]->Set_IsColl(true);

			Call_CollisionFunc(m_ActiveColliders[i], m_ActiveColliders[j]);
		}
	}

	Call_CollisionExitFunc();

	m_ActiveColliders.clear();	

	m_PreColPairs = m_CurColPairs;
	m_CurColPairs.clear();
}

void CCollision_Manager::Clear()
{
	for (auto& pair : m_Colliders)
		Safe_Release(pair.second);
	m_Colliders.clear();

	m_PreColPairs.clear();
	m_CurColPairs.clear();
}

_bool CCollision_Manager::CanCollision(CCollider* pSrc, CCollider* pDst)
{
	if ((pSrc->Get_Layer() & pDst->Get_Mask())|| (pDst->Get_Layer() & pSrc->Get_Mask()))
	{
		return true;
	}

	return false;
}

void CCollision_Manager::Call_CollisionFunc(CCollider* pSrc, CCollider* pDst)
{
	_ulonglong ullColPair = COLLISION_PAIR(pSrc->Get_ID(), pDst->Get_ID()).Get_CollisionPair();

	auto iter = m_PreColPairs.find(ullColPair);

	COLLISION_INFO tSrcCol = MakeCollisionInfo(pSrc, pDst);
	COLLISION_INFO tDstCol = MakeCollisionInfo(pDst, pSrc);

	if (iter == m_PreColPairs.end())
	{	
		pSrc->Get_Owner()->OnCollision_Enter(tSrcCol);
		pDst->Get_Owner()->OnCollision_Enter(tDstCol);
	}
	else
	{
		pSrc->Get_Owner()->OnCollision_Stay(tSrcCol);
		pDst->Get_Owner()->OnCollision_Stay(tDstCol);
	}

	m_CurColPairs.insert(ullColPair);
}

void CCollision_Manager::Call_CollisionExitFunc()
{
	for (auto ullColPair : m_PreColPairs)
	{
		auto iter  = m_CurColPairs.find(ullColPair);

		if (iter == m_CurColPairs.end())
		{
			_uint iLeftId = static_cast<_uint>((ullColPair) >> 32);
			_uint iRightId = static_cast<_uint>(ullColPair);

			CCollider* pLeft = Find_ColliderById(iLeftId);
			CCollider* pRight = Find_ColliderById(iRightId);

			if (pLeft == nullptr || pRight == nullptr)
				continue;

			COLLISION_INFO tSrcCol = MakeCollisionInfo(pLeft, pRight);
			COLLISION_INFO tDstCol = MakeCollisionInfo(pRight, pLeft);

			pLeft->Get_Owner()->OnCollision_Exit(tSrcCol);
			pRight->Get_Owner()->OnCollision_Exit(tDstCol);
		}
	}
}

CCollider* CCollision_Manager::Find_ColliderById(_uint iId)
{
	auto iter = m_Colliders.find(iId);

	if(iter == m_Colliders.end())
		return nullptr;

	return iter->second;
}

COLLISION_INFO CCollision_Manager::MakeCollisionInfo(CCollider* pSrc, CCollider* pDst)
{
	COLLISION_INFO tSrcCol{};
	tSrcCol.pColObject = pDst->Get_Owner();
	tSrcCol.pMyCollider = pSrc;
	tSrcCol.pColCollider = pDst;

	return tSrcCol;
}

_uint CCollision_Manager::Acquire_ColliderId()
{
	return m_iNextColliderId++;
}

void CCollision_Manager::Substract_CollisionPair(CCollider* pCollider)
{
	_uint iColliderId = pCollider->Get_ID();

	for (auto iter = m_PreColPairs.begin(); iter != m_PreColPairs.end();) {
		_uint iLeftId = static_cast<_uint>((*iter) >> 32);
		_uint iRightId = static_cast<_uint>(*iter);

		if (iLeftId == iColliderId || iRightId == iColliderId)
		{
			iter = m_PreColPairs.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

CCollision_Manager* CCollision_Manager::Create()
{
	CCollision_Manager* pInstance = new CCollision_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created: CCollision_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollision_Manager::Free()
{
	for (auto& Pair : m_Colliders)
		Safe_Release(Pair.second);
	m_Colliders.clear();

	Safe_Release(m_pGameInstance);

	__super::Free();
}
