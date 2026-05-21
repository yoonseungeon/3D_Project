#pragma once

#include "CSkillState.h"

class CLiDailin_W : public CSkillState
{
private:
	CLiDailin_W();
	virtual ~CLiDailin_W() = default;

private:
	HRESULT Initialize();

public:
	void Enter(CLiDailin* pPlayer) override;
	void Update(CLiDailin* pPlayer, _float fTimeDelta) override;
	void Exit(CLiDailin* pPlayer) override;
	void HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command) override;

private:
	static const _char* m_pBoneNames[];

	_uint m_iMaxDrinkCount{};
	_uint m_iRemainDrinkCount{};
	_float m_fAccDrink{};
	_float m_fMaxDrink{};

	_uint m_iSoundCount{};

public:
	static CLiDailin_W* Create();
protected:
	virtual void Free() override;
};

