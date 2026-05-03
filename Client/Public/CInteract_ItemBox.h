#pragma once

#include "CActionState.h"

class CInteract_ItemBox : public CActionState
{
private:
	CInteract_ItemBox();
	virtual ~CInteract_ItemBox() = default;

private:
	HRESULT Initialize();

public:
	void Enter(CLiDailin* pPlayer) override;
	void Update(CLiDailin* pPlayer, _float fTimeDelta) override;
	void Exit(CLiDailin* pPlayer) override;
	void HandleActionCommand(CLiDailin* pPlayer, ACTION_COMMAND& eAction_Command) override;

private:
	_bool m_bIsOpenUI{};

private:
	void MoveToPos(CLiDailin* pPlayer, _float3& vPos);
	void Move_OR_Interact(CLiDailin* pPlayer);

public:
	static CInteract_ItemBox* Create();
protected:
	virtual void Free() override;
};

