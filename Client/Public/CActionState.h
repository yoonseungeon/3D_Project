#pragma once

#include "Client_Defines.h"
#include "CState.h"

NS_BEGIN(Client)

class CLiDailin;

class CActionState abstract : public CState
{
protected:
	CActionState();
	virtual ~CActionState() = default;

public:
	virtual void Enter(CLiDailin* pPlayer) = 0;
	virtual void Update(CLiDailin* pPlayer, _float fTimeDelta) = 0;
	virtual void Exit(CLiDailin* pPlayer) = 0;
	virtual void HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command) = 0;

protected:
	virtual void Free();
};

NS_END