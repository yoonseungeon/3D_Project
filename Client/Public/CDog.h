#pragma once

#include "CAbstractMonster.h"
#include "CBody_Dog.h"

NS_BEGIN(Engine)
class CMove;
class CNavigation;
class CCollider;
NS_END

NS_BEGIN(Client)
class CDog : public CAbstractMonster
{
public:
	struct DOG_DESC : public CAbstractMonster::CABSTRACTMONSTER_DESC
	{
	};

private:
	enum DOG_ACTION { WAIT, RUN, ENDBATTLE, DYING, DEATH, DANCE, BEWARE_START, BEWARE_LOOP, BEWARE_END, ATK, APPEAR, RETURN, SLEEP_START, SLEEP, SLEEP_END, END };

private:
	CDog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDog(const CDog& Prototype);
	virtual ~CDog() = default;

protected:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Enter_Animation(CBody_Dog::DOG_ANI eNewAnimation);
	void Update_Action(_float fTimeDelta);
	void Enter_Action(DOG_ACTION eNewAction);
	void Execute_Action(_float fTimeDelta);

private:
	// Com
	CMove* m_pMoveCom{ nullptr };
	CNavigation* m_pNavigationCom{ nullptr };

	// Part
	CBody_Dog* m_pBodyDog{};

	DOG_ACTION m_eCurState{ END };
	DOG_ACTION m_ePreState{ END };

	CBody_Dog::DOG_ANI m_eCurAni{};

	_uint m_iAttackIndex{};

	_bool m_bIsCried{};

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

	void Run_OR_ATTACK();

	virtual HRESULT Initialize_Stat() override;

public:
	static CDog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END