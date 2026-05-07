#pragma once

#include "CActionState.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)

class CLiDailin_Q : public CActionState
{
private:
	CLiDailin_Q();
	virtual ~CLiDailin_Q() = default;

public:
	void Enter(CLiDailin* pPlayer) override;
	void Update(CLiDailin* pPlayer, _float fTimeDelta) override;
	void Exit(CLiDailin* pPlayer) override;
	void HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command) override;

	virtual void OnCollision_Enter(const COLLISION_INFO& tCollision) override;
	virtual void OnCollision_Stay(const COLLISION_INFO& tCollision) override;
	virtual void OnCollision_Exit(const COLLISION_INFO& tCollision) override;

private:
	_uint m_iPhase{};

	unordered_set<CGameObject*> m_AttackedObj;

	_bool m_bEnhanced{};

public:
	static CLiDailin_Q* Create();
protected:
	virtual void Free() override;
};

NS_END