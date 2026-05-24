#pragma once

#include "CUnit.h"

NS_BEGIN(Client)

class CInGame_Manager;
class CUnit;

class CAbstractMonster abstract : public CUnit
{
public:
	struct CABSTRACTMONSTER_DESC : public CGameObject::GAMEOBJECT_DESC
	{
	};

	enum MONSTER_CONDITION
	{ 
		CON_ATTACK = 1 << 0,
		CON_HPZERO = 1 << 1,
		CON_DEAD = 1 << 2,
		CON_STUN = 1 << 3
	};

protected:
	CAbstractMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAbstractMonster(const CAbstractMonster& Prototype);
	virtual ~CAbstractMonster() = default;

protected:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Damaged(const DAMAGE_INFO& tDamageInfo) override;
	virtual _bool IsUnitDead() override;

	virtual _bool XM_CALLCONV IsInOpenRange(_fvector vPos, _float fWorldDistance) override;

protected:
	_float m_fBewareRange{};
	CUnit* m_pTargetPlayer{};

	_uint m_iMonsterCondition{};

	_float3 m_vStartPos{};

	_bool m_bIsAttackProcessed{};

	MODEL_LOCAL_MIN_MAX tLocalMinMax{};

	_float m_fStunTime{};

protected:
	_bool PlayerIsInRange(_float fRange);	
	_bool IsNearSpawnPoint(_float fRange);
	void Cal_LocalMinMaxAABB(MODEL_LOCAL_MIN_MAX& tLocalMinMax, _float3& vCenter, _float3& vSize);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free() override;
};

NS_END