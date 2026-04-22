#pragma once

#include "CMovementState.h"

class CLiDailinIdle : public CMovementState
{
private:
	CLiDailinIdle();
	virtual ~CLiDailinIdle() = default;

public:
	void Enter(CLiDailin* pPlayer) override;
	void Update(CLiDailin* pPlayer, _float fTimeDelta) override;
	void Exit(CLiDailin* pPlayer) override;
	void HandleMovementCommand(CLiDailin* pPlayer, MOVEMENT_COMMAND& eMovement_Command) override;

public:
	static CLiDailinIdle* Create();
protected:
	virtual void Free() override;
};

