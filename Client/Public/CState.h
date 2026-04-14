#pragma once

#include "Client_Defines.h"
#include "CBase.h"

#include "Command.h"

NS_BEGIN(Client)

class CPlayer;

class CState abstract : public CBase
{
protected:
	CState();
	virtual ~CState() = default;

public:
	virtual void Enter(CPlayer* pPlayer) = 0;
	virtual void Update(CPlayer* pPlayer, _float fTimeDelta) = 0;
	virtual void Exit(CPlayer* pPlayer) = 0;
	virtual void HandleCommand(CPlayer* pPlayer, COMMAND& eCommand) = 0;

protected:
	virtual void Free();
};

NS_END