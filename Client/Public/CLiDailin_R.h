#pragma once

#include "CActionState.h"

class CLiDailin_R : public CActionState
{
private:
	CLiDailin_R();
	virtual ~CLiDailin_R() = default;

public:
	void Enter(CLiDailin* pPlayer) override;
	void Update(CLiDailin* pPlayer, _float fTimeDelta) override;
	void Exit(CLiDailin* pPlayer) override;
	void HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command) override;

private:
	_float m_fChanneling{};

	_bool bIsCol{};
	_bool bIsAniR2Changed{};

	_bool m_bEnhanced{};

public:
	static CLiDailin_R* Create();
protected:
	virtual void Free() override;
};

