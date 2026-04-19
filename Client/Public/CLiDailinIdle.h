#pragma once

#include "CMovementState.h"

class CLiDailinIdle : public CMovementState
{
private:
	CLiDailinIdle();
	virtual ~CLiDailinIdle() = default;

public:
	void Enter(CPlayer* pPlayer) override;
	void Update(CPlayer* pPlayer, _float fTimeDelta) override;
	void Exit(CPlayer* pPlayer) override;
	void HandleMovementCommand(CPlayer* pPlayer, MOVEMENT_COMMAND& eMovement_Command) override;

public:
	static CLiDailinIdle* Create();
protected:
	virtual void Free() override;
};

