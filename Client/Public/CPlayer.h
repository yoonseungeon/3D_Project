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
class CState;

class CPlayer final : public CContainerObject
{
public:


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
	void Set_WaitState(wstring wstrState);
	void Set_CurCommand(COMMAND& tCommand) { m_tCurCommand = tCommand; }
	void Apply_WaitState();
	const COMMAND& Get_CurCommand() { return m_tCurCommand; }

	_bool IsTargetInRange();

	// Part Obj
	void Set_Animation(wstring wstrPartObjTag, _uint eAniIndex, _bool bLoop);
	_bool IsAnimationFinished(wstring wstrPartObjTag);

	void Set_AniInterpolationTime(wstring wstrPartObjTag, _float InterpolationTime);
	void Set_AniSpeed(wstring wstrPartObjTag, _uint iIndex, _float fAniSpeed);

	const CBody_Player* Get_BodyPlayer() const { return m_pBody; }

	// MoveCom
	_bool Update_Move_To_Pos(_float fTimeDelta);
	void Move_To_Pos(_float3 vPos, _bool bOperateNavi = false);
	void Stop_Move_To_Pos();

	// Cool
	STACK_COOL_INFO& Get_QCoolInfo() { return tQCool; }
	_bool CanUseQ();

	COOL_INFO* Get_CoolInfo(const _tchar* SkillName);
	_bool CanUseSkill(const _tchar* SkillName);

private:
	// Com
	CMove* m_pMoveCom{ nullptr };
	CNavigation* m_pNavigationCom{ nullptr };
	CCollider* m_pColliderCom = { nullptr };

	// Part Obj
	CBody_Player* m_pBody{};

	// State
	CState* m_pCurrentState{};
	CState* m_pWaitState{};

	unordered_map<wstring, CState*> m_States;

	// COMMAND
	COMMAND m_tCurCommand{};

	// COOL
	STACK_COOL_INFO tQCool{};
	COOL_INFO tECool{};
	COOL_INFO tRCool{};

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