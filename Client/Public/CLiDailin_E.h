#pragma once

#include "CActionState.h"

class CLiDailin_E : public CActionState
{
private:
	CLiDailin_E();
	virtual ~CLiDailin_E() = default;

public:
	void Enter(CPlayer* pPlayer) override;
	void Update(CPlayer* pPlayer, _float fTimeDelta) override;
	void Exit(CPlayer* pPlayer) override;
	void HandleActionCommand(CPlayer* pPlayer, ACTION_COMMAND& eAction_Command) override;

private:
	_float m_fChanneling{};
	_bool m_bLock{};

public:
	static CLiDailin_E* Create();
protected:
	virtual void Free() override;
};

