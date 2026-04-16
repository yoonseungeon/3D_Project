#pragma once

#include "CState.h"

class CLiDailin_E : public CState
{
private:
	CLiDailin_E();
	virtual ~CLiDailin_E() = default;

public:
	void Enter(CPlayer* pPlayer) override;
	void Update(CPlayer* pPlayer, _float fTimeDelta) override;
	void Exit(CPlayer* pPlayer) override;
	void HandleCommand(CPlayer* pPlayer, COMMAND& eCommand) override;

private:
	_float m_fChanneling{};
	_bool m_bLock{};

public:
	static CLiDailin_E* Create();
protected:
	virtual void Free() override;
};

