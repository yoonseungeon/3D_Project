#pragma once

#include "Client_Defines.h"
#include "CState.h"

NS_BEGIN(Client)

class CLiDailin;

class CMovementState abstract : public CState
{
protected:
	CMovementState();
	virtual ~CMovementState() = default;

public:
	virtual void Enter(CLiDailin* pPlayer) = 0;
	virtual void Update(CLiDailin* pPlayer, _float fTimeDelta) = 0;
	virtual void Exit(CLiDailin* pPlayer) = 0;
	virtual void HandleMovementCommand(CLiDailin* pPlayer, MOVEMENT_COMMAND& eMovement_Command) = 0;

protected:
	virtual void Free();
};

NS_END