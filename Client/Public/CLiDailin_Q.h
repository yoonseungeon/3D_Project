#pragma once

#include "CActionState.h"

class CLiDailin_Q : public CActionState
{
private:
	CLiDailin_Q();
	virtual ~CLiDailin_Q() = default;

public:
	void Enter(CLiDailin* pPlayer) override;
	void Update(CLiDailin* pPlayer, _float fTimeDelta) override;
	void Exit(CLiDailin* pPlayer) override;
	void HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command) override;

private:
	_uint m_iPhase{};

public:
	static CLiDailin_Q* Create();
protected:
	virtual void Free() override;
};

