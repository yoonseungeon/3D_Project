#pragma once

#include "CUnit.h"

#include "Command.h"

NS_BEGIN(Engine)
class CNavigation;
NS_END

NS_BEGIN(Client)

class CEquipment;
class CCraftList;

class CCraftTool;
class CCraftHammer;
class CBurner;
class CFryingPan;

class CSkillState;

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
	virtual void DefaultProcess_ActionCommand(ACTION_COMMAND& tAction_Command) = 0;

	void Try_Craft(_uint iItemId);

	// Begin 동기화 필요 함수들
	// Inventory, Equipment
	_bool Use_Inventory(_uint iSlotIndex);
	_bool Unequip(_uint iSlotIndex);

	// Inven, Equipment, CraftList
	_bool TryEquip_AddInven(_int iItemId, _uint iItemCount = 1);

	// CraftList
	_bool Craft_Item(_int iItemId);
	// End

	// Equipment
	const CEquipment* Get_Equipment() const { return m_pEquipment; }
	CEquipment* Get_Equipment() { return m_pEquipment; }

	// CraftList
	const CCraftList* Get_CraftList() const { return m_pCraftList; }
	CCraftList* Get_CraftList() { return m_pCraftList; }

	// SkillCool;
	virtual COOL_INFO* Get_CoolInfo(const SKILL_SLOT eType);
	virtual _bool CanUseSkill(const SKILL_SLOT eType);

	// Skill Icon
	virtual void Get_SkillSlotType(const SKILL_SLOT eType, SKILL_DESC& tDesc);

	virtual _uint Get_MaxSkillLevel(const SKILL_SLOT eType) = 0;
	virtual _uint Get_CurSkillLevel(const SKILL_SLOT eType) = 0;
	virtual void LevelUpSkill(const SKILL_SLOT eType) = 0;
	virtual _bool CanLevelUpSkill(const SKILL_SLOT eType) = 0;


	// Part Obj
	CCraftTool* Get_CraftTool() { return m_pCraftTool; }
	CCraftHammer* Get_CraftHammer() { return m_pCraftHammer; }
	CBurner* Get_Burner() { return m_pBurner; }
	CFryingPan* Get_FryingPan() { return m_pFryingPan; }

	void Look_MouseDir();

protected:
	CEquipment* m_pEquipment{};
	CCraftList* m_pCraftList{};

	// Part Obj
	CCraftTool* m_pCraftTool{};
	CCraftHammer* m_pCraftHammer{};
	CBurner* m_pBurner{};
	CFryingPan* m_pFryingPan{};

	CNavigation* m_pNavigationCom{ nullptr };

	_int m_iCurAreaIndex{ -1 };
	_float m_fSoundLoopGap{};
	_float m_fAccSoundLoopTime{};

protected:
	virtual HRESULT Initialize_Skill();
	virtual HRESULT Initialize_State();

	// 동기화 필요 함수
	_bool Equip(_uint iSlotIndex);
	_bool Use_Consumable(_uint iSlotIndex);

	void SetItemStat(_int iItemId, _bool bAdd);

	virtual CSkillState* FindSkill(const SKILL_SLOT eType) = 0;

	void Update_GroundSound(_float fTimeDelta);
	void Choose_Area(SOUND_KEY& eSoundKey);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free() override;
};

NS_END