#pragma once

#include "CState.h"

class CLiDailin_R : public CState
{
private:
	CLiDailin_R();
	virtual ~CLiDailin_R() = default;

public:
	void Enter(CPlayer* pPlayer) override;
	void Update(CPlayer* pPlayer, _float fTimeDelta) override;
	void Exit(CPlayer* pPlayer) override;
	void HandleCommand(CPlayer* pPlayer, COMMAND& eCommand) override;

private:
	_float m_fChanneling{};

	_bool bIsCol{};
	_bool bIsAniR2Changed{};

public:
	static CLiDailin_R* Create();
protected:
	virtual void Free() override;
};

