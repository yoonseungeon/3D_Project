#pragma once

#include "CActionState.h"

class CLiDailin_E : public CActionState
{
private:
	CLiDailin_E();
	virtual ~CLiDailin_E() = default;

public:
	void Enter(CLiDailin* pPlayer) override;
	void Update(CLiDailin* pPlayer, _float fTimeDelta) override;
	void Exit(CLiDailin* pPlayer) override;
	void HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command) override;

private:
	_float	m_fChanneling{};
	_bool	m_bCancleLock{};

public:
	static CLiDailin_E* Create();
protected:
	virtual void Free() override;
};

