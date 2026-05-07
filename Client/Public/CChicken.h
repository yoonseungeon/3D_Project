#pragma once

#include "CAbstractMonster.h"
#include "CBody_Chicken.h"

NS_BEGIN(Engine)
class CMove;
class CNavigation;
class CCollider;
NS_END

NS_BEGIN(Client)

class CBody_Chicken;
class CBody_Chicken;

class CChicken : public CAbstractMonster
{
public:
	struct CHICKEN_DESC : public CAbstractMonster::CABSTRACTMONSTER_DESC
	{
	};

private:
	enum CHICKEN_ACTION { WAIT, RUN, ENDBATTLE, DYING, DEATH, DANCE, BEWARE_START, BEWARE_LOOP, BEWARE_END, ATK, APPEAR, RETURN, END };

private:
	CChicken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChicken(const CChicken& Prototype);
	virtual ~CChicken() = default;

protected:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Enter_Animation(CBody_Chicken::CHICKEN_ANI eNewAnimation);
	void Update_Action(_float fTimeDelta);
	void Enter_Action(CHICKEN_ACTION eNewAction);
	void Execute_Action(_float fTimeDelta);

private:
	// Com
	CMove* m_pMoveCom{ nullptr };
	CNavigation* m_pNavigationCom{ nullptr };

	// Part
	CBody_Chicken* m_pBodyChicken{};

	CHICKEN_ACTION m_eCurState{ END };
	CHICKEN_ACTION m_ePreState{ END };

	CBody_Chicken::CHICKEN_ANI m_eCurAni{};

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

	void Run_OR_ATTACK();

	virtual HRESULT Initialize_Stat() override;

public:
	static CChicken* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END