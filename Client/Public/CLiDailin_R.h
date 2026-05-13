#pragma once

#include "CSkillState.h"

class CLiDailin_R : public CSkillState
{
private:
	enum R_ATK {
		RESET = 0,
		GIVE_R2 = 1 << 0,
		GIVE_R3 = 1 << 1,
		GIVE_R4 = 1 << 2,
		END_R2 = 1 << 3,
		END_R3 = 1 << 4,
		END_R4 = 1 << 5
	};

private:
	CLiDailin_R();
	virtual ~CLiDailin_R() = default;

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
	_float m_fChanneling{};

	_bool bIsCol{};
	_bool bIsAniRNextChanged{};

	_bool m_bEnhanced{};

	_float fADRatio{};

	_int m_iConsumeIntoxication{};

	_uint m_eRATKP{};
	CGameObject* m_pTempCompareTarget{};  //이걸로 참조x

private:
	void Give_Damage(const COLLISION_INFO& tCollision, _bool bStun =  false);

public:
	static CLiDailin_R* Create();
protected:
	virtual void Free() override;
};

