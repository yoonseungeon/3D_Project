#pragma once

#include "CActionState.h"

class CAction_Collect : public CActionState
{
private:
	CAction_Collect();
	virtual ~CAction_Collect() = default;

private:
	HRESULT Initialize(_uint iCollectAni);

public:
	void Enter(CLiDailin* pPlayer) override;
	void Update(CLiDailin* pPlayer, _float fTimeDelta) override;
	void Exit(CLiDailin* pPlayer) override;
	void HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command) override;

private:
	_uint m_iCollectAni{};

	_bool m_bIsInteract{};

	_float m_fMaxTime{};
	_float m_fAccTime{};

	_int m_iItemId{ -1 };

private:
	void MoveToPos(CLiDailin* pPlayer, _float3& vPos);
	void Move_OR_Interact(CLiDailin* pPlayer);

public:
	static CAction_Collect* Create(_uint iCollectAni);
protected:
	virtual void Free() override;
};

