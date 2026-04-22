#pragma once

#include "CActionState.h"

class CLiDailin_W : public CActionState
{
private:
	CLiDailin_W();
	virtual ~CLiDailin_W() = default;

private:
	HRESULT Initialize();

public:
	void Enter(CLiDailin* pPlayer) override;
	void Update(CLiDailin* pPlayer, _float fTimeDelta) override;
	void Exit(CLiDailin* pPlayer) override;
	void HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command) override;

private:
	static const _char* m_pBoneNames[];

public:
	static CLiDailin_W* Create();
protected:
	virtual void Free() override;
};

