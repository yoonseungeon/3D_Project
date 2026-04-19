#pragma once

#include "Client_Defines.h"
#include "CState.h"

NS_BEGIN(Client)

class CPlayer;

class CMovementState abstract : public CState
{
protected:
	CMovementState();
	virtual ~CMovementState() = default;

public:
	virtual void Enter(CPlayer* pPlayer) = 0;
	virtual void Update(CPlayer* pPlayer, _float fTimeDelta) = 0;
	virtual void Exit(CPlayer* pPlayer) = 0;
	virtual void HandleMovementCommand(CPlayer* pPlayer, MOVEMENT_COMMAND& eMovement_Command) = 0;

protected:
	virtual void Free();
};

NS_END