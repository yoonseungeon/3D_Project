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

	virtual void OnCollision_Enter(const COLLISION_INFO& tCollision);
	virtual void OnCollision_Stay(const COLLISION_INFO& tCollision);
	virtual void OnCollision_Exit(const COLLISION_INFO& tCollision);

protected:
	virtual void Free();
};

NS_END