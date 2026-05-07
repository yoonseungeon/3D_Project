#pragma once

#include "CAbstractPlayer.h"

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
class CInGame_Manager;

class CLiDailin final : public CAbstractPlayer
{
public:
	enum LIDAILIN_COLLIDER { LIDAILINE_BODY, LIDAILIN_Q, LIDAILIN_E, LIDAILIN_R };

private:
	struct INTOXICATION_MANAGE
	{
		_bool bEnhancedBasicATK{};
		_bool bResetDecreaseTimer{};
		_float fDecreaseTime{};
	};

private:
	CLiDailin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLiDailin(const CLiDailin& Prototype);
	virtual ~CLiDailin() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OnCollision_Enter(const COLLISION_INFO& tCollision) override;
	virtual void OnCollision_Stay(const COLLISION_INFO& tCollision) override;
	virtual void OnCollision_Exit(const COLLISION_INFO& tCollision) override;

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
	virtual void Process_ActionCommand(ACTION_COMMAND& tAction_Command) override;
	virtual void DefaultProcess_ActionCommand(ACTION_COMMAND& tAction_Command) override;

	// Part Obj
	const CBody_Player* Get_BodyPlayer() const { return m_pBody; }
	CWeapon* Get_Weapon() { return m_pWeapon; }

	// Get_Com
	CMove* Get_MoveCom() { return m_pMoveCom; }

	// Cool
	virtual COOL_INFO* Get_CoolInfo(const SKILL_SLOT eType) override;
	virtual _bool CanUseSkill(const SKILL_SLOT eType) override;

	// Icon Info
	virtual void Get_SkillSlotType(const SKILL_SLOT eType, SKILL_DESC& tDesc) override;

	// MoveBlock
	void Set_MoveBlock(_bool bMoveBlock) { m_bMoveBlock = bMoveBlock; }
	_bool Get_MoveBlock() { return m_bMoveBlock; }

	// MoveAniBlock
	void Set_MovementAniBlock(_bool bAniBlock) { m_bAniBlock = bAniBlock; }
	_bool Get_MovementAniBlock() { return m_bAniBlock; }

	// Ani
	void Set_CurAni(LiDailin_Ani eLiDailinCurAni) { m_eLiDailinCurAni = eLiDailinCurAni; }
	const LiDailin_Ani& Get_CurAni() { return m_eLiDailinCurAni; }

	// Cancle
	_bool Get_CanMoveCancle() { return m_bCanMoveCancle; }
	void Set_CanMoveCancle(_bool MoveCanCancle) { m_bCanMoveCancle = MoveCanCancle; }

	// Intoxication
	void Set_EnhancedBasicATK(_bool bEnhanced) { tIntoxication_Manage.bEnhancedBasicATK = bEnhanced; }
	void Set_IntoxicationTimer(_bool bResetTimer) { tIntoxication_Manage.bResetDecreaseTimer = bResetTimer; }
	_bool Get_EnhancedBasicATK() { return tIntoxication_Manage.bEnhancedBasicATK; }

private:
	// Com
	CMove* m_pMoveCom{ nullptr };
	CNavigation* m_pNavigationCom{ nullptr };

	// Part Obj
	CBody_Player* m_pBody{};
	CWeapon* m_pWeapon{};

	// State
	unordered_map<wstring, CState*> m_States;

	CMovementState* m_pCurMovementState{};
	CMovementState* m_pWaitMovementState{};

	CActionState* m_pCurActionState{};
	CActionState* m_pWaitActionState{};

	// COMMAND
	MOVEMENT_COMMAND m_tCurMovementCommand{};
	ACTION_COMMAND m_tCurActionCommand{};

	// Block
	_bool m_bMoveBlock{};
	_bool m_bAniBlock{};
	_bool m_bActionEnd{};

	// Cancle
	_bool m_bCanMoveCancle{};

	// COOL
	STACK_COOL_INFO tQCool{};
	COOL_INFO tWCool{};
	COOL_INFO tECool{};
	COOL_INFO tRCool{};

	// Ani
	LiDailin_Ani m_eLiDailinCurAni{};

	// Intoxication
	INTOXICATION_MANAGE tIntoxication_Manage{};

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

	void Key_Input();

	// COOL
	void CoolTimer(_float fTimeDelta);

	virtual HRESULT Initialize_Stat() override;
	virtual HRESULT Initialize_Skill() override;
	virtual HRESULT Initialize_State() override;

	CActionState* Get_ActionState(const wstring& wstrState);

public:
	static CLiDailin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END