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
	void Enter(CPlayer* pPlayer) override;
	void Update(CPlayer* pPlayer, _float fTimeDelta) override;
	void Exit(CPlayer* pPlayer) override;
	void HandleActionCommand(CPlayer* pPlayer, ACTION_COMMAND& eAction_Command) override;

private:
	static const _char* m_pBoneNames[];

public:
	static CLiDailin_W* Create();
protected:
	virtual void Free() override;
};

