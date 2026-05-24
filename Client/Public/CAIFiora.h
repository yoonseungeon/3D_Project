#pragma once

#include "CAbstractMonster.h"
#include "CBody_Fiora.h"

NS_BEGIN(Engine)
class CMove;
class CNavigation;
class CCollider;
NS_END

NS_BEGIN(Client)

class CInGame_Manager;
class CFiora_Q_1;

class CAIFiora : public CAbstractMonster
{
public:
	struct AIFIORA_DESC : public CAbstractMonster::CABSTRACTMONSTER_DESC
	{
	};

private:
	enum AIFIORA_ACTION { WAIT, CHASE, Q, W, E, E_ATK, R, NORMAL_ATK, DEAD, STUN, END_DECO };

	enum AIFIORA_COLLIDER { AIFIORA_BODY, AIFIORA_Q, AIFIORA_W1, AIFIORA_W2, AIFIORA_W3, AIFIORA_E, AIFIORA_R};

	enum AIFIORA_CONDITION {
		SKILL_E_COL = 1 << 0,
		SKILL_R_COL = 1 << 1,
		IS_COL_PLAYER = 1 << 2,
	};

private:
	CAIFiora(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAIFiora(const CAIFiora& Prototype);
	virtual ~CAIFiora() = default;

protected:
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

	void Enter_Animation(CBody_Fiora::FIORA_ANI eNewAnimation);
	void Update_Action(_float fTimeDelta);
	void Enter_Action(AIFIORA_ACTION eNewAction);
	void Execute_Action(_float fTimeDelta);

private:
	// Com
	CMove* m_pMoveCom{ nullptr };
	CNavigation* m_pNavigationCom{ nullptr };

	// Part
	CBody_Fiora* m_pBodyFiora{};

	AIFIORA_ACTION m_eCurState{ END_DECO };
	AIFIORA_ACTION m_ePreState{ END_DECO };

	CBody_Fiora::FIORA_ANI m_eCurAni{};

	CInGame_Manager* m_pInGame_Manager{};

	_bool m_bIsArrive{};
	_uint m_iMovePosIndex{};
	_float3 m_vMovePos[2]{};

	// Cool
	COOL_INFO tQCool{};
	COOL_INFO tWCool{};
	STACK_COOL_INFO tECool{};
	STACK_COOL_INFO tRCool{};

	vector<_float> m_SkillRange{};

	_uint m_iCondition{};

	unordered_set<CGameObject*> m_AttackedWEnemy{};

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

	virtual HRESULT Initialize_Skill();
	virtual HRESULT Initialize_Stat() override;

	void WaitStateMove();
	void CoolTimer(_float fTimeDelta);

	_bool CanUseSkill(const SKILL_SLOT eType);
	_bool CanUseAnythingSkill();
	_bool Update_Chase(_float fTimeDelta);

	_bool Choose_UseSkill(_float fLength);

	void LookTargetDir();

	_bool Get_TargetDistance(_float& Length);

	_uint m_iNormalATKCount{};

	CFiora_Q_1* pQEffect{};

public:
	static CAIFiora* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END