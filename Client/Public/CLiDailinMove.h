#pragma once

#include "CMovementState.h"

class CLiDailinMove : public CMovementState
{
private:
	CLiDailinMove();
	virtual ~CLiDailinMove() = default;

public:
	void Enter(CLiDailin* pPlayer) override;
	void Update(CLiDailin* pPlayer, _float fTimeDelta) override;
	void Exit(CLiDailin* pPlayer) override;
	void HandleMovementCommand(CLiDailin* pPlayer, MOVEMENT_COMMAND& eMovement_Command) override;


private:
	_float m_fFootStepRatios[2]{};;
	_bool m_bPlaySound[2]{};;

	_uint m_iFootStepIndex{};
	_uint m_iFootStepStartSoundKey{};
	_uint m_iFootStepSoundKeyCount{};

private:
	void Play_FootStepSound();

public:
	static CLiDailinMove* Create();
protected:
	virtual void Free() override;
};

