#pragma once

#include "CAbstractMonster.h"
#include "CBody_Boar.h"

NS_BEGIN(Engine)
class CMove;
class CNavigation;
class CCollider;
NS_END

NS_BEGIN(Client)

class CBoar : public CAbstractMonster
{
public:
	struct BOAR_DESC : public CAbstractMonster::CABSTRACTMONSTER_DESC
	{
	};

private:
	enum BOAR_ACTION { WAIT, RUN, ENDBATTLE, DYING, DEATH, DANCE, BEWARE_START, BEWARE_LOOP, BEWARE_END, ATK, APPEAR, RETURN, END_DECO };

private:
	CBoar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoar(const CBoar& Prototype);
	virtual ~CBoar() = default;

protected:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Enter_Animation(CBody_Boar::BOAR_ANI eNewAnimation);
	void Update_Action(_float fTimeDelta);
	void Enter_Action(BOAR_ACTION eNewAction);
	void Execute_Action(_float fTimeDelta);

private:
	// Com
	CMove* m_pMoveCom{ nullptr };
	CNavigation* m_pNavigationCom{ nullptr };

	// Part
	CBody_Boar* m_pBodyBoar{};

	BOAR_ACTION m_eCurState{ END_DECO };
	BOAR_ACTION m_ePreState{ END_DECO };

	CBody_Boar::BOAR_ANI m_eCurAni{};

	_uint m_iAttackIndex{};

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

	void Run_OR_ATTACK();

	virtual HRESULT Initialize_Stat() override;

public:
	static CBoar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END