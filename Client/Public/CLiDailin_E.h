#pragma once

#include "CSkillState.h"

class CLiDailin_E : public CSkillState
{
private:
	CLiDailin_E();
	virtual ~CLiDailin_E() = default;

private:
	HRESULT Initialize();

public:
	void Enter(CLiDailin* pPlayer) override;
	void Update(CLiDailin* pPlayer, _float fTimeDelta) override;
	void Exit(CLiDailin* pPlayer) override;
	void HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command) override;

	virtual void OnCollision_Enter(const COLLISION_INFO& tCollision) override;
	virtual void OnCollision_Stay(const COLLISION_INFO& tCollision) override;
	virtual void OnCollision_Exit(const COLLISION_INFO& tCollision) override;

private:
	_float	m_fChanneling{};
	_float	m_fMaxChanneling{};

	_bool	m_bCancleLock{};

	unordered_set<CGameObject*> m_AttackedObj;

	_bool m_bEnhanced{};

public:
	static CLiDailin_E* Create();
protected:
	virtual void Free() override;
};

