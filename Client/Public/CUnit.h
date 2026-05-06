#pragma once

#include "Client_Defines.h"
#include "CInvenOwner.h"

NS_BEGIN(Client)

class CUnit abstract : public CInvenOwner
{
public:
	struct UNIT_STAT
	{
		_int iLevel{};
		_int iEXP{}; 

		_int iHP{};
		_int iMP{};

		_float fHPRecoveryP{};
		_float fMPRecoveryP{};

		_int iATKPower{};
		_int iSkillAmp{};
		_int iPenetrationDefense{}; _uint iPenetrationDefensePercent{};
		_int iDefense{};
		_float fATKSpeed{};
		_int iCoolDown{};
		_int iCritical{};
		_float fSpeed{};

		UNIT_STAT& operator += (const UNIT_STAT& rhs) {
			iLevel += rhs.iLevel;					iEXP += rhs.iEXP;

			iHP += rhs.iHP;							iMP += rhs.iMP;
			fHPRecoveryP += rhs.fHPRecoveryP;		fMPRecoveryP += rhs.fMPRecoveryP;

			iATKPower += rhs.iATKPower;				iSkillAmp += rhs.iSkillAmp;
			iPenetrationDefense += rhs.iPenetrationDefense;
			iPenetrationDefensePercent += rhs.iPenetrationDefensePercent;
			iDefense += rhs.iDefense;				fATKSpeed += rhs.fATKSpeed;
			iCoolDown += rhs.iCoolDown;				iCritical += rhs.iCritical;
			fSpeed += rhs.fSpeed;
			return *this;
		}

		UNIT_STAT& operator -= (const UNIT_STAT& rhs)
		{
			iLevel -= rhs.iLevel;                    iEXP -= rhs.iEXP;

			iHP -= rhs.iHP;                          iMP -= rhs.iMP;
			fHPRecoveryP -= rhs.fHPRecoveryP;        fMPRecoveryP -= rhs.fMPRecoveryP;

			iATKPower -= rhs.iATKPower;              iSkillAmp -= rhs.iSkillAmp;
			iPenetrationDefense -= rhs.iPenetrationDefense;
			iPenetrationDefensePercent -= rhs.iPenetrationDefensePercent;
			iDefense -= rhs.iDefense;                fATKSpeed -= rhs.fATKSpeed;
			iCoolDown -= rhs.iCoolDown;              iCritical -= rhs.iCritical;
			fSpeed -= rhs.fSpeed;

			return *this;
		}
	};

private:
	struct RECOVERY
	{
		_float fRemainTime{ 15.f };
		_float fAccTime{ 0.f };

		_uint iRemainHPRecovery{};
		_uint iHPRecoveryPerSec{};
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

	void Add_Recovery(_uint iConsumableHP);

	virtual void Damaged(const DAMAGE_INFO& tDamageInfo);

	_bool XM_CALLCONV IsInRange(_fvector vPos, _float fRange);

	_bool Get_IsInvincible() { return m_bIsInvincible; }

protected:
	void LevelUp();

	void SetStat(UNIT_STAT& tStat,
		_int iLevel, _int iExp, _int iHp, _int iMp,
		_float fHpRec, _float fMpRec, _int iATK, _int iSkillAmp,
		_int iPen, _int iPenPct, _int iDef,
		_float fATKSpeed, _int iCool, _int iCrit, _float fSpeed);

	void AddStat(UNIT_STAT& tStat, const UNIT_STAT& tAddStat, _bool bAdd);

	void SetFinalStat();

	virtual HRESULT Initialize_Stat();

	void Update_Recovery(_float fTimeDelta);

protected:
	UNIT_STAT m_tFinalStat{}; // Base와 Item의 합산
	UNIT_STAT m_tBaseStat{};

	UNIT_STAT m_tItemStat{};

	UNIT_STAT m_tLevelUpStat{}; // 레벨업 할 때 base와 cur에 더해줘야 할 값

	UNIT_STAT m_tCurStat{};

	MAINGAUGE_TYPE m_eMPType{};

	vector<RECOVERY> m_Recoverys;

	_float m_fAttackRange{};

	_bool m_bIsInvincible{};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free() override;
};

NS_END