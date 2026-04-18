#pragma once

#include "CState.h"

class CLiDailin_W : public CState
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
	void HandleCommand(CPlayer* pPlayer, COMMAND& eCommand) override;

private:
	unordered_set<string> m_BoneNames;

public:
	static CLiDailin_W* Create();
protected:
	virtual void Free() override;
};

