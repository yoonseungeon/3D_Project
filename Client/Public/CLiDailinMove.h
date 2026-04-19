#pragma once

#include "CMovementState.h"

class CLiDailinMove : public CMovementState
{
private:
	CLiDailinMove();
	virtual ~CLiDailinMove() = default;

public:
	void Enter(CPlayer* pPlayer) override;
	void Update(CPlayer* pPlayer, _float fTimeDelta) override;
	void Exit(CPlayer* pPlayer) override;
	void HandleMovementCommand(CPlayer* pPlayer, MOVEMENT_COMMAND& eMovement_Command) override;

public:
	static CLiDailinMove* Create();
protected:
	virtual void Free() override;
};

