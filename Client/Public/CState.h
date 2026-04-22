#pragma once

#include "Client_Defines.h"
#include "CBase.h"

#include "Command.h"

NS_BEGIN(Client)

class CLiDailin;

class CState abstract : public CBase
{
protected:
	CState();
	virtual ~CState() = default;

public:
	virtual void Enter(CLiDailin* pPlayer) = 0;
	virtual void Update(CLiDailin* pPlayer, _float fTimeDelta) = 0;
	virtual void Exit(CLiDailin* pPlayer) = 0;

protected:
	virtual void Free();
};

NS_END