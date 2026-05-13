#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

class CGameInstance;
class CCollider;

class CCollision_Manager final : public CBase
{
private:
	struct COLLISION_PAIR
	{
		COLLISION_PAIR(_uint iLeft, _uint iRight)
		{
			iLeftId = iLeft;
			iRightId = iRight;
			if (iLeft > iRight)
			{
				std::swap(iLeftId, iRightId);
			}
		}

		_uint iLeftId{};
		_uint iRightId{};

		_ulonglong Get_CollisionPair()
		{
			return (static_cast<_ulonglong>(iLeftId) << 32) | (static_cast<_ulonglong>(iRightId));
		}
	};

private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

private:
	HRESULT Initialize();

public:
	HRESULT Add_Collider(CCollider* pCollider);
	HRESULT Substract_Collider(CCollider* pCollider);

	void Update_Collision();

private:
	CGameInstance* m_pGameInstance = { nullptr };

	unordered_map<_uint, CCollider*> m_Colliders;
	vector<CCollider*> m_ActiveColliders;

	_uint m_iNextColliderId{};

	unordered_set<_ulonglong> m_PreColPairs;
	unordered_set<_ulonglong> m_CurColPairs;

private:
	_bool CanCollision(CCollider* pSrc, CCollider* pDst);
	void Call_CollisionFunc(CCollider* pSrc, CCollider* pDst);
	void Call_CollisionExitFunc();

	CCollider* Find_ColliderById(_uint iId);

	COLLISION_INFO MakeCollisionInfo(CCollider* pSrc, CCollider* pDst);

	_uint Acquire_ColliderId();

	void Substract_CollisionPair(CCollider* pCollider);

public:
	static CCollision_Manager* Create();
protected:
	virtual void Free() override;
};

NS_END