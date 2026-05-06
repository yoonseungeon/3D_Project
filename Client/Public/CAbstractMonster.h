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
		CON_ATTACK = 1 << 0
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

protected:
	CInGame_Manager* m_pInGame_Manager{};

	_float m_fBewareRange{};
	CUnit* m_pTargetPlayer{};

	_uint iMonsterCondition{};

	_float3 m_vStartPos{};

	_bool m_bIsAttackProcessed{};

protected:
	_bool PlayerIsInRange(_float fRange);	
	_bool IsNearSpawnPoint(_float fRange);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free() override;
};

NS_END