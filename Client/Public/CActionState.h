#pragma once

#include "Client_Defines.h"
#include "CState.h"

NS_BEGIN(Client)

class CPlayer;

class CActionState abstract : public CState
{
protected:
	CActionState();
	virtual ~CActionState() = default;

public:
	virtual void Enter(CPlayer* pPlayer) = 0;
	virtual void Update(CPlayer* pPlayer, _float fTimeDelta) = 0;
	virtual void Exit(CPlayer* pPlayer) = 0;
	virtual void HandleActionCommand(CPlayer* pPlayer, ACTION_COMMAND& eAction_Command) = 0;

protected:
	virtual void Free();
};

NS_END