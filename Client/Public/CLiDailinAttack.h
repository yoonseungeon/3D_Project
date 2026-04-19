#pragma once

#include "CActionState.h"

class CLiDailinAttack : public CActionState
{
private:
	CLiDailinAttack();
	virtual ~CLiDailinAttack() = default;

public:
	void Enter(CPlayer* pPlayer) override;
	void Update(CPlayer* pPlayer, _float fTimeDelta) override;
	void Exit(CPlayer* pPlayer) override;
	void HandleActionCommand(CPlayer* pPlayer, ACTION_COMMAND& eAction_Command) override;

private:
	_bool m_bChase{};

private:
	void Attack(CPlayer* pPlayer);
	void Chase(CPlayer* pPlayer);
	void DoAttackOrChase(CPlayer* pPlayer);

public:
	static CLiDailinAttack* Create();
protected:
	virtual void Free() override;
};

