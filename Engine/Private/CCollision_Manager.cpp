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
	{
		return E_FAIL;
	}

	auto iter = std::find(m_Colliders.begin(), m_Colliders.end(), pCollider);

	if (iter != m_Colliders.end())
	{
		return E_FAIL;
	}

	Safe_AddRef(pCollider);
	m_Colliders.push_back(pCollider);
	pCollider->Set_ID(Acquire_ColliderId());

	return S_OK;
}

HRESULT CCollision_Manager::Substract_Collider(CCollider* pCollider)
{
	if (pCollider == nullptr)
	{
		return E_FAIL;
	}

	auto iter = std::find(m_Colliders.begin(), m_Colliders.end(), pCollider);

	if (iter == m_Colliders.end())
	{
		return E_FAIL;
	}

	Substract_CollisionPair(pCollider);

	Safe_Release(*iter);
	m_Colliders.erase(iter);

	return S_OK;
}

void CCollision_Manager::Update_Collision()
{
	for (auto pCollider : m_Colliders)
	{
		pCollider->Set_IsColl(false);

		if (pCollider->Get_Active() == true)
		{
			m_ActiveColliders.push_back(pCollider);
#ifdef _DEBUG
			m_pGameInstance->Add_DebugComponent(pCollider);
#endif   
		}
	}

	for (_uint i = 0; i < m_ActiveColliders.size(); ++i)
	{
		for (_uint j = i + 1; j < m_ActiveColliders.size(); ++j)
		{
			if (m_ActiveColliders[i]->Get_Active() == false || m_ActiveColliders[j]->Get_Active() == false)
			{
				//Call_CollisionExitFunc(m_ActiveColliders[i], m_ActiveColliders[j]);
				continue;
			}

			if (CanCollision(m_ActiveColliders[i], m_ActiveColliders[j]) == false)
				continue;

			if (m_ActiveColliders[i]->Intersect(m_ActiveColliders[j]) == false)
			{
				Call_CollisionExitFunc(m_ActiveColliders[i], m_ActiveColliders[j]);
				continue;
			}

			m_ActiveColliders[i]->Set_IsColl(true);
			m_ActiveColliders[j]->Set_IsColl(true);

			Call_CollisionFunc(m_ActiveColliders[i], m_ActiveColliders[j]);
		}
	}

	m_ActiveColliders.clear();	

	m_PreColPairs = m_CurColPairs;
	m_CurColPairs.clear();
}

_bool XM_CALLCONV CCollision_Manager::Collision_Ray(_fvector vRayPos, _fvector vRayDir, COLLISION_RAY_INFO& tOutColInfo)
{
	tOutColInfo.pColObject = nullptr;
	tOutColInfo.pColCollider = nullptr;

	_float fMinDist = { FLT_MAX };
	_bool bFinalCol = { false };

	for (auto pCollider : m_Colliders)
	{
		if (pCollider->Get_CanMousePicking() == false)
			continue;

		_float fDist{};
		_bool bCol{};

		bCol = pCollider->Intersect_Ray(vRayPos, vRayDir, fDist);

		if (bCol == true && fMinDist > fDist)
		{
			fMinDist = fDist;
			bFinalCol = true;

			tOutColInfo.pColObject = pCollider->Get_Owner();
			tOutColInfo.pColCollider = pCollider;
		}
	}

	return bFinalCol;
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

void CCollision_Manager::Call_CollisionExitFunc(CCollider* pSrc, CCollider* pDst)
{
	_ulonglong ullColPair = COLLISION_PAIR(pSrc->Get_ID(), pDst->Get_ID()).Get_CollisionPair();

	auto iter = m_PreColPairs.find(ullColPair);

	if (iter != m_PreColPairs.end())
	{
		COLLISION_INFO tSrcCol = MakeCollisionInfo(pSrc, pDst);
		COLLISION_INFO tDstCol = MakeCollisionInfo(pDst, pSrc);

		pSrc->Get_Owner()->OnCollision_Exit(tSrcCol);
		pDst->Get_Owner()->OnCollision_Exit(tDstCol);
	}
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
	for (auto pCollider : m_Colliders)
		Safe_Release(pCollider);
	m_Colliders.clear();

	Safe_Release(m_pGameInstance);

	__super::Free();
}
