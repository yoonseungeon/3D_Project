#pragma once

#include "Client_Defines.h"
#include "CContainerObject.h"

#include "Command.h"

NS_BEGIN(Engine)
class CMove;
class CNavigation;
class CCollider;
NS_END

NS_BEGIN(Client)

class CBody_Player;
class CWeapon;
class CState;
class CMovementState;
class CActionState;

class CPlayer final : public CContainerObject
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	// MovementState
	void Set_WaitMovementState(const wstring& wstrState);
	void Apply_WaitMovementState();
	void Set_CurMovementCommand(MOVEMENT_COMMAND& tMovement_Command) { m_tCurMovementCommand = tMovement_Command; }
	const MOVEMENT_COMMAND& Get_CurMovementCommand() { return m_tCurMovementCommand; }

	void Process_MovementCommand(MOVEMENT_COMMAND& tMovement_Command);

	// ActionState
	void Set_WaitActionState(const wstring& wstrState);
	void Apply_WaitActionState();
	void Set_CurActionCommand(ACTION_COMMAND& tAction_Command) { m_tCurActionCommand = tAction_Command; }
	const ACTION_COMMAND& Get_CurActionCommand() { return m_tCurActionCommand; }

	void Set_ActionEnd() { m_bActionEnd = true; }
	void Process_ActionCommand(ACTION_COMMAND& tAction_Command);

	// Part Obj
	const CBody_Player* Get_BodyPlayer() const { return m_pBody; }
	CWeapon* Get_Weapon() { return m_pWeapon; }

	// MoveCom
	_bool Update_Move_To_Pos(_float fTimeDelta);
	void Move_To_Pos(_float3 vPos, _bool bOperateNavi = false);
	void Stop_Move_To_Pos();

	// Cool
	STACK_COOL_INFO& Get_QCoolInfo() { return tQCool; }

	COOL_INFO* Get_CoolInfo(const _tchar* SkillName);
	_bool CanUseSkill(const _tchar* SkillName);

	// MoveBlock
	void Set_MoveBlock(_bool bMoveBlock) { m_bMoveBlock = bMoveBlock; }
	_bool Get_MoveBlock() { return m_bMoveBlock; }
	void Set_AniBlock(_bool bAniBlock) { m_bAniBlock = bAniBlock; }
	_bool Get_AniBlock() { return m_bAniBlock; }
	void Set_CurAni(LiDailin_Ani eLiDailinCurAni) { m_eLiDailinCurAni = eLiDailinCurAni; }
	const LiDailin_Ani& Get_CurAni() { return m_eLiDailinCurAni; }

	// Cancle
	_bool Get_CanMoveCancle() { return m_bCanMoveCancle; }
	void Set_CanMoveCancle(_bool MoveCanCancle) { m_bCanMoveCancle = MoveCanCancle; }

private:
	// Com
	CMove* m_pMoveCom{ nullptr };
	CNavigation* m_pNavigationCom{ nullptr };
	CCollider* m_pColliderCom = { nullptr };

	// Part Obj
	CBody_Player* m_pBody{};
	CWeapon* m_pWeapon{};

	// State
	CMovementState* m_pCurMovementState{};
	CMovementState* m_pWaitMovementState{};

	CActionState* m_pCurActionState{};
	CActionState* m_pWaitActionState{};

	unordered_map<wstring, CState*> m_States;

	// COMMAND
	MOVEMENT_COMMAND m_tCurMovementCommand{};
	ACTION_COMMAND m_tCurActionCommand{};

	// COOL
	STACK_COOL_INFO tQCool{};
	COOL_INFO tWCool{};
	COOL_INFO tECool{};
	COOL_INFO tRCool{};

	// Block
	_bool m_bMoveBlock{};
	_bool m_bAniBlock{};
	LiDailin_Ani m_eLiDailinCurAni{};
	_bool m_bActionEnd{};

	// Cancle
	_bool m_bCanMoveCancle{};

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

	HRESULT Ready_Layer_UI_Image(const _wstring& strLayerTag);

	void Key_Input();

	// COOL
	void CoolTimer(_float fTimeDelta);

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END