#pragma once

#include "CState.h"

class CLiDailinMove : public CState
{
private:
	CLiDailinMove();
	virtual ~CLiDailinMove() = default;

public:
	void Enter(CPlayer* pPlayer) override;
	void Update(CPlayer* pPlayer, _float fTimeDelta) override;
	void Exit(CPlayer* pPlayer) override;
	void HandleCommand(CPlayer* pPlayer, COMMAND& eCommand) override;

public:
	static CLiDailinMove* Create();
protected:
	virtual void Free() override;
};

