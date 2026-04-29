#pragma once

#include "CUnit.h"

NS_BEGIN(Client)

class CEquipment;

class CAbstractPlayer abstract : public CUnit
{
public:
	struct CABSTRACTPLAYER_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		ITEM_TYPE eItemType{};
	};

protected:
	CAbstractPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAbstractPlayer(const CAbstractPlayer& Prototype);
	virtual ~CAbstractPlayer() = default;

protected:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	// ActionState
	virtual void Process_ActionCommand(ACTION_COMMAND& tAction_Command) = 0;

	//Inven
	void Try_Craft(_uint iItemIndex);

	// Equipment
	const CEquipment* Get_Equipment() const { return m_pEquipment; }
	CEquipment* Get_Equipment() { return m_pEquipment; }

public:
	// Cool;
	virtual COOL_INFO* Get_CoolInfo(const SKILL_SLOT eType);
	virtual _bool CanUseSkill(const SKILL_SLOT eType);

	// Skill Icon
	virtual void Get_SkillSlotType(const SKILL_SLOT eType, SKILL_DESC& tDesc);

protected:
	CEquipment* m_pEquipment{};

protected:
	virtual HRESULT Initialize_Skill();
	virtual HRESULT Initialize_State();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free() override;
};

NS_END