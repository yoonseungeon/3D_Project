#pragma once

#include "CActionState.h"

class CLiDailin_Q : public CActionState
{
private:
	CLiDailin_Q();
	virtual ~CLiDailin_Q() = default;

public:
	void Enter(CPlayer* pPlayer) override;
	void Update(CPlayer* pPlayer, _float fTimeDelta) override;
	void Exit(CPlayer* pPlayer) override;
	void HandleActionCommand(CPlayer* pPlayer, ACTION_COMMAND& eAction_Command) override;

private:
	_uint m_iPhase{};

public:
	static CLiDailin_Q* Create();
protected:
	virtual void Free() override;
};

