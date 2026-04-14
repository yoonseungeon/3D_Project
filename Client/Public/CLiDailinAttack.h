#pragma once

#include "CState.h"

class CLiDailinAttack : public CState
{
private:
	CLiDailinAttack();
	virtual ~CLiDailinAttack() = default;

public:
	void Enter(CPlayer* pPlayer) override;
	void Update(CPlayer* pPlayer, _float fTimeDelta) override;
	void Exit(CPlayer* pPlayer) override;
	void HandleCommand(CPlayer* pPlayer, COMMAND& eCommand) override;

public:
	static CLiDailinAttack* Create();
protected:
	virtual void Free() override;
};

