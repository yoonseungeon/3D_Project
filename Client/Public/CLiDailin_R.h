#pragma once

#include "CSkillState.h"

class CLiDailin_R : public CSkillState
{
private:
	CLiDailin_R();
	virtual ~CLiDailin_R() = default;

private:
	HRESULT Initialize();

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

