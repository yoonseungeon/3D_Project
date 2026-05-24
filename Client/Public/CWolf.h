#pragma once

#include "CAbstractMonster.h"
#include "CBody_Wolf.h"

NS_BEGIN(Engine)
class CMove;
class CNavigation;
class CCollider;
NS_END

NS_BEGIN(Client)
class CWolf : public CAbstractMonster
{
public:
	struct WOLF_DESC : public CAbstractMonster::CABSTRACTMONSTER_DESC
	{
	};

private:
	enum WOLF_ACTION { WAIT, RUN, ENDBATTLE, DYING, DEATH, DANCE, BEWARE_START, BEWARE_LOOP, BEWARE_END, ATK, APPEAR, RETURN, SLEEP_START, SLEEP, SLEEP_END, END_DECO };

private:
	CWolf(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWolf(const CWolf& Prototype);
	virtual ~CWolf() = default;

protected:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Enter_Animation(CBody_Wolf::WOLF_ANI eNewAnimation);
	void Update_Action(_float fTimeDelta);
	void Enter_Action(WOLF_ACTION eNewAction);
	void Execute_Action(_float fTimeDelta);

private:
	// Com
	CMove* m_pMoveCom{ nullptr };
	CNavigation* m_pNavigationCom{ nullptr };

	// Part
	CBody_Wolf* m_pBodyWolf{};

	WOLF_ACTION m_eCurState{ END_DECO };
	WOLF_ACTION m_ePreState{ END_DECO };

	CBody_Wolf::WOLF_ANI m_eCurAni{};

	_uint m_iAttackIndex{};

	_bool m_bIsCried{};

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

	void Run_OR_ATTACK();

	virtual HRESULT Initialize_Stat() override;

public:
	static CWolf* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END