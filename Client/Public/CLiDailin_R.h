#pragma once

#include "CActionState.h"

class CLiDailin_R : public CActionState
{
private:
	CLiDailin_R();
	virtual ~CLiDailin_R() = default;

public:
	void Enter(CPlayer* pPlayer) override;
	void Update(CPlayer* pPlayer, _float fTimeDelta) override;
	void Exit(CPlayer* pPlayer) override;
	void HandleActionCommand(CPlayer* pPlayer, ACTION_COMMAND& eAction_Command) override;

private:
	_float m_fChanneling{};

	_bool bIsCol{};
	_bool bIsAniR2Changed{};

public:
	static CLiDailin_R* Create();
protected:
	virtual void Free() override;
};

