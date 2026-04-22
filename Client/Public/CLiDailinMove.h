#pragma once

#include "CMovementState.h"

class CLiDailinMove : public CMovementState
{
private:
	CLiDailinMove();
	virtual ~CLiDailinMove() = default;

public:
	void Enter(CLiDailin* pPlayer) override;
	void Update(CLiDailin* pPlayer, _float fTimeDelta) override;
	void Exit(CLiDailin* pPlayer) override;
	void HandleMovementCommand(CLiDailin* pPlayer, MOVEMENT_COMMAND& eMovement_Command) override;

public:
	static CLiDailinMove* Create();
protected:
	virtual void Free() override;
};

