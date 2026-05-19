#pragma once

#include "CAbstractMonster.h"
#include "CBody_Bear.h"

NS_BEGIN(Engine)
class CMove;
class CNavigation;
class CCollider;
NS_END

NS_BEGIN(Client)
class CBear : public CAbstractMonster
{
public:
	struct BEAR_DESC : public CAbstractMonster::CABSTRACTMONSTER_DESC
	{
	};

private:
	enum BEAR_ACTION { WAIT, RUN, ENDBATTLE, DYING, DEATH, DANCE, BEWARE_START, BEWARE_LOOP, BEWARE_END, ATK, APPEAR, RETURN, SLEEP_START, SLEEP, SLEEP_END, END_DECO };

private:
	CBear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBear(const CBear& Prototype);
	virtual ~CBear() = default;

protected:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Enter_Animation(CBody_Bear::BEAR_ANI eNewAnimation);
	void Update_Action(_float fTimeDelta);
	void Enter_Action(BEAR_ACTION eNewAction);
	void Execute_Action(_float fTimeDelta);

private:
	// Com
	CMove* m_pMoveCom{ nullptr };
	CNavigation* m_pNavigationCom{ nullptr };

	// Part
	CBody_Bear* m_pBodyWolf{};

	BEAR_ACTION m_eCurState{ END_DECO };
	BEAR_ACTION m_ePreState{ END_DECO };

	CBody_Bear::BEAR_ANI m_eCurAni{};

	_uint m_iAttackIndex{};

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

	void Run_OR_ATTACK();

	virtual HRESULT Initialize_Stat() override;

public:
	static CBear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END