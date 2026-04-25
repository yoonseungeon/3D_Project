#pragma once

#include "Client_Defines.h"
#include "CInvenOwner.h"

NS_BEGIN(Client)

class CUnit abstract : public CInvenOwner
{
public:
	struct UNIT_STAT
	{
		_ubyte iLevel{};//최대 o
		_ushort iEXP{}; //최대 o 최대가 변함

		_ushort iHP{};//최대 o 최대가 변함
		_ushort iMP{};//최대 o 최대가 변함

		_float fHPRecovery{};
		_float fMPRecovery{};

		_ushort iATKPower{};
		_ushort iSkillAmp{};
		_ushort iPenetrationDefense{}; _ushort iPenetrationDefensePercent{};
		_ushort iDefense{};
		_float fATKSpeed{};
		_ushort iCoolDown{};
		_ushort iCritical{};
		_float fSpeed{};

		UNIT_STAT& operator += (const UNIT_STAT& rhs) {
			iLevel += rhs.iLevel;
			iEXP += rhs.iEXP;

			iHP += rhs.iHP;
			iMP += rhs.iMP;
			fHPRecovery += rhs.fHPRecovery;
			fMPRecovery += rhs.fMPRecovery;

			iATKPower += rhs.iATKPower;
			iSkillAmp += rhs.iSkillAmp;
			iPenetrationDefense += rhs.iPenetrationDefense;
			iPenetrationDefensePercent += rhs.iPenetrationDefensePercent;
			iDefense += rhs.iDefense;
			fATKSpeed += rhs.fATKSpeed;
			iCoolDown += rhs.iCoolDown;
			iCritical += rhs.iCritical;
			fSpeed += rhs.fSpeed;
			return *this;
		}
	};

protected:
	CUnit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUnit(const CUnit& Prototype);
	virtual ~CUnit() = default;

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
	const UNIT_STAT& Get_FinalStat() { return m_tFinalStat; }
	const UNIT_STAT& Get_CurStat() { return m_tCurStat; }

	void AddEXP(_uint iEXP);
	void AddHP(_uint iHP);
	void AddMP(_uint iMP);

	const MAINGAUGE_TYPE& Get_MPType() { return m_eMPType; }

protected:
	void LevelUp();

	void SetStat(UNIT_STAT& tStat,
		_ubyte iLevel, _ushort iExp, _ushort iHp, _ushort iMp,
		_float fHpRec, _float fMpRec, _ushort iATK, _ushort iSkillAmp,
		_ushort iPen, _ushort iPenPct, _ushort iDef,
		_float fATKSpeed, _ushort iCool, _ushort iCrit, _float fSpeed);

	void SetFinalStat();

protected:
	UNIT_STAT m_tFinalStat{}; // Base와 Item의 합산
	UNIT_STAT m_tBaseStat{};

	UNIT_STAT m_tItemStat{};

	UNIT_STAT m_tLevelUpStat{}; // 레벨업 할 때 base와 cur에 더해줘야 할 값

	UNIT_STAT m_tCurStat{};

	MAINGAUGE_TYPE m_eMPType{};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free() override;
};

NS_END