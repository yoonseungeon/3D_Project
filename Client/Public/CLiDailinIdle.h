#pragma once

#include "CState.h"

class CLiDailinIdle : public CState
{
private:
	CLiDailinIdle();
	virtual ~CLiDailinIdle() = default;

public:
	void Enter(CPlayer* pPlayer) override;
	void Update(CPlayer* pPlayer, _float fTimeDelta) override;
	void Exit(CPlayer* pPlayer) override;
	void HandleCommand(CPlayer* pPlayer, COMMAND& eCommand) override;

public:
	static CLiDailinIdle* Create();
protected:
	virtual void Free() override;
};

