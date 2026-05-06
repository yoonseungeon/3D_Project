#pragma once

#include "CActionState.h"

class CLiDailinAttack : public CActionState
{
private:
	CLiDailinAttack();
	virtual ~CLiDailinAttack() = default;

public:
	void Enter(CLiDailin* pPlayer) override;
	void Update(CLiDailin* pPlayer, _float fTimeDelta) override;
	void Exit(CLiDailin* pPlayer) override;
	void HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command) override;

private:
	_bool m_bIsChasing{};

	_float m_fAttackRange{};

	_bool m_bIsAttackProcessed{};

	_uint m_iCurBodyAni{};
	_uint m_iCurWeaponAni{};


private:
	HRESULT Initialize(_float fAttackRange);

	void Attack(CLiDailin* pPlayer);
	void Chase(CLiDailin* pPlayer);

	_bool IsInAttackRange(CLiDailin* pPlayer);
	void ApplyDamage(CLiDailin* pPlayer);

public:
	static CLiDailinAttack* Create(_float fAttackRange);
protected:
	virtual void Free() override;
};

