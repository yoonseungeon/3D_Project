#pragma once

#include "CAbstractMonster.h"
#include "CBody_Bat.h"

NS_BEGIN(Engine)
class CMove;
class CNavigation;
class CCollider;
NS_END

NS_BEGIN(Client)

class CBody_Bat;

class CBat : public CAbstractMonster
{
public:
	struct BAT_DESC : public CAbstractMonster::CABSTRACTMONSTER_DESC
	{
	};

private:
	enum BAT_ACTION { WAIT, RUN, ENDBATTLE, DYING, DEATH, BEWARE_START, BEWARE_LOOP, BEWARE_END, ATK, APPEAR, RETURN, END_DECO };

private:
	CBat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBat(const CBat& Prototype);
	virtual ~CBat() = default;

protected:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Enter_Animation(CBody_Bat::BAT_ANI eNewAnimation);
	void Update_Action(_float fTimeDelta);
	void Enter_Action(BAT_ACTION eNewAction);
	void Execute_Action(_float fTimeDelta);

private:
	// Com
	CMove* m_pMoveCom{ nullptr };
	CNavigation* m_pNavigationCom{ nullptr };

	// Part
	CBody_Bat* m_pBodyBat{};

	BAT_ACTION m_eCurState{ END_DECO };
	BAT_ACTION m_ePreState{ END_DECO };

	CBody_Bat::BAT_ANI m_eCurAni{};

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

	void Run_OR_ATTACK();

	virtual HRESULT Initialize_Stat() override;

public:
	static CBat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END