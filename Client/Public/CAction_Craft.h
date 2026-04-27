#pragma once

#include "CActionState.h"

class CAction_Craft : public CActionState
{
private:
	CAction_Craft();
	virtual ~CAction_Craft() = default;

public:
	void Enter(CLiDailin* pPlayer) override;
	void Update(CLiDailin* pPlayer, _float fTimeDelta) override;
	void Exit(CLiDailin* pPlayer) override;
	void HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command) override;

public:
	static CAction_Craft* Create();
protected:
	virtual void Free() override;
};

