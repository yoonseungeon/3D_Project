#pragma once

#include "CActionState.h"

class CAction_Craft : public CActionState
{
private:
	CAction_Craft();
	virtual ~CAction_Craft() = default;

private: 
	HRESULT Initialize(_uint iCurAni);

public:
	void Enter(CLiDailin* pPlayer) override;
	void Update(CLiDailin* pPlayer, _float fTimeDelta) override;
	void Exit(CLiDailin* pPlayer) override;
	void HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command) override;

private:
	_uint m_iCurAni{};

	_float m_fMaxTime{};
	_float m_fAccTime{};

	_int m_iItemIndex{ -1 };

public:
	static CAction_Craft* Create(_uint iCurAni);
protected:
	virtual void Free() override;
};

